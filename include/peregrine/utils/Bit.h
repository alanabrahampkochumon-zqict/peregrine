#pragma once
/**
 * @file Bit.h
 * @author Alan Abraham P Kochumon
 * @date Created on: September 09, 2026
 *
 * @brief Bit Manipulation helpers.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <bit>
#include <concepts>

namespace pmm::utils
{

    /**
     * Find Last Set.
     * Return the index of MSB(Most Significant Bit) set to 1.
     *
     * @return The zero-based index of MSB set to 1.
     *
     * @warning Passing in zero is undefined.
     *
     * @code
     * // Example
     * fls(0b00001100); // 3
     * fls(0b10001100); // 7
     * fls(0b10000000); // 7
     * fls(0b00000001); // 0
     * fls(0b00000000); // UB
     * @endcode
     *
     * @relatedalso ffs(const T)
     */
    template <typename T>
        requires std::integral<T>
    constexpr size_t fls(const T num) noexcept
    {
        // Bit width returns the minimum number of bits required to represent
        // the number and subtracting one from it returns the index(0-based)
        // of index of MSB set to 1.
        if constexpr (std::is_signed_v<T>)
        {
            // bit_width only works with unsigned types so we need to get the unsigned type of T
            // and bit_cast it to its signed counterpart before using it with bit_width.
            using UnsignedType = std::make_unsigned_t<T>;
            return static_cast<size_t>(std::bit_width(std::bit_cast<UnsignedType>(num)) - 1);
        }
        else
        {
            return static_cast<size_t>(std::bit_width(num) - 1);
        }
    }


    /**
     * Find First Set.
     * Return the index of LSB(Least Significant Bit) set to 1.
     *
     * @return The zero-based index of LSB set to 1.
     *
     * @warning Passing in zero is undefined.
     *
     * @code
     * // Example
     * ffs(0b00001100); // 2
     * ffs(0b10001100); // 2
     * ffs(0b10000000); // 7
     * ffs(0b00000001); // 0
     * ffs(0b00000000); // UB
     * @endcode
     *
     * @relatedalso fls(const T)
     */
    template <typename T>
        requires std::integral<T>
    constexpr size_t ffs(const T num) noexcept
    {
        // Return the number of zero counted from the left.
        // Since the value returned is a whole number(1 and beyond) we don't need adjust
        // it since it will return the first index of 1.
        // 0011 1000 will return 3 which is the ffs.
        if constexpr (std::is_signed_v<T>)
        {
            // countr_zero only works with unsigned types so we need to get the unsigned type of T
            // and bit_cast it to its signed counterpart before using it with countr_zero.
            using UnsignedType = std::make_unsigned_t<T>;
            return static_cast<size_t>(std::countr_zero(std::bit_cast<UnsignedType>(num)));
        }
        else
        {
            return static_cast<size_t>(std::countr_zero(num));
        }
    }
} // namespace pmm::utils
