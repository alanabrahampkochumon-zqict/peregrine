/**
 * @file ArenaTelemetryTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 30, 2026
 *
 * @brief Verify @ref pmm::ArenaTelemetry creation and update logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/telemetry/ArenaTelemetry.h>


#ifdef ENABLE_PMM_TELEMETRY

/**
 * @addtogroup T_PMM_Telemetry
 * @{
 */

namespace
{
    /**************************************
     *                                    *
     *            TEST SETUP              *
     *                                    *
     **************************************/

    class ArenaTelemetry: public testing::Test
    {
    public:
        const std::size_t SIZE = 1024;
        pmm::ArenaTelemetry telemetry{ SIZE };
    };

} // namespace



/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/** @brief Verify that arena telemetry is initialized with size and usage defaults. */
TEST_F(ArenaTelemetry, IntializesWithSizeAndDefaultStats)
{
    EXPECT_EQ(SIZE, telemetry.getArenaSize());
    EXPECT_EQ(0, telemetry.getCurrentUsage());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinUsage());
    EXPECT_EQ(0, telemetry.getPeakUsage());
}


/** @brief Verify that arena telemetry is updated with correct usage values. */
TEST_F(ArenaTelemetry, UpdateTelemetry_UpdatesWithCorrectUsage)
{
    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);
    telemetry.updateAllocationUsage(20);
    telemetry.updateAllocationUsage(30);
    telemetry.updateAllocationUsage(10);

    EXPECT_EQ(120, telemetry.getCurrentUsage());
    EXPECT_EQ(10, telemetry.getMinUsage());
    EXPECT_EQ(50, telemetry.getPeakUsage());
}


/**
 * @brief Verify that arena telemetry update min usage updates the minimum usage
 *        when passing in a smaller usage value.
 */
TEST_F(ArenaTelemetry, UpdateMinUsage_UpdateMinimumWhenPassingInASmallValue)
{
    constexpr std::size_t newMin = 5;

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);

    telemetry.updateMinUsage(newMin);
    EXPECT_EQ(newMin, telemetry.getMinUsage());
}


/**
 * @brief Verify that arena telemetry update min usage does not update the minimum usage
 *        when passing in a larger usage value.
 */
TEST_F(ArenaTelemetry, UpdateMinUsage_DoesNotUpdateMinimumWhenPassingInALargerValue)
{
    constexpr std::size_t newMin = 50;

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);

    const auto oldMin = telemetry.getMinUsage();

    telemetry.updateMinUsage(newMin);
    EXPECT_EQ(oldMin, telemetry.getMinUsage());
}


/**
 * @brief Verify that arena telemetry update peak usage updates the peak usage
 *        when passing in a larger usage value.
 */
TEST_F(ArenaTelemetry, UpdatePeakUsage_UpdatePeakUsageWhenPassingInALargerValue)
{
    constexpr std::size_t newPeak = 500;

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);

    telemetry.updatePeakUsage(newPeak);
    EXPECT_EQ(newPeak, telemetry.getPeakUsage());
}


/**
 * @brief Verify that arena telemetry update peak usage does not update the peak usage
 *        when passing in a smaller usage value.
 */
TEST_F(ArenaTelemetry, UpdatePeakUsage_DoesNotUpdatePeakUsageWhenPassingInASmallerValue)
{
    constexpr std::size_t newPeak = 15;

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);

    const auto oldMin = telemetry.getPeakUsage();

    telemetry.updatePeakUsage(newPeak);
    EXPECT_EQ(oldMin, telemetry.getPeakUsage());
}


/**
 * @brief Verify that @ref pmm::ArenaTelemetry::resetCurrentUsage resets current usage
 *        but preserves peak and minimum usage.
 */
TEST_F(ArenaTelemetry, ResetCurrentUsage_OnlyResetsCurrentUsage)
{

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);
    telemetry.updateAllocationUsage(20);
    telemetry.updateAllocationUsage(30);
    telemetry.updateAllocationUsage(10);

    // Reset current usage
    telemetry.resetCurrentUsage();

    // Only reset current usage
    EXPECT_EQ(0, telemetry.getCurrentUsage());

    // But preserves the min and peak usage
    EXPECT_EQ(10, telemetry.getMinUsage());
    EXPECT_EQ(50, telemetry.getPeakUsage());
}


/** @brief Verify that arena telemetry reset to default values. */
TEST_F(ArenaTelemetry, Reset_ResetsUsages)
{
    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);
    telemetry.updateAllocationUsage(20);
    telemetry.updateAllocationUsage(30);
    telemetry.updateAllocationUsage(10);

    telemetry.resetTelemetry();
    EXPECT_EQ(0, telemetry.getCurrentUsage());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinUsage());
    EXPECT_EQ(0, telemetry.getPeakUsage());
}

#endif

/** @} */
