#pragma once
/**
 * @file Helpers.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 29, 2026
 *
 * @brief Implementation of functions defined in Helpers.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Preprocessors.h"

#include <bit>


namespace pmm
{
    PMM_INLINE constexpr size_t calcAlignmentPadding(const uintptr_t address, const size_t alignment)
    {
        // To make sure alignment is the power of 2
        PMM_ASSERT_MSG(std::has_single_bit(alignment), "Alignment must be a power of (2, 4, 8...)");

        // Calculate the misalignment
        // & (alignment - 1) skips the need for branching since
        // we are masking-off bits greater than or equal to alignment.
        const auto misalignment = address & (alignment - 1);
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
        return (alignment - misalignment) & (alignment - 1);
    }

} // namespace pmm
