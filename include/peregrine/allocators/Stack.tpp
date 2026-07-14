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


namespace pmm
{
    inline Stack::Stack(const std::size_t sizeInBytes) noexcept
        : _buffer{ new uint8_t[sizeInBytes] }, _size{ sizeInBytes }, _offset{ 0 }
    {}

    constexpr std::size_t Stack::size() const noexcept { return _size; }


    /**************************************
     *                                    *
     *            ALLOCATIONS             *
     *                                    *
     **************************************/

    // TODO: Update tests to include assertions
    // TODO: Out of range
    // TODO: non 2^n alignment
    inline void* Stack::alloc(const std::size_t size, const std::size_t alignment) noexcept
    {
        // TODO: Change from MinStackHeader to appropriate header when using policy
        PMM_ASSERT_MSG(std::has_single_bit(alignment), "Alignment must be a power of 2");
        PMM_ASSERT_MSG(_offset + size + std::max(alignment, sizeof(MinStackHeader)) <= _size,
                       "Stack near capacity. Cannot assign memory!");
        // Align the address
        const auto padding = _alignAddress(alignment);

        const auto currentAddress = _buffer + _offset;
        auto* header = reinterpret_cast<MinStackHeader*>(currentAddress - sizeof(MinStackHeader));
        header->padding = static_cast<uint8_t>(padding);

        _offset += size;
        memset(currentAddress, 0, size); // Zero out memory(TODO: Remove when using HAL)

        return currentAddress; // TODO: Remove
    }


    /**************************************
     *                                    *
     *         PRIVATE HELPERS            *
     *                                    *
     **************************************/
    // Note: Offset is incremented internally, so don't add additional padding.
    inline std::size_t Stack::_alignAddress(const std::size_t alignment) noexcept {
        const auto baseAddress = reinterpret_cast<uintptr_t>(_buffer);
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
            requiredStorage -= requiredPadding; // Calculate the rest of storage needed that is not covered by the padding from alignment
            // Rounding factor is applied to ensure no additional allocation when the storage is a multiple of alignment
            // Like for 8 bytes, we need to round up if storage is less than 8, i.e, 1-7 bytes
            // If the factor is multiple of alignment say, requiredStorage = n * alignment, we can just add it to the padding
            const auto roundingFactor = static_cast<std::size_t>((requiredStorage & (alignment - 1)) != 0);
            requiredPadding += alignment * (roundingFactor + requiredStorage / alignment);
        }
        _offset += requiredPadding;
        return requiredPadding;
    }


} // namespace pmm
