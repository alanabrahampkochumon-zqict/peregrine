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

    /// @test Test fixture for validating @ref pmm::malloc's invalid allocation size triggering process death.
    class PMMMallocDeathTests: public testing::TestWithParam<size_t>
    {};
    INSTANTIATE_TEST_SUITE_P(InvalidSizes, PMMMallocDeathTests, testing::Values(0));

} // namespace



TEST_P(PMMMallocDeathTests, InvalidSizes_TriggersAssertionInDebugMode)
{ EXPECT_DEATH(static_cast<void>(pmm::malloc(GetParam())), ""); }


TEST(PMMMFreeDeathTests, Nullptr_TriggersAssertionInDebugMode) { EXPECT_DEATH(pmm::mfree(nullptr, 0), ""); }



/**************************************
 *         LINUX & MACOS TESTS        *
 **************************************/

    #if defined(PMM_PLATFORM_LINUX) || defined(PMM_PLATFORM_MACOS)
/**
 * @test Verify that freeing a zero size ptr triggers assertion on Linux and MacOS.
 */
TEST(PMMMFreeDeathTests, ZeroSize_TriggersAssertionInDebugMode)
{
    const auto address = pmm::malloc(24);
    EXPECT_DEATH(pmm::mfree(address, 0), "");
}

    #endif



#endif

/** @} */
