#pragma once
/**
 * @file Arena.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Implementation for Linear/Arena allocator member functions defined in
 *        Arena.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "../memory/Memory.h"

#include <bit>
#include <cassert>
#include <cstring>
#include <utility>

namespace pmm
{

    /**************************************
     *           CONSTRUCTORS             *
     **************************************/

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr Arena<MemStrategy, TelPolicy, Safe>::Arena(const size_t arenaSize) noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
        : _buffer(static_cast<uint8_t*>(memAlloc(arenaSize))),
          _arenaSize(arenaSize),
          _offset(0),
          _prevOffset(0),
          _telemetry{ getTelemetryInstance<TelPolicy>(arenaSize) }
    {
        PMM_ASSERT_MSG(arenaSize > 0, "Cannot allocate zero size arena");
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr Arena<MemStrategy, TelPolicy, Safe>::Arena(void* backingBuffer,
                                                                    const size_t bufferSize) noexcept
        requires std::same_as<MemStrategy, UnmanagedMemory>
        : _buffer(static_cast<uint8_t*>(backingBuffer)),
          _arenaSize(bufferSize),
          _offset(0),
          _prevOffset(0),
          _telemetry{ getTelemetryInstance<TelPolicy>(bufferSize) }
    {
        PMM_ASSERT_MSG(backingBuffer != nullptr, "Backing buffer cannot be a nullptr");
        PMM_ASSERT_MSG(bufferSize > 0, "Cannot allocate zero size arena");
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr Arena<MemStrategy, TelPolicy, Safe>::~Arena() noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
    { memFree(_buffer, _arenaSize); }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr Arena<MemStrategy, TelPolicy, Safe>::Arena(Arena&& arena) noexcept
        : _buffer{ std::exchange(arena._buffer, nullptr) },
          _arenaSize{ std::exchange(arena._arenaSize, 0) },
          _offset{ std::exchange(arena._offset, 0) },
          _prevOffset{ std::exchange(arena._prevOffset, 0) },
          _telemetry{ std::exchange(arena._telemetry, getTelemetryInstance<TelPolicy>(_arenaSize)) }
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr Arena<MemStrategy, TelPolicy, Safe>& Arena<MemStrategy, TelPolicy, Safe>::operator=(
        Arena&& arena) noexcept
    {
        // For self assignment return the current arena.
        if (this == &arena)
        {
            return *this;
        }

        if constexpr (std::same_as<MemStrategy, ManagedMemory>)
        {
            memFree(_buffer, _arenaSize);
        }

        // Move the data members and null-out the moved data members.
        _buffer     = std::exchange(arena._buffer, nullptr);
        _offset     = std::exchange(arena._offset, 0);
        _prevOffset = std::exchange(arena._prevOffset, 0);
        _arenaSize  = std::exchange(arena._arenaSize, 0);
        _telemetry  = std::exchange(arena._telemetry, getTelemetryInstance<TelPolicy>(_arenaSize));

        return *this;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr std::size_t Arena<MemStrategy, TelPolicy, Safe>::freeSize() const noexcept
    { return _arenaSize - _offset; }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr std::size_t Arena<MemStrategy, TelPolicy, Safe>::usedSize() const noexcept
    { return _offset; }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr std::size_t Arena<MemStrategy, TelPolicy, Safe>::size() const noexcept
    { return _arenaSize; }


    /**
     * Align the "base address" of the arena's next allocation to @p alignment.
     * @param alignment The alignment to which the offset + base address is aligned to.
     */
    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE void Arena<MemStrategy, TelPolicy, Safe>::_alignForward(const std::size_t alignment) noexcept
    {
        // To make sure alignment is the power of 2
        PMM_ASSERT_MSG(std::has_single_bit(alignment), "Alignment must be a power of 2");
        // Base address of the pointer
        const auto absoluteBaseAddress = reinterpret_cast<uintptr_t>(_buffer);

        // Current memory address
        const auto absoluteAddress = absoluteBaseAddress + _offset;
        const auto misalignment    = absoluteAddress & (alignment - 1);
        // & (alignment - 1) skips the need for branching since
        // we are masking-off bits greater than or equal to alignment.
        // So, when we hit a misalignment of 0, and the result of subtraction go to alignment,
        // essentially cancelling it out to 0
        // E.g: 64 -> 1 0 0 0 0 0 0 (Alignment)
        //      63 -> 0 1 1 1 1 1 1 (Mask)
        //
        //      24 -> 0 0 1 1 0 0 0 (Padding Required)
        //      24 -> 0 0 1 1 0 0 0 (Offset Increment)
        //
        //      64 -> 1 0 0 0 0 0 0 (Padding Required)
        //       0 -> 0 0 0 0 0 0 0 (Offset Increment)
        const auto padding = (alignment - misalignment) & (alignment - 1);
        _telemetry.logPaddingUsage(padding);
        _offset += padding;
    }

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE void* Arena<MemStrategy, TelPolicy, Safe>::allocBytes(const std::size_t sizeInBytes,
                                                                     const std::size_t alignment) noexcept
    {
        _alignForward(alignment);
        PMM_ASSERT_MSG(sizeInBytes > 0 && _arenaSize >= _offset + sizeInBytes, "Arena: Not Enough Memory");
        if constexpr (Safe)
        {
            if (sizeInBytes == 0 || _offset + sizeInBytes > _arenaSize)
            {
                return nullptr;
            }
        }

        void* ptr   = &_buffer[_offset];
        _prevOffset = _offset;
        _offset += sizeInBytes;

        // Update the telemetry usage
        _telemetry.logAllocationUsage(sizeInBytes);

        return ptr;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    template <typename T, typename... Args>
    PMM_INLINE T* Arena<MemStrategy, TelPolicy, Safe>::alloc(Args... args) noexcept
    {
        // Allocate memory in the arena.
        void* raw = allocBytes(sizeof(T), alignof(T));
        if constexpr (Safe)
        {
            if (raw == nullptr)
            {
                return nullptr;
            }
        }


        // Instantiate the object with arguments.
        return new (raw) T(std::forward<Args>(args)...);
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    template <typename T>
    PMM_INLINE std::span<T> Arena<MemStrategy, TelPolicy, Safe>::allocV(std::size_t count) noexcept
    {
        if constexpr (Safe)
        {
            if (_offset + sizeof(T) * count > _arenaSize)
            {
                return std::span<T>();
            }
        }
        // Allocate the raw memory and wrap it in a span
        return std::span<T>(static_cast<T*>(allocBytes(sizeof(T) * count, alignof(T))), count);
    }

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE void Arena<MemStrategy, TelPolicy, Safe>::clear() noexcept
    {
        _offset     = 0;
        _prevOffset = _offset;

        _telemetry.resetCurrentUsage();
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE void Arena<MemStrategy, TelPolicy, Safe>::zeroOut() const noexcept
    { std::memset(_buffer, 0, _arenaSize); }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE void* Arena<MemStrategy, TelPolicy, Safe>::resize(void* oldMemory, const std::size_t oldSize,
                                                                 const std::size_t newSize,
                                                                 const std::size_t alignment) noexcept
    {
        PMM_ASSERT_MSG(oldMemory != nullptr, "Cannot resize a nullptr. Use alloc* variants for fresh allocations.");
        PMM_ASSERT_MSG(oldSize != 0, "Cannot resize 0 bytes of memory.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 bytes. Arena does not support individual frees.");

        if constexpr (Safe)
        {
            if (oldMemory == nullptr || oldSize == 0 || newSize == 0 || !std::has_single_bit(alignment))
            {
                return nullptr;
            }
        }

        // If the new size is smaller than the old size
        // No resizing required
        if (oldSize >= newSize)
        {
            return oldMemory;
        }

        // Check whether the old memory is the last allocation we made
        const auto allocationAddress    = reinterpret_cast<uintptr_t>(oldMemory);
        const auto lastAllocatedAddress = reinterpret_cast<uintptr_t>(_buffer) + _prevOffset;
        const auto offsetDiff           = newSize - oldSize;

        // Lastest allocation safe check
        if constexpr (Safe)
        {
            if (allocationAddress == lastAllocatedAddress && _offset + offsetDiff > _arenaSize)
            {
                return nullptr;
            }
        }
        // If there is enough memory in the arena to "expand" last allocation
        // expand the offset to the difference between new size and old size
        if (allocationAddress == lastAllocatedAddress && _arenaSize >= _offset + offsetDiff)
        {
            _offset += offsetDiff;
            // Update the telemetry to include the difference
            _telemetry.logAllocationUsage(offsetDiff);
            _telemetry.logMinUsage(newSize);
            _telemetry.logPeakUsage(newSize);

            return oldMemory;
        }

        // The memory exists else where in the arena, so create a new byte chunk
        // copy the existing data and return it
        // @note: This leaves a "hole" where the previous allocation was
        PMM_ASSERT_MSG(_arenaSize >= _offset + newSize, "Not enough memory for resize");
        if constexpr (Safe)
        {
            if (_offset + newSize > _arenaSize)
            {
                return nullptr;
            }
        }

        void* newLocation = allocBytes(newSize, alignment);
        memmove(newLocation, oldMemory, oldSize);
        return newLocation;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE void* Arena<MemStrategy, TelPolicy, Safe>::resizeFast(const void* oldMemory, const std::size_t oldSize,
                                                                     const std::size_t newSize,
                                                                     const std::size_t alignment)
    {
        PMM_ASSERT_MSG(
            oldMemory != nullptr,
            "Cannot resize a nullptr. If you want to allocate memory, use alloc<Type>, allocBytes, or allocV instead.");
        PMM_ASSERT_MSG(oldSize != 0, "Cannot resize 0 bytes of memory.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 size. Use `free` to deallocate memory.");
        if constexpr (Safe)
        {
            if (oldMemory == nullptr || oldSize == 0 || newSize == 0 || !std::has_single_bit(alignment))
            {
                return nullptr;
            }
        }

        const auto newPtr = allocBytes(newSize, alignment);
        if constexpr (Safe)
        {
            if (newPtr == nullptr)
            {
                return nullptr;
            }
        }
        memmove(newPtr, oldMemory, oldSize);

        return newPtr;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr const ArenaTelemetryType<TelPolicy>& Arena<MemStrategy, TelPolicy, Safe>::getTelemetry()
        const noexcept
    { return _telemetry; }

} // namespace pmm
