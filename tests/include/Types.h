#pragma once
/**
 * @file Types.h
 * @author Alan Abraham P Kochumon
 * @date Created on: September 09, 2026
 *
 * @brief Type aliases for GTest's TYPED_TESTS.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstdint>
#include <gtest/gtest.h>


using TestIntegralTypes = testing::Types<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>;
