#pragma once
/**
 * @file Stack.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Implementation of member functions defined in Stack.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */



#include "peregrine/memory/Memory.h"
#include "peregrine/utils/Preprocessors.h"

#include <bit>
#include <cstring>
#include <limits>
#include <type_traits>


namespace pmm
{

    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::Stack(
        const std::size_t stackSize) noexcept
        requires(MemoryPolicy == MemPolicy::Internal)
        : _buffer{ static_cast<uint8_t*>(memAlloc(stackSize)) },
          _stackSize{ stackSize },
          _prevOffset{},
          _telemetry{ getTelemetryInstance<TelemetryPolicy>(stackSize) }
    { PMM_ASSERT_MSG(stackSize > 0, "Cannot allocate a zero size stack"); }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::Stack(
        uint8_t* buffer, const std::size_t bufferSize) noexcept
        requires(MemoryPolicy == MemPolicy::External)
        : _buffer{ buffer },
          _stackSize{ bufferSize },
          _prevOffset{},
          _telemetry{ getTelemetryInstance<TelemetryPolicy>(bufferSize) }
    {
        PMM_ASSERT_MSG(bufferSize > 0, "Cannot allocate a zero size stack");
        PMM_ASSERT_MSG(buffer != nullptr, "Stack backing buffer must not be a nullptr");
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::Stack(
        Stack&& stack) noexcept
        : _buffer{ std::exchange(stack._buffer, nullptr) },
          _stackSize{ stack._stackSize },
          _offset{ stack._offset },
          _telemetry{ std::exchange(stack._telemetry, getTelemetryInstance<TelemetryPolicy>(_stackSize)) }
    {
        if constexpr ((Type == StackPolicy::Strict))
        {
            _prevOffset = stack._prevOffset;
        }
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>& Stack<
        Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::operator=(Stack&& stack) noexcept
    {
        // For self assignment return the current arena.
        if (this == &stack)
        {
            return *this;
        }


        if constexpr (MemoryPolicy == MemPolicy::Internal)
        {
            // Release the buffer held by the current arena (ONLY applicable for managed arena)
            memFree(_buffer, _stackSize);
        }


        // Move the data members and null-out the moved data members.
        _buffer    = std::exchange(stack._buffer, nullptr);
        _offset    = stack._offset;
        _stackSize = stack._stackSize;
        _telemetry = std::exchange(stack._telemetry, getTelemetryInstance<TelemetryPolicy>(_stackSize));
        if constexpr ((Type == StackPolicy::Strict))
        {
            _prevOffset = stack._prevOffset;
        }

        return *this;
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr std::size_t Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::size()
        const noexcept
    { return _stackSize; }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr std::size_t Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode,
                                           MultithreadingPolicy>::freeSize() const noexcept
    { return _stackSize - _offset; }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr std::size_t Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode,
                                           MultithreadingPolicy>::usedSize() const noexcept
    { return _offset; }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr bool Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode,
                                    MultithreadingPolicy>::isTelemetryEnabled() noexcept
    { return TelemetryPolicy == TelPolicy::Enabled; }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr const StackTelemetryType<TelemetryPolicy>& Stack<
        Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::getTelemetry() const noexcept
    { return _telemetry; }




    /**************************************
     *                                    *
     *            ALLOCATIONS             *
     *                                    *
     **************************************/

    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::allocBytes(
        const std::size_t size, const std::size_t alignment) noexcept
        requires(Type == StackPolicy::Loose)
    {
        PMM_ASSERT_MSG(std::has_single_bit(alignment) && alignment != 1, "Alignment must be a power of 2");

        const auto padding = _calcAlignment(alignment);
        PMM_ASSERT_MSG(_offset + size + padding <= _stackSize, "Stack capacity exceeded. Cannot assign memory!");
        // NOTE: This assertion is redundant since we are using size_t and it's impossible to pad that amount of memory
        // realistically
        PMM_ASSERT_MSG(padding <= std::numeric_limits<decltype(LooseStackHeader::padding)>::max(),
                       "Alignment exceeded maximum permissible size of padding.");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (!std::has_single_bit(alignment) || alignment == 1 || _offset + size + padding > _stackSize)
            {
                return nullptr;
            }
        }

        // Move the offset to aligned address
        _offset += padding;

        // Store the header behind the allocated address
        const auto currentAddress = _buffer + _offset;
        auto* header              = reinterpret_cast<LooseStackHeader*>(currentAddress - sizeof(LooseStackHeader));
        header->padding           = padding;

        _offset += size;
        if constexpr (TelemetryPolicy == TelPolicy::Enabled)
        {
            _telemetry.incStackUsage(size, padding);
        }
        return currentAddress;
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::allocBytes(
        const std::size_t size, const std::size_t alignment) noexcept
        requires(Type == StackPolicy::Strict)
    {
        PMM_ASSERT_MSG(std::has_single_bit(alignment) && alignment != 1, "Alignment must be a power of 2");

        const auto padding = _calcAlignment(alignment);
        PMM_ASSERT_MSG(_offset + size + padding <= _stackSize, "Stack capacity exceeded. Cannot assign memory!");
        // NOTE: This assertion is redundant since we are using size_t and it's impossible to pad that amount of memory
        // realistically
        PMM_ASSERT_MSG(padding <= std::numeric_limits<decltype(StrictStackHeader::padding)>::max(),
                       "Alignment exceeded maximum permissible size of padding.");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (!std::has_single_bit(alignment) || alignment == 1 || _offset + size + padding > _stackSize)
            {
                return nullptr;
            }
        }
        // Store the current allocation's previous offset
        auto prevAllocOffset = _prevOffset;

        // Move the offsets
        _prevOffset = _offset;
        _offset += padding;

        // Store the header behind the allocated address
        const auto currentAddress = _buffer + _offset;
        auto* header              = reinterpret_cast<StrictStackHeader*>(currentAddress - sizeof(StrictStackHeader));
        header->padding           = padding;
        header->prevOffset        = prevAllocOffset;

        _offset += size;
        if constexpr (TelemetryPolicy == TelPolicy::Enabled)
        {
            _telemetry.incStackUsage(size, padding);
        }
        return currentAddress;
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T, typename... Args>
    PMM_INLINE T* Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::alloc(
        Args... args) noexcept
    {
        auto rawMemory = allocBytes(sizeof(T), alignof(T));
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (rawMemory == nullptr)
            {
                return nullptr;
            }
        }
        return new (rawMemory) T(std::forward<Args>(args)...);
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T>
    PMM_INLINE std::span<T> Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::allocV(
        std::size_t count) noexcept
    {
        PMM_ASSERT_MSG(count > 0, "[Stack]: Cannot allocate an array of size 0");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (_offset + sizeof(T) * count > _stackSize || count == 0)
            {
                return std::span<T>();
            }
        }
        return std::span(static_cast<T*>(allocBytes(sizeof(T) * count, alignof(T))), count);
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::resize(
        void* oldMemory, const std::size_t oldSize, const std::size_t newSize, const std::size_t alignment)
        requires(Type == StackPolicy::Loose)
    {
        PMM_ASSERT_MSG(
            oldMemory != nullptr,
            "Cannot resize a nullptr. If you want to allocate memory, use alloc<Type>, allocBytes, or allocV instead.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 size. Use `free` to deallocate memory.");
        PMM_ASSERT_MSG(oldSize != 0, "Cannot resize from 0 size.");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (oldMemory == nullptr || newSize == 0 || oldSize == 0 || std::has_single_bit(alignment) || alignment < 2)
            {
                return nullptr;
            }
        }

        // If the current allocation requires a resize to a smaller buffer
        if (oldSize >= newSize)
        {
            return oldMemory; // Return the old address
        }

        // Else make new allocations
        auto newPtr = allocBytes(newSize, alignment);
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (newPtr == nullptr)
            {
                return nullptr;
            }
        }
        return memmove(newPtr, oldMemory, oldSize);
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::resize(
        void* oldMemory, const std::size_t oldSize, const std::size_t newSize, const std::size_t alignment)
        requires(Type == StackPolicy::Strict)
    {
        PMM_ASSERT_MSG(
            oldMemory != nullptr,
            "Cannot resize a nullptr. If you want to allocate memory, use alloc<Type>, allocBytes, or allocV instead.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 size. Use `free` to deallocate memory.");
        PMM_ASSERT_MSG(oldSize != 0, "Cannot resize from 0 size.");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (oldMemory == nullptr || newSize == 0 || oldSize == 0 || !std::has_single_bit(alignment) ||
                alignment < 2)
            {
                return nullptr;
            }
        }
        // Used for comparing if two offsets are matching giving us whether the resize is of the latest
        // allocation
        const auto currentOffset = reinterpret_cast<uintptr_t>(oldMemory) - reinterpret_cast<uintptr_t>(_buffer);

        // If the current allocation requires a resize to a smaller buffer
        // Since current offset retrieve the top of the start address exclusive of header size
        // We need to account for that.
        const std::size_t isLatestAllocation = _prevOffset == currentOffset - sizeof(StrictStackHeader);
        if (oldSize >= newSize)
        {
            // Move the offset by the difference
            // If the allocation is not the latest don't move the offset.
            _offset -= isLatestAllocation * (oldSize - newSize);
            if constexpr (TelemetryPolicy == TelPolicy::Enabled)
            {
                _telemetry.decStackUsage(isLatestAllocation * (oldSize - newSize), 0);
            }
            return oldMemory; // Return the old address
        }

        // Larger allocation can either be added to (in case of lastest allocation)
        // or provided with a new memory address
        if (isLatestAllocation)
        {
            PMM_ASSERT_MSG(_offset + (newSize - oldSize) <= _stackSize, "Insufficient memory for resize.");
            if constexpr (SafeMode == SafeModePolicy::Safe)
            {
                if (_offset + (newSize - oldSize) > _stackSize)
                {
                    return nullptr;
                }
            }
            _offset += newSize - oldSize; // Size difference
            if constexpr (TelemetryPolicy == TelPolicy::Enabled)
            {
                _telemetry.incStackUsage(newSize - oldSize, 0);
            }
            return oldMemory;
        }

        auto newPtr = allocBytes(newSize, alignment);
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (newPtr == nullptr)
            {
                return nullptr;
            }
        }
        return memmove(newPtr, oldMemory, oldSize);
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::resizeFast(
        const void* oldMemory, const std::size_t oldSize, const std::size_t newSize, const std::size_t alignment)
    {
        PMM_ASSERT_MSG(
            oldMemory != nullptr,
            "Cannot resize a nullptr. If you want to allocate memory, use alloc<Type>, allocBytes, or allocV instead.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 size. Use `free` to deallocate memory.");
        PMM_ASSERT_MSG(oldSize != 0, "Cannot resize from 0 size.");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (oldMemory == nullptr || newSize == 0 || oldSize == 0 || std::has_single_bit(alignment) || alignment < 2)
            {
                return nullptr;
            }
        }

        auto newPtr = allocBytes(newSize, alignment);
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (newPtr == nullptr)
            {
                return nullptr;
            }
        }

        return memmove(newPtr, oldMemory, oldSize);
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::resizeLast(
        void* oldMemory, const std::size_t oldSize, const std::size_t newSize)
        requires(Type == StackPolicy::Loose)
    {
        PMM_ASSERT_MSG(
            oldMemory != nullptr,
            "Cannot resize a nullptr. If you want to allocate memory, use alloc<Type>, allocBytes, or allocV instead.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 size. Use `free` to deallocate memory.");
        PMM_ASSERT_MSG(oldSize != 0, "Cannot resize from 0 size.");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (oldMemory == nullptr || newSize == 0 || oldSize == 0 ||
                (newSize > oldSize && _offset + (newSize - oldSize) > _stackSize))
            {
                return nullptr;
            }
        }
        // Move the forward or backward depending on the new size.
        // Although all the operands are unsigned, even if oldSize is larger(result in negative result)
        // offset will move backward or forward, in the correct direction. (TESTED)
        _offset += newSize - oldSize;
        if constexpr (TelemetryPolicy == TelPolicy::Enabled)
        {
            if (newSize > oldSize)
            {
                _telemetry.incStackUsage(newSize - oldSize, 0);
            }
            else
            {
                _telemetry.decStackUsage(oldSize - newSize, 0);
            }
        }
        return oldMemory;
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::resizeLast(
        void* oldMemory, const std::size_t oldSize, const std::size_t newSize)
        requires(Type == StackPolicy::Strict)
    {
        PMM_ASSERT_MSG(
            oldMemory != nullptr,
            "Cannot resize a nullptr. If you want to allocate memory, use alloc<Type>, allocBytes, or allocV instead.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 size. Use `free` to deallocate memory.");
        PMM_ASSERT_MSG(oldSize != 0, "Cannot resize from 0 size.");
        PMM_ASSERT_MSG(reinterpret_cast<uintptr_t>(oldMemory) ==
                           reinterpret_cast<uintptr_t>(_buffer) + _prevOffset + sizeof(StrictStackHeader),
                       "Out-of-order resize. resizeLast will only allow resizing the latest allocation.");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (oldMemory == nullptr || newSize == 0 || oldSize == 0 ||
                reinterpret_cast<uintptr_t>(oldMemory) !=
                    reinterpret_cast<uintptr_t>(_buffer) + _prevOffset + sizeof(StrictStackHeader) ||
                (newSize > oldSize && _offset + (newSize - oldSize) > _stackSize))
            {
                return nullptr;
            }
        }
        // Move the forward or backward depending on the new size.
        // Although all the operands are unsigned, even if oldSize is larger(result in negative result)
        // offset will move backward or forward, in the correct direction. (TESTED)
        _offset += newSize - oldSize;
        if constexpr (TelemetryPolicy == TelPolicy::Enabled)
        {
            if (newSize > oldSize)
            {
                _telemetry.incStackUsage(newSize - oldSize, 0);
            }
            else
            {
                _telemetry.decStackUsage(oldSize - newSize, 0);
            }
        }
        return oldMemory;
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE bool Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::freeBytes(
        void* ptr) noexcept
        requires(Type == StackPolicy::Loose)
    {
        PMM_ASSERT_MSG(ptr != nullptr, "Cannot free a nullptr");
        PMM_ASSERT_MSG(ptr >= _buffer + sizeof(LooseStackHeader) && ptr <= _buffer + _offset, "Out-of-bounds free!");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (ptr == nullptr || ptr < _buffer + sizeof(LooseStackHeader) || ptr > _buffer + _offset)
            {
                return false;
            }
        }

        const auto header = reinterpret_cast<LooseStackHeader*>(static_cast<char*>(ptr) - sizeof(LooseStackHeader));
        // Previous offset is the current ptr's position - whatever space we assigned for padding
        const auto prevOffset =
            reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(_buffer) - header->padding;

        // Move the pointer back to the previous offset.
        if constexpr (TelemetryPolicy == TelPolicy::Enabled)
        {
            const auto padding = header->padding;
            // Since offset-prevOffset includes the padding, we need remove the padding to ensure that
            // size and padding are independently decremented
            const auto allocationSize = _offset - prevOffset - padding;
            _telemetry.decStackUsage(allocationSize, padding);
        }
        _offset = prevOffset;
        return true;
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE bool Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::freeBytes(
        void* ptr) noexcept
        requires(Type == StackPolicy::Strict)
    {
        PMM_ASSERT_MSG(ptr != nullptr, "Cannot free a nullptr");
        PMM_ASSERT_MSG(ptr >= _buffer + sizeof(StrictStackHeader) && ptr <= _buffer + _offset, "Out-of-bounds free!");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (ptr == nullptr || ptr < _buffer + sizeof(StrictStackHeader) || ptr > _buffer + _offset)
            {
                return false;
            }
        }
        const auto header = reinterpret_cast<StrictStackHeader*>(static_cast<char*>(ptr) - sizeof(StrictStackHeader));
        // Previous offset is the current ptr's position - whatever space we assigned for padding
        const auto currentBlockStart =
            reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(_buffer) - header->padding;

        PMM_ASSERT_MSG(_prevOffset == currentBlockStart, "Out of order stack free!");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (_prevOffset != currentBlockStart)
            {
                return false;
            }
        }
        // Move the pointer back to the previous offset, and then by the header size.
        if constexpr (TelemetryPolicy == TelPolicy::Enabled)
        {
            _telemetry.decStackUsage(_offset - currentBlockStart - header->padding, header->padding);
        }
        _offset     = currentBlockStart;
        _prevOffset = header->prevOffset;
        return true;
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T>
    PMM_INLINE bool Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::free(T* ptr) noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            ptr->~T();
        }
        return freeBytes(ptr);
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T>
    PMM_INLINE bool Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::freeV(
        std::span<T> vector) noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (auto& item : vector)
            {
                item.~T();
            }
        }
        return freeBytes(vector.data());
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::clear()
    {
        _offset = 0;
        if constexpr (Type == StackPolicy::Strict)
        {
            _prevOffset = 0;
        }
        if constexpr (TelemetryPolicy == TelPolicy::Enabled)
        {
            _telemetry.resetCurrentUsage();
        }
    }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::zeroOut() const noexcept
    { std::memset(_buffer, 0, _stackSize); }


    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::~Stack() noexcept
        requires(MemoryPolicy == MemPolicy::Internal)
    { memFree(_buffer, _stackSize); }



    /**************************************
     *                                    *
     *         PRIVATE HELPERS            *
     *                                    *
     **************************************/

    template <StackPolicy Type, MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr std::size_t Stack<Type, MemoryPolicy, TelemetryPolicy, SafeMode,
                                           MultithreadingPolicy>::_calcAlignment(const std::size_t alignment) noexcept
    {
        const auto baseAddress    = reinterpret_cast<uintptr_t>(_buffer);
        const auto currentAddress = baseAddress + _offset;

        // Alignment - (Address % alignment)
        // Address % alignment gives misalignment
        // Subtracting it from alignment gives the padding to make it aligned
        // Since we need at least alignment amount of storage for header or more
        // We don't need to check for making modulo == 0
        auto requiredPadding = alignment - (currentAddress & (alignment - 1));
        auto requiredStorage = sizeof(LooseStackHeader);

        // Change required storage based on header type
        if constexpr ((Type == StackPolicy::Strict))
        {
            requiredStorage = sizeof(StrictStackHeader);
        }

        // TODO: Try to eliminate conditionals
        if (requiredPadding < requiredStorage)
        {
            requiredStorage -= requiredPadding; // Calculate the rest of storage needed that is not covered by the
                                                // padding from alignment
            // Rounding factor is applied to ensure no additional allocation when the storage is a multiple of alignment
            // Like for 8 bytes, we need to round up if storage is less than 8, i.e, 1-7 bytes
            // If the factor is multiple of alignment say, requiredStorage = n * alignment, we can just add it to the
            // padding
            const auto roundingFactor = static_cast<std::size_t>((requiredStorage & (alignment - 1)) != 0);
            requiredPadding += alignment * (roundingFactor + requiredStorage / alignment);
        }
        return requiredPadding;
    }


} // namespace pmm
