#pragma once
/**
 * @file Vec2.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 18, 2026
 *
 * @brief 2D Vector.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

namespace math
{
    struct Vec2
    {
        float x, y;

        constexpr Vec2(float x, float y) noexcept;
    };



    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    // IMPLEMENTATIONS
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-

    constexpr Vec2::Vec2(const float x, const float y) noexcept: x(x), y(y) {}
} // namespace math
