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

#include "Vec2.h"

#include <cmath>


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

        /// @brief Add two vector together and return a new vector containing the vector sum.
        constexpr Vec3 operator+=(const Vec3& other) noexcept;

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

        /// @brief Compute the squared magnitude of this vector.
        /// @relatedalso mag()
        [[nodiscard]] constexpr float magSq() const noexcept;

        /// @brief Compute the magnitude of this vector.
        /// @relatedalso magSq()
        [[nodiscard]] float mag() const noexcept;

        /// @brief Compute the squared distance between two vector.
        [[nodiscard]] constexpr float distSq(const Vec3& other) const noexcept;

        /// @brief Compute the distance between two vector.
        [[nodiscard]] float dist(const Vec3& other) const noexcept;

        /// @brief Normalize this vector and return a new vector.
        [[nodiscard]] Vec3 normalize() const noexcept;

        /// @brief Get the xy component of the vector as a 2D vector.
        [[nodiscard]] constexpr Vec2 xy() const noexcept;
    };



    //+=+=+=+=+=+=+=+=+=
    // IMPLEMENTATIONS
    //+=+=+=+=+=+=+=+=+=

    constexpr Vec3::Vec3(float x, float y, float z) noexcept: x{ x }, y{ y }, z{ z } {}

    constexpr Vec3 Vec3::operator+(const Vec3& other) const noexcept
    { return Vec3{ x + other.x, y + other.y, z + other.z }; }


    constexpr Vec3 Vec3::operator+=(const Vec3& other) noexcept
    {
        *this = *this + other;
        return *this;
    }

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

    constexpr float Vec3::magSq() const noexcept { return dot(*this); }

    inline float Vec3::mag() const noexcept { return std::sqrt(magSq()); }

    constexpr float Vec3::distSq(const Vec3& other) const noexcept
    {
        const auto dx = other.x - x;
        const auto dy = other.y - y;
        const auto dz = other.z - z;
        return dx * dx + dy * dy + dz * dz;
    }


    constexpr Vec2 Vec3::xy() const noexcept { return Vec2{ x, y }; }

    inline float Vec3::dist(const Vec3& other) const noexcept { return std::sqrt(distSq(other)); }

    inline Vec3 Vec3::normalize() const noexcept { return *this / mag(); }



} // namespace psm
