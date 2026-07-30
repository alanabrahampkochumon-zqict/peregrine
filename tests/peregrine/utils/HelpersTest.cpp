/**
 * @file HelpersTest.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 29, 2026
 *
 * @brief Verifies peregrine's helper function logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/utils/Helpers.h>
#include "Mocks.h"


/**
 * @addtogroup T_PMM_Helpers
 * @{
 */

namespace
{
    /**************************************
     *                                    *
     *             TEST SETUP             *
     *                                    *
     **************************************/

    /// @test Structure for grouping parameters for alignment padding calculation tests.
    struct AlignmentPaddingParams
    {
        size_t unAlignedSize{ 0 }, alignment{ 0 }, paddingRequired{ 0 };
    };
    /// @test Test fixture for alignment padding calculation.
    class AlignmentPadding: public testing::TestWithParam<AlignmentPaddingParams>
    {};
    INSTANTIATE_TEST_SUITE_P(
        PowersOfTwoAlignment, AlignmentPadding,
        testing::Values(AlignmentPaddingParams{ .unAlignedSize = 13, .alignment = 8, .paddingRequired = 3 },
                        AlignmentPaddingParams{ .unAlignedSize = 15, .alignment = 128, .paddingRequired = 113 },
                        AlignmentPaddingParams{ .unAlignedSize = 128, .alignment = 128, .paddingRequired = 0 },
                        AlignmentPaddingParams{ .unAlignedSize = 24, .alignment = 8, .paddingRequired = 0 }));


#ifndef NDEBUG
    INSTANTIATE_TEST_SUITE_P(NonPowersOfTwoAlignment, AlignmentPadding,
                             testing::Values(AlignmentPaddingParams{ 8, 1 }, AlignmentPaddingParams{ 4, 5 },
                                             AlignmentPaddingParams{ 0, 0 }, AlignmentPaddingParams{ 256, 127 }));
#endif


    /// @test Test fixture for alignment padding bad alignments.

} // namespace



TEST_P(AlignmentPadding, ReturnsValidPadding)
{
    const auto& [unAlignedSize, alignment, padding] = GetParam();
    ASSERT_EQ(padding, pmm::calcAlignmentPadding(unAlignedSize, alignment));
}

#ifndef NDEBUG
TEST_P(AlignmentPadding, NonPowerOfTwoAlignment_TriggersAssertion_InDebugMode)
{
    const auto& [unAlignedSize, alignment, padding] = GetParam();
    ASSERT_DEBUG_DEATH(pmm::calcAlignmentPadding(unAlignedSize, alignment), "");
}
#endif

/** @} */
