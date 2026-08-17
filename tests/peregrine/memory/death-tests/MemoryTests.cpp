/**
 * @file MemoryTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 4, 2026
 *
 * @brief Verifies PMM's memory utilities assertions.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/memory/Memory.h>
#include <peregrine/utils/Constants.h>


/**
 * @addtogroup T_PMM_Memory
 * @{
 */

#ifdef ENABLE_PMM_DEATH_TESTS

namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/

    /// @test Test fixture for validating @ref pmm::memAlloc's invalid allocation size triggering process death.
    class MemAllocDeathTests: public testing::TestWithParam<size_t>
    {};
    INSTANTIATE_TEST_SUITE_P(InvalidSizes, MemAllocDeathTests, testing::Values(0));

} // namespace


TEST_P(MemAllocDeathTests, InvalidSizes_TriggersAssertionInDebugMode)
{ EXPECT_DEATH(static_cast<void>(pmm::memAlloc(GetParam())), ""); }

#endif

/** @} */
