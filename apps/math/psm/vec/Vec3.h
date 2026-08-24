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

        /// @brief Compute the dot product of this vector with @p other.
        [[nodiscard]] constexpr float dot(const Vec3& other) const noexcept;

        /// @brief Compute the cross product of this vector with @p other.
        [[nodiscard]] constexpr Vec3 cross(const Vec3& other) const noexcept;

        /// @brief Compute the magnitude of this vector.
        /// @relatedalso magSq()
        [[nodiscard]] constexpr float mag() const noexcept;

        /// @brief Compute the squared magnitude of this vector.
        /// @relatedalso mag()
        [[nodiscard]] constexpr float magSq() const noexcept;
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


    constexpr float Vec3::dot(const Vec3& other) const noexcept { return x * other.x + y * other.y + z * other.z; }

    constexpr Vec3 Vec3::cross(const Vec3& other) const noexcept
    {
        // x -> A_y * B_z - A_z * B_y
        // y -> A_z * B_x - A_x * B_z
        // z -> A_x * B_y - A_y * B_z
        return Vec3{ y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.z };
    }

    constexpr float Vec3::mag() const noexcept { return std::sqrt(dot(*this)); }

    constexpr float Vec3::magSq() const noexcept { return dot(*this); }


} // namespace psm
