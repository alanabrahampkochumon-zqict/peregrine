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



#include "peregrine/utils/Preprocessors.h"

#include <bit>
#include <limits>
#include <new>
#include <type_traits>


namespace pmm
{

    template <stack::StackType Type>
    PMM_INLINE Stack<Type>::Stack(const std::size_t sizeInBytes) noexcept
        : _buffer{ new uint8_t[sizeInBytes] }, _size{ sizeInBytes }
    {}


    template <stack::StackType Type>
    PMM_INLINE constexpr std::size_t Stack<Type>::size() const noexcept
    { return _size; }
    template <stack::StackType Type>
    template <typename T>
    constexpr std::span<T> Stack<Type>::allocV(std::size_t count) noexcept
    {
        PMM_ASSERT_MSG(count > 0, "[Stack]: Cannot allocate an array of size 0");

        auto bytes = static_cast<T*>(allocBytes(sizeof(T) * count, alignof(T)));
        return std::span(bytes, count);
    }



    /**************************************
     *                                    *
     *            ALLOCATIONS             *
     *                                    *
     **************************************/

    template <stack::StackType Type>
    PMM_INLINE void* Stack<Type>::allocBytes(const std::size_t size, const std::size_t alignment) noexcept
        requires std::same_as<Type, stack::Loose>
    {
        PMM_ASSERT_MSG(std::has_single_bit(alignment) && alignment != 1, "Alignment must be a power of 2");

        const auto padding = _calcAlignment(alignment);
        // TODO: Change from MinStackHeader to appropriate header when using policy
        PMM_ASSERT_MSG(_offset + size + padding <= _size, "Stack capacity exceeded. Cannot assign memory!");
        PMM_ASSERT_MSG(padding <= std::numeric_limits<decltype(LooseStackHeader::padding)>::max(),
                       "Alignment exceeded maximum permissible size of padding.");

        // Move the offset to aligned address
        _offset += padding;

        // Store the header behind the allocated address
        const auto currentAddress = _buffer + _offset;
        auto* header              = reinterpret_cast<LooseStackHeader*>(currentAddress - sizeof(LooseStackHeader));
        header->padding           = padding;

        _offset += size;
        memset(currentAddress, 0, size); // Zero out memory(TODO: Remove when using HAL)

        return currentAddress; // TODO: Remove
    }


    template <stack::StackType Type>
    template <typename T, typename... Args>
    T* Stack<Type>::alloc(Args... args) noexcept
        requires std::same_as<Type, stack::Loose>
    {
        auto rawMemory = allocBytes(sizeof(T), alignof(T));

        return new (rawMemory) T(std::forward<Args>(args)...);
    }


    template <stack::StackType Type>
    void* Stack<Type>::resize(void* oldMemory, const std::size_t oldSize, const std::size_t newSize,
                              const std::size_t alignment)
    {
        PMM_ASSERT_MSG(
            oldMemory != nullptr,
            "Cannot resize a nullptr. If you want to allocate memory, use alloc<Type>, allocBytes, or allocV instead.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 size. Use `free` to deallocate memory.");

        // Compare the two offset and if they are equal then this the latest allocation in which case
        const auto targetOffset  = _offset - oldSize;
        const auto currentOffset = reinterpret_cast<uintptr_t>(oldMemory) - reinterpret_cast<uintptr_t>(_buffer);

        // If the current allocation requires a resize to a smaller buffer
        if (oldSize >= newSize)
        {
            if (targetOffset == currentOffset)
            {
                // Move the offset by the difference
                _offset -= oldSize - newSize;
            }
            return oldMemory; // Return the old address
        }

        // Larger allocation can either be added to (in case of lastest allocation)
        // or provided with a new memory address
        if (targetOffset == currentOffset)
        {
            _offset += newSize - oldSize; // Size difference
            return oldMemory;
        }

        auto newPtr = allocBytes(newSize, alignment);
        memmove(newPtr, oldMemory, oldSize);

        return newPtr;
    }


    template <stack::StackType Type>
    void* Stack<Type>::resizeFast(const void* oldMemory, const std::size_t oldSize, const std::size_t newSize,
                                  const std::size_t alignment)
    {
        PMM_ASSERT_MSG(
            oldMemory != nullptr,
            "Cannot resize a nullptr. If you want to allocate memory, use alloc<Type>, allocBytes, or allocV instead.");
        PMM_ASSERT_MSG(newSize != 0, "Cannot resize to 0 size. Use `free` to deallocate memory.");

        auto newPtr = allocBytes(newSize, alignment);
        memmove(newPtr, oldMemory, oldSize);

        return newPtr;
    }


    template <stack::StackType Type>
    PMM_INLINE void Stack<Type>::freeBytes(void* ptr) noexcept
        requires std::same_as<Type, stack::Loose>
    {
        PMM_ASSERT_MSG(ptr != nullptr, "Cannot free a nullptr");
        PMM_ASSERT_MSG(ptr >= _buffer && ptr <= _buffer + _offset, "Out-of-bounds free!");

        // TODO: Disable warning
        const auto header = reinterpret_cast<LooseStackHeader*>(static_cast<char*>(ptr) - sizeof(LooseStackHeader));
        // Previous offset is the current ptr's position - whatever space we assigned for padding
        const auto prevOffset =
            reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(_buffer) - header->padding;
        // Move the pointer back to the previous offset, and then by the header size.
        _offset = prevOffset;
    }


    template <stack::StackType Type>
    template <typename T>
    void Stack<Type>::free(T* ptr) noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            ptr->~T();
        }
        freeBytes(ptr);
    }


    template <stack::StackType Type>
    template <typename T>
    void Stack<Type>::freeV(std::span<T> vector) noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (auto& item : vector)
            {
                item.~T();
            }
        }
        freeBytes(vector.data());
    }


    template <stack::StackType Type>
    void Stack<Type>::freeAll()
    { _offset = 0; }


    template <stack::StackType Type>
    PMM_INLINE Stack<Type>::~Stack() noexcept
    { delete[] _buffer; }



    /**************************************
     *                                    *
     *         PRIVATE HELPERS            *
     *                                    *
     **************************************/

    template <stack::StackType Type>
    PMM_INLINE std::size_t Stack<Type>::_calcAlignment(const std::size_t alignment) noexcept
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
        if constexpr (std::same_as<Type, stack::Strict>)
        {
            requiredStorage = sizeof(StrictStackHeader);
        }

        // TODO: Add tests
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
