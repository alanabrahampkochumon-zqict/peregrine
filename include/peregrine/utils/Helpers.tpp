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


namespace pmm
{
    template <std::integral T>
    PMM_INLINE constexpr T calcAlignmentPadding(T currentBytes, T alignment)
    { return T(currentBytes + alignment); }

} // namespace pmm
