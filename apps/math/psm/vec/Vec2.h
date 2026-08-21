#pragma once
/**
 * @file Vec2.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 21, 2026
 *
 * @brief 2D-vector (float) class.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

// Peregrine Simple Math
namespace psm
{

    struct alignas(16) Vec2
    {
        float x, y;

        Vec2() = default;

        [[nodiscard]] constexpr explicit Vec2(float x, float y) noexcept;

        /// @brief Add two vector together and return a new vector containing the vector sum.
        [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const noexcept;

        /// @brief Subtract a vector from this vector and return a new vector containing the vector difference.
        [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const noexcept;

        /// @brief Multiply this vector by a scalar and return a new scaled vector.
        [[nodiscard]] constexpr Vec2 operator*(float scalar) const noexcept;

        /// @brief Divide this vector by a scalar and return a new inverse scaled vector.
        [[nodiscard]] constexpr Vec2 operator/(float scalar) const noexcept;
    };



    //+=+=+=+=+=+=+=+=+=
    // IMPLEMENTATIONS
    //+=+=+=+=+=+=+=+=+=

    constexpr Vec2::Vec2(const float x, const float y) noexcept: x{ x }, y{ y } {}

    constexpr Vec2 Vec2::operator+(const Vec2& other) const noexcept { return Vec2{ x + other.x, y + other.y }; }

    constexpr Vec2 Vec2::operator-(const Vec2& other) const noexcept { return Vec2{ x - other.x, y - other.y }; }

    constexpr Vec2 Vec2::operator*(const float scalar) const noexcept { return Vec2{ x * scalar, y * scalar }; }

    constexpr Vec2 Vec2::operator/(const float scalar) const noexcept
    {
        const float factor = 1.0f / scalar;
        return *this * factor;
    }


} // namespace psm
