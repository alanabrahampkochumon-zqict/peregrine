#pragma once
/**
 * @file Rotations.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 22, 2026
 *
 * @brief Rotation helpers.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <concepts>
#include <numbers>

namespace math
{
    /**
     * @brief Convert an angle from degrees to radians.
     *
     * @param angle The angle in degrees.
     *
     * @return The angle in radians.
     */
    [[nodiscard]] constexpr float toRad(const float angle) { return std::numbers::inv_pi_v<float> * 180.0f * angle; }


    // /**
    //  * @brief Convert an integral representation of an angle in degrees to radians.
    //  * @tparam T  The numeric type of angle.
    //  * @param angle The angle in degrees.
    //  * @return The angle in radians.
    //  */
    // template <typename T>
    //     requires std::integral<T> || std::floating_point<T>
    // [[nodiscard]] constexpr float operator""_rad(const T angle) noexcept
    // { return std::numbers::inv_pi_v<T> * T(180) * angle; }

} // namespace math
