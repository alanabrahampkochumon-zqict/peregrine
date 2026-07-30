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

    /**
     * @brief Empty struct to enable optimizing away conditional variables.
     */
    struct EmptyMember
    {};


    /**
     * @brief Calculate the padding required to make @p address aligned to @p alignment.
     *
     * @tparam T The numeric type of size and alignment.
     *
     * @param address   The unaligned address start.
     * @param alignment The target alignment.
     *
     * @return The byte difference to add to @p address to make it aligned to @p alignment.
     */
    constexpr size_t calcAlignmentPadding(uintptr_t address, size_t alignment);


    /** @} */

} // namespace pmm


#include "Helpers.tpp"
