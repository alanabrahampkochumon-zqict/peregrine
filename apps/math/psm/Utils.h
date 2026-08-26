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
#include <random>


namespace psm
{
    /// @brief Return whether a number is near zero.
    /// @note For integrals evaluation is based on whether is greater than zero.
    /// @tparam T The numeric type of number.
    template <typename T>
    bool nearZero(const T num) noexcept
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return std::abs(num) < std::numeric_limits<T>::epsilon();
        }
        else
        {
            return num != 0;
        }
    }


    /// @brief Generate a random number between @p from and @p to inclusive.
    /// @tparam T The numeric type of the number to generate.
    template <typename T>
    inline T genRand(const T from, const T to)
    {
        std::random_device rd;
        std::mt19937 generator{ rd() };

        if constexpr (std::is_floating_point_v<T>)
        {
            std::uniform_real_distribution<T> dist{ from, to };
            return dist(generator);
        }
        else
        {
            // const std::uniform_real_distribution<float> dist{ 0.0f, 12.0f };
            std::uniform_int_distribution dist{ from, to };
            return dist(generator);
        }
    }
} // namespace psm
