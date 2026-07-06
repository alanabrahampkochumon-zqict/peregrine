#pragma once
/**
 * @file Utils.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 09, 2026.
 *
 * @brief Utilities and helper for testing.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

struct alignas(16) Vec4
{
    float x, y, z, w;
    Vec4(const float x, const float y, const float z, const float w): x(x), y(y), z(z), w(w) {};
};

