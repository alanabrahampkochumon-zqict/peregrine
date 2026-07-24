/**
 * @file StackTelemetryTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 24, 2026
 *
 * @brief Verify @ref pmm::StackTelemetry creation and update logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/telemetry/StackTelemetry.h>


/**
 * @addtogroup T_PMM_Telemetry
 * @{
 */

#ifdef ENABLE_PMM_TELEMETRY

/** @test Verify that arena telemetry is initialized with size and usage defaults. */
TEST(StackTelemetry, IntializesWithSizeAndDefaultStats)
{
    constexpr auto size = 1024;
    pmm::StackTelemetry telemetry(size);

    EXPECT_EQ(size, telemetry.getStackSize());
    EXPECT_EQ(0, telemetry.getCurrentUsage());
    EXPECT_EQ(0, telemetry.getTotalPadding());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinUsage());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinPaddingUsage());
    EXPECT_EQ(0, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(0, telemetry.getPeakPaddingUsage());
}


/** @test Verify that arena telemetry is incremented with correct usage values. */
TEST(StackTelemetry, IncTelemetryUsage_UpdatesWithCorrectUsage)
{
    constexpr auto size = 1024;
    pmm::StackTelemetry telemetry(size);

    telemetry.incStackUsage(100, 8);
    telemetry.incStackUsage(500, 24);
    telemetry.incStackUsage(200, 100);
    telemetry.incStackUsage(300, 4);
    telemetry.incStackUsage(100, 0);

    EXPECT_EQ(1200, telemetry.getCurrentUsage());
    EXPECT_EQ(100, telemetry.getMinUsage());
    EXPECT_EQ(500, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(1200, telemetry.getCurrentUsage());
    EXPECT_EQ(100, telemetry.getMinUsage());
    EXPECT_EQ(500, telemetry.getPeakMemoryUsage());

}


/**
 * @test Verify that arena telemetry update min usage updates the minimum usage
 *        when passing in a smaller usage value.
 */
TEST(StackTelemetry, UpdateMinUsage_UpdateMinimumWhenPassingInASmallValue)
{
    constexpr auto size = 1024;
    pmm::StackTelemetry telemetry(size);
    constexpr std::size_t newMin = 5;

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);

    telemetry.updateMinMemoryUsage(newMin);
    EXPECT_EQ(newMin, telemetry.getMinUsage());
}


/**
 * @test Verify that arena telemetry update min usage does not update the minimum usage
 *        when passing in a larger usage value.
 */
TEST(StackTelemetry, UpdateMinUsage_DoesNotUpdateMinimumWhenPassingInALargerValue)
{
    constexpr auto size = 1024;
    pmm::StackTelemetry telemetry(size);
    constexpr std::size_t newMin = 50;

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);

    const auto oldMin = telemetry.getMinUsage();

    telemetry.updateMinMemoryUsage(newMin);
    EXPECT_EQ(oldMin, telemetry.getMinUsage());
}


/**
 * @test Verify that arena telemetry update peak usage updates the peak usage
 *        when passing in a larger usage value.
 */
TEST(StackTelemetry, UpdatePeakUsage_UpdatePeakUsageWhenPassingInALargerValue)
{
    constexpr auto size = 1024;
    pmm::StackTelemetry telemetry(size);
    constexpr std::size_t newPeak = 500;

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);

    telemetry.updatePeakMemoryUsage(newPeak);
    EXPECT_EQ(newPeak, telemetry.getPeakMemoryUsage());
}


/**
 * @test Verify that arena telemetry update peak usage does not update the peak usage
 *        when passing in a smaller usage value.
 */
TEST(StackTelemetry, UpdatePeakUsage_DoesNotUpdatePeakUsageWhenPassingInASmallerValue)
{
    constexpr auto size = 1024;
    pmm::StackTelemetry telemetry(size);
    constexpr std::size_t newPeak = 15;

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);

    const auto oldMin = telemetry.getPeakMemoryUsage();

    telemetry.updatePeakMemoryUsage(newPeak);
    EXPECT_EQ(oldMin, telemetry.getPeakMemoryUsage());
}


/**
 * @test Verify that @ref pmm::StackTelemetry::resetCurrentUsage resets current usage
 *        but preserves peak and minimum usage.
 */
TEST(StackTelemetry, ResetCurrentUsage_OnlyResetsCurrentUsage)
{
    constexpr auto size = 1024;
    pmm::StackTelemetry telemetry(size);

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
    EXPECT_EQ(50, telemetry.getPeakMemoryUsage());
}


/** @test Verify that arena telemetry reset to default values. */
TEST(StackTelemetry, Reset_ResetsUsages)
{
    constexpr auto size = 1024;
    pmm::StackTelemetry telemetry(size);

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);
    telemetry.updateAllocationUsage(20);
    telemetry.updateAllocationUsage(30);
    telemetry.updateAllocationUsage(10);

    telemetry.resetTelemetry();
    EXPECT_EQ(0, telemetry.getCurrentUsage());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinUsage());
    EXPECT_EQ(0, telemetry.getPeakMemoryUsage());
}

#endif

/** @} */
