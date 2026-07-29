#pragma once
/**
 * @file Helpers.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 09, 2026
 *
 * @brief Utility and helper functions.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */




#include <concepts>

namespace pmm
{

    /**
     * @addtogroup PMM_Helpers
     * @{
     */


    // TODO: Add group
    /**
     * @brief Empty struct to enable optimizing away conditional variables.
     */
    struct EmptyMember
    {};


    /**
     * @brief Calculate the padding required to make @p currentBytes aligned to @p alignment.
     *
     * @tparam T The numeric type of size and alignment.
     *
     * @param currentBytes The unaligned byte size.
     * @param alignment    The target alignment.
     *
     * @return The byte difference to add to @p currentBytes to make it aligned to @p alignment.
     */
    template <std::integral T>
    constexpr T calcAlignmentPadding(T currentBytes, T alignment);


    /** @} */

} // namespace pmm


#include "Helpers.tpp"
