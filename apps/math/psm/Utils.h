#pragma once
/**
 * @file Utils.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 24, 2026
 *
 * @brief Math Utilities.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <concepts>
#include <limits>

namespace psm
{
    /// @brief Return whether a number is near zero.
    /// @note For integrals evaluation is based on whether is greater than zero.
    /// @tparam T The numeric type of number.
    template <typename T>
    constexpr bool nearZero(const T num) noexcept
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return num > std::numeric_limits<T>::epsilon();
        }
        else
        {
            return num > 0;
        }
    }
} // namespace psm
