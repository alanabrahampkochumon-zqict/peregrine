#pragma once
/**
 * @file Vec3.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 20, 2026
 *
 * @brief 3D-vector (float) class.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

// Peregrine Simple Math
namespace psm
{

    struct alignas(16) Vec3
    {
        float x, y, z;

        Vec3() = default;

        [[nodiscard]] constexpr explicit Vec3(float x, float y, float z) noexcept;

        /// @brief Add two vector together and return a new vector containing the vector sum.
        [[nodiscard]] constexpr Vec3 operator+(const Vec3& other) const noexcept;

        /// @brief Subtract a vector from this vector and return a new vector containing the vector difference.
        [[nodiscard]] constexpr Vec3 operator-(const Vec3& other) const noexcept;

        /// @brief Multiply this vector by a scalar and return a new scaled vector.
        [[nodiscard]] constexpr Vec3 operator*(float scalar) const noexcept;

        /// @brief Divide this vector by a scalar and return a new inverse scaled vector.
        [[nodiscard]] constexpr Vec3 operator/(float scalar) const noexcept;
    };



    //+=+=+=+=+=+=+=+=+=
    // IMPLEMENTATIONS
    //+=+=+=+=+=+=+=+=+=

    constexpr Vec3::Vec3(float x, float y, float z) noexcept: x{ x }, y{ y }, z{ z } {}

    constexpr Vec3 Vec3::operator+(const Vec3& other) const noexcept
    { return Vec3{ x + other.x, y + other.y, z + other.z }; }

    constexpr Vec3 Vec3::operator-(const Vec3& other) const noexcept
    { return Vec3{ x - other.x, y - other.y, z - other.z }; }

    constexpr Vec3 Vec3::operator*(const float scalar) const noexcept
    { return Vec3{ x * scalar, y * scalar, z * scalar }; }

    constexpr Vec3 Vec3::operator/(const float scalar) const noexcept
    {
        const float factor = 1.0f / scalar;
        return *this * factor;
    }


} // namespace psm
