/**
 * @file HelpersTest.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 29, 2026
 *
 * @brief Verifies peregrine's helper function logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"

#include <gtest/gtest.h>
#include <peregrine/utils/Helpers.h>


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
    // NOLINT(modernize-use-cxx17-variable-templates)
    INSTANTIATE_TEST_SUITE_P(
        PowersOfTwoAlignment, AlignmentPadding,
        testing::Values(AlignmentPaddingParams{ .unAlignedSize = 13, .alignment = 8, .paddingRequired = 3 },
                        AlignmentPaddingParams{ .unAlignedSize = 15, .alignment = 128, .paddingRequired = 113 },
                        AlignmentPaddingParams{ .unAlignedSize = 128, .alignment = 128, .paddingRequired = 0 },
                        AlignmentPaddingParams{
                            .unAlignedSize   = 24,
                            .alignment       = 8,
                            .paddingRequired = 0 }));


#ifdef ENABLE_PMM_TESTS
    class NonPowerOfTwoAlignment: public testing::TestWithParam<size_t>
    {};
    // NOLINT(modernize-use-cxx17-variable-templates)
    INSTANTIATE_TEST_SUITE_P(InvalidAlignment, NonPowerOfTwoAlignment, testing::Values(0, 3, 6, 127, 4092));
#endif


    /// @test Test fixture for alignment padding bad alignments.

} // namespace



TEST_P(AlignmentPadding, ReturnsValidPadding)
{
    const auto& [unAlignedSize, alignment, padding] = GetParam();
    ASSERT_EQ(padding, pmm::calcAlignmentPadding(unAlignedSize, alignment));
}

#ifdef ENABLE_PMM_TESTS
TEST_P(NonPowerOfTwoAlignment, NonPowerOfTwoAlignment_TriggersAssertion_InDebugMode)
{
    const auto alignment = GetParam();
    ASSERT_DEBUG_DEATH(pmm::calcAlignmentPadding(2049, alignment), "");
}
#endif

/** @} */
