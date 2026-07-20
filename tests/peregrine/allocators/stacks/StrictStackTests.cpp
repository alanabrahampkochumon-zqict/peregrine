/**
 * @file StrictStackTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Verify @ref pmm::Stack<pmm::stack::Strict> allocation, deallocation, and other related memory management logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"
#include "Utils.h"

#include <array>
#include <gtest/gtest.h>
#include <peregrine/allocators/Stack.h>
#include <peregrine/utils/Constants.h>


/**************************************
 *                                    *
 *             TEST SETUP             *
 *                                    *
 **************************************/
//
// using namespace pmm::constants;
// constexpr static auto STACK_SIZE = 5_KB;
// class StrictStackTests: public ::testing::Test
// {
//
// public:
//     std::size_t stackSize{ STACK_SIZE };
//     pmm::Stack<pmm::stack::Strict> stack{ stackSize };
// };
//
//
// class StrictStackAllocationAlignment: public ::testing::TestWithParam<std::size_t>
// {};
// INSTANTIATE_TEST_SUITE_P(
//     StackAlignmentTests, StrictStackAllocationAlignment,
//     ::testing::Values(4, 8, 16, 32, 64, 512, 4096));
//
// #ifndef NDEBUG
// class StrictStackAllocationAlignmentNonBinaryPowers: public ::testing::TestWithParam<std::size_t>
// {};
// INSTANTIATE_TEST_SUITE_P(NonPowersOfTwo, StrictStackAllocationAlignmentNonBinaryPowers, ::testing::Values(0, 1, 3, 5, 111));
// #endif
//
//
//
// struct StrictStackResizeLastParams
// {
//     std::size_t oldSize, newSize;
// };
// class StrictStackResizeLast: public ::testing::TestWithParam<StrictStackResizeLastParams>
// {};
// INSTANTIATE_TEST_SUITE_P(StackResize, StrictStackResizeLast,
//                          ::testing::Values(StrictStackResizeLastParams{ .oldSize = 256, .newSize = 128 },
//                                            StrictStackResizeLastParams{ .oldSize = 128, .newSize = 512 },
//                                            StrictStackResizeLastParams{ .oldSize = 2048, .newSize = 4096 },
//                                            StrictStackResizeLastParams{ .oldSize = 4096, .newSize = 2048 }));

