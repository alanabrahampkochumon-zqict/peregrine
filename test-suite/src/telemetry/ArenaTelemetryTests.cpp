/**
 * @file ArenaTelemetryTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 30, 2026
 *
 * @brief Verify @ref pmm::ArenaTelemetry creation and update logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#define ENABLE_PMM_TELEMETRY // TODO: Move to CMake of TestSuite


#include <gtest/gtest.h>
#include <telemetry/ArenaTelemetry.h>


/**
 * @addtogroup T_PMM_Telemetry
 * @{
 */

#ifdef ENABLE_PMM_TELEMETRY

/** @brief Verify that arena telemetry is initialized with size and usage defaults. */
TEST(ArenaTelemetry, IntializesWithSizeAndDefaultStats)
{
    constexpr auto size = 1024;
    pmm::ArenaTelemetry telemetry(size);

    EXPECT_EQ(size, telemetry.size);
    EXPECT_EQ(0, telemetry.currentUsage);
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.minUsage);
    EXPECT_EQ(0, telemetry.peakUsage);
}


/** @brief Verify that arena telemetry is updated with correct usage values. */
TEST(ArenaTelemetry, UpdateTelemetry_UpdatesWithCorrectUsage)
{
    constexpr auto size = 1024;
    pmm::ArenaTelemetry telemetry(size);

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);
    telemetry.updateAllocationUsage(20);
    telemetry.updateAllocationUsage(30);
    telemetry.updateAllocationUsage(10);

    EXPECT_EQ(120, telemetry.currentUsage);
    EXPECT_EQ(10, telemetry.minUsage);
    EXPECT_EQ(50, telemetry.peakUsage);
}


/** @brief Verify that arena telemetry reset to default values. */
TEST(ArenaTelemetry, Reset_ResetsUsages)
{
    constexpr auto size = 1024;
    pmm::ArenaTelemetry telemetry(size);

    telemetry.updateAllocationUsage(10);
    telemetry.updateAllocationUsage(50);
    telemetry.updateAllocationUsage(20);
    telemetry.updateAllocationUsage(30);
    telemetry.updateAllocationUsage(10);

    telemetry.resetTelemetry();
    EXPECT_EQ(0, telemetry.currentUsage);
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.minUsage);
    EXPECT_EQ(0, telemetry.peakUsage);
}


#endif

/** @} */
