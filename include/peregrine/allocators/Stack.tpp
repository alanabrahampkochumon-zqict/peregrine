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


namespace pmm
{

    template <StoragePolicy Policy>
    PMM_INLINE Stack<Policy>::Stack(const std::size_t sizeInBytes) noexcept
        : _buffer{ new uint8_t[sizeInBytes] }, _size{ sizeInBytes }, _offset{ 0 }
    {}

    template <StoragePolicy Policy>
    PMM_INLINE constexpr std::size_t Stack<Policy>::size() const noexcept
    { return _size; }



    /**************************************
     *                                    *
     *            ALLOCATIONS             *
     *                                    *
     **************************************/

    template <StoragePolicy Policy>
    PMM_INLINE void* Stack<Policy>::alloc(const std::size_t size, const std::size_t alignment) noexcept
        requires std::same_as<Policy, MinimalStackPolicy>
    {
        PMM_ASSERT_MSG(std::has_single_bit(alignment) && alignment != 1, "Alignment must be a power of 2");

        const auto padding = _calcAlignment(alignment);
        // TODO: Change from MinStackHeader to appropriate header when using policy
        PMM_ASSERT_MSG(_offset + size + padding <= _size, "Stack capacity exceeded. Cannot assign memory!");
        PMM_ASSERT_MSG(padding <= std::numeric_limits<decltype(MinStackHeader::padding)>::max(),
                       "Alignment exceeded maximum permissible size of padding.");

        // Move the offset to aligned address
        _offset += padding;

        // Store the header behind the allocated address
        const auto currentAddress = _buffer + _offset;
        auto* header              = reinterpret_cast<MinStackHeader*>(currentAddress - sizeof(MinStackHeader));
        header->padding           = static_cast<uint8_t>(padding);

        _offset += size;
        memset(currentAddress, 0, size); // Zero out memory(TODO: Remove when using HAL)

        return currentAddress; // TODO: Remove
    }


    // TODO: Add tests
    template <StoragePolicy Policy>
    PMM_INLINE void Stack<Policy>::free(void* ptr) noexcept
        requires std::same_as<Policy, MinimalStackPolicy>
    {
        PMM_ASSERT_MSG(ptr != nullptr, "Cannot free a nullptr");
        PMM_ASSERT_MSG(ptr >= _buffer && ptr <= _buffer + _offset, "Out-of-bounds free!");

        // TODO: Disable warning
        const auto header     = reinterpret_cast<MinStackHeader*>(static_cast<char*>(ptr) - sizeof(MinStackHeader));
        const auto prevOffset = reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(_buffer);
        // Move the pointer back to the previous offset, and then by the header size.
        _offset -= prevOffset + header->padding;

        ptr = nullptr;
    }

    template <StoragePolicy Policy>
    PMM_INLINE Stack<Policy>::~Stack() noexcept
    { delete[] _buffer; }


    /**************************************
     *                                    *
     *         PRIVATE HELPERS            *
     *                                    *
     **************************************/
    // Note: Offset is incremented internally, so don't add additional padding.
    template <StoragePolicy Policy>
    PMM_INLINE std::size_t Stack<Policy>::_calcAlignment(const std::size_t alignment) noexcept
    {
        const auto baseAddress    = reinterpret_cast<uintptr_t>(_buffer);
        const auto currentAddress = baseAddress + _offset;

        // Alignment - (Address % alignment)
        // Address % alignment gives misalignment
        // Subtracting it from alignment gives the padding to make it aligned
        // Since we need at least alignment amount of storage for header or more
        // We don't need to check for making modulo == 0
        auto requiredPadding = alignment - (currentAddress & (alignment - 1));
        auto requiredStorage = sizeof(MinStackHeader); // TODO: Update when reimplementing for Different Header

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
