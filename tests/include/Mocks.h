#pragma once
/**
 * @file Mocks.h
 * @author Alan Abraham P Kochumon
 * @date Created on: July 18, 2026
 *
 * @brief Defines mocks for testing.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <array>
#include <cstddef>
#include <cstdint>


/**
 * @brief Test mock used for tracking number of times struct destructor is called.
 */
struct DestructionTracker
{
    int* destructorCalledCount{};

    explicit DestructionTracker(int* counter): destructorCalledCount(counter) {}

    ~DestructionTracker() { (*destructorCalledCount)++; }
};


/**
 * @brief Mock data structure to store @p Size bytes for data for mocking.
 * @tparam Size The size of the data structure.
 */
template <std::size_t Size>
struct alignas(8) LargeData
{
    std::array<uint8_t, Size> data{};
};
