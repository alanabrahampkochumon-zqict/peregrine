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



/**
 * @addtogroup T_PMM_Telemetry
 * @{
 */

namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/

    class ArenaTelemetryTests: public testing::Test
    {
    public:
        const std::size_t SIZE = 1024;
        pmm::ArenaTelemetry telemetry{ SIZE };
    };

} // namespace



/**************************************
 *           RUNTIME TESTS            *
 **************************************/

TEST_F(ArenaTelemetryTests, Ctor_IntializesWithMembersWithDefaultValues)
{
    EXPECT_EQ(SIZE, telemetry.getArenaSize());
    EXPECT_EQ(0, telemetry.getUsedSize());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinUsage());
    EXPECT_EQ(0, telemetry.getPeakUsage());
}


TEST_F(ArenaTelemetryTests, LogAllocationUsage_UpdatesWithCorrectUsage)
{
    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);
    telemetry.logAllocationUsage(20);
    telemetry.logAllocationUsage(30);
    telemetry.logAllocationUsage(10);

    EXPECT_EQ(120, telemetry.getUsedSize());
    EXPECT_EQ(10, telemetry.getMinUsage());
    EXPECT_EQ(50, telemetry.getPeakUsage());
}


TEST_F(ArenaTelemetryTests, LogMinUsage_UpdatesMinimumWhenPassingInASmallerValue)
{
    constexpr std::size_t newMin = 5;

    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);

    telemetry.logMinUsage(newMin);
    EXPECT_EQ(newMin, telemetry.getMinUsage());
}



TEST_F(ArenaTelemetryTests, LogMinUsage_DoesNotUpdateMinimumWhenPassingInALargerValue)
{
    constexpr std::size_t newMin = 50;

    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);

    const auto oldMin = telemetry.getMinUsage();

    telemetry.logMinUsage(newMin);
    EXPECT_EQ(oldMin, telemetry.getMinUsage());
}


TEST_F(ArenaTelemetryTests, LogPeakUsage_UpdatesPeakUsageWhenPassingInALargerValue)
{
    constexpr std::size_t newPeak = 500;

    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);

    telemetry.logPeakUsage(newPeak);
    EXPECT_EQ(newPeak, telemetry.getPeakUsage());
}


TEST_F(ArenaTelemetryTests, LogPeakUsage_DoesNotUpdatePeakUsageWhenPassingInASmallerValue)
{
    constexpr std::size_t newPeak = 15;

    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);

    const auto oldMin = telemetry.getPeakUsage();

    telemetry.logPeakUsage(newPeak);
    EXPECT_EQ(oldMin, telemetry.getPeakUsage());
}


TEST_F(ArenaTelemetryTests, LogPaddingUsage_IncrementsPaddingSize)
{
    telemetry.logPaddingUsage(10);
    telemetry.logPaddingUsage(40);
    EXPECT_EQ(50, telemetry.getTotalPadding());
}


TEST_F(ArenaTelemetryTests, GetUsedSize_ReturnsTotalAllocationSize)
{
    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);
    EXPECT_EQ(10 + 50, telemetry.getUsedSize());
}


TEST_F(ArenaTelemetryTests, GetFreeSize_ReturnsRemainingSizeAfterAllocation)
{
    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);
    EXPECT_EQ(SIZE - (10 + 50), telemetry.getFreeSize());
}


TEST_F(ArenaTelemetryTests, ResetCurrentUsage_OnlyResetsCurrentUsage)
{

    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);
    telemetry.logAllocationUsage(20);
    telemetry.logAllocationUsage(30);
    telemetry.logAllocationUsage(10);
    telemetry.logPaddingUsage(50);

    // Reset current usage
    telemetry.resetCurrentUsage();

    // Only reset current usage
    EXPECT_EQ(0, telemetry.getUsedSize());
    EXPECT_EQ(0, telemetry.getTotalPadding());

    // But preserves the min and peak usage
    EXPECT_EQ(10, telemetry.getMinUsage());
    EXPECT_EQ(50, telemetry.getPeakUsage());
}


TEST_F(ArenaTelemetryTests, ResetTelemetry_ResetsAllUsageStats)
{
    telemetry.logAllocationUsage(10);
    telemetry.logAllocationUsage(50);
    telemetry.logAllocationUsage(20);
    telemetry.logAllocationUsage(30);
    telemetry.logAllocationUsage(10);
    telemetry.logPaddingUsage(50);

    telemetry.resetTelemetry();
    EXPECT_EQ(0, telemetry.getUsedSize());
    EXPECT_EQ(0, telemetry.getTotalPadding());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinUsage());
    EXPECT_EQ(0, telemetry.getPeakUsage());
}

/** @} */
