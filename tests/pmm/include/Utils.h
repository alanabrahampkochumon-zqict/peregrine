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
    Vec4(const float v0, const float v1, const float v2, const float v3): x(v0), y(v1), z(v2), w(v3) {}
};
