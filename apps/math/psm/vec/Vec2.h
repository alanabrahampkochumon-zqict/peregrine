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

#include <cmath>
#include <random>

// Peregrine Simple Math
namespace psm
{

    struct alignas(16) Vec2
    {
        float x, y;

        Vec2() = default;

        [[nodiscard]] constexpr explicit Vec2(const float x, const float y) noexcept: x{ x }, y{ y } {};

        /// @brief Add two vector together and return a new vector containing the vector sum.
        [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const noexcept;

        /// @brief Add two vector together in-place.
        constexpr Vec2& operator+=(const Vec2& other) noexcept;

        /// @brief Subtract a vector from this vector and return a new vector containing the vector difference.
        [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const noexcept;

        /// @brief Multiply this vector by a scalar and return a new scaled vector.
        [[nodiscard]] constexpr Vec2 operator*(float scalar) const noexcept;

        /// @brief Divide this vector by a scalar and return a new inverse scaled vector.
        [[nodiscard]] constexpr Vec2 operator/(float scalar) const noexcept;

        /// @brief Compute the dot product of this vector with @p other.
        [[nodiscard]] constexpr float dot(const Vec2& other) const noexcept;

        /// @brief Compute the pseudo-cross product of this vector with @p other.
        [[nodiscard]] constexpr float cross(const Vec2& other) const noexcept;

        /// @brief Compute the squared magnitude of this vector.
        /// @relatedalso mag()
        [[nodiscard]] constexpr float magSq() const noexcept;

        /// @brief Compute the magnitude of this vector.
        /// @relatedalso magSq()
        [[nodiscard]] float mag() const noexcept;

        /// @brief Compute the squared distance between two vector.
        [[nodiscard]] constexpr float distSq(const Vec2& other) const noexcept;

        /// @brief Compute the distance between two vector.
        [[nodiscard]] float dist(const Vec2& other) const noexcept;

        /// @brief Normalize this vector and return a new vector.
        [[nodiscard]] Vec2 normalize() const noexcept;

        /**
         * Get a random vector between @p from and @p to.
         *
         * @note Both ranges are inclusive.
         *
         * @param from The lower limit for the random vector.
         * @param to   The upper limit for the random vector.
         * @return A 2D vector with random values between @p from and @p to.
         */
        [[nodiscard]] static Vec2 getRandom(Vec2 from, Vec2 to) noexcept;
    };

    static constexpr Vec2 ZERO{ 0.0f, 0.0f };


    //+=+=+=+=+=+=+=+=+=
    // IMPLEMENTATIONS
    //+=+=+=+=+=+=+=+=+=

    constexpr Vec2 Vec2::operator+(const Vec2& other) const noexcept { return Vec2{ x + other.x, y + other.y }; }

    constexpr Vec2& Vec2::operator+=(const Vec2& other) noexcept
    {
        *this = *this + other;
        return *this;
    }

    constexpr Vec2 Vec2::operator-(const Vec2& other) const noexcept { return Vec2{ x - other.x, y - other.y }; }

    constexpr Vec2 Vec2::operator*(const float scalar) const noexcept { return Vec2{ x * scalar, y * scalar }; }

    constexpr Vec2 Vec2::operator/(const float scalar) const noexcept
    {
        const float factor = 1.0f / scalar;
        return *this * factor;
    }


    constexpr float Vec2::dot(const Vec2& other) const noexcept { return x * other.x + y * other.y; }

    constexpr float Vec2::cross(const Vec2& other) const noexcept
    {
        // Ax * By - Ay * Bx
        return x * other.y - y * other.x;
    }

    constexpr float Vec2::magSq() const noexcept { return dot(*this); }

    inline float Vec2::mag() const noexcept { return std::sqrt(magSq()); }

    constexpr float Vec2::distSq(const Vec2& other) const noexcept
    {
        const auto dx = other.x - x;
        const auto dy = other.y - y;
        return dx * dx + dy * dy;
    }


    inline Vec2 Vec2::getRandom(const Vec2 from, const Vec2 to) noexcept
    {
        std::random_device rd;
        std::mt19937 generator(rd());

        std::uniform_real_distribution xDistribution(from.x, to.x);
        std::uniform_real_distribution yDistribution(from.y, to.y);

        return Vec2{ xDistribution(generator), yDistribution(generator) };
    }


    inline float Vec2::dist(const Vec2& other) const noexcept
    {
        // Note: This can be written as as (a - b).mag()
        return std::sqrt(distSq(other));
    }

    inline Vec2 Vec2::normalize() const noexcept { return *this / mag(); }



} // namespace psm
