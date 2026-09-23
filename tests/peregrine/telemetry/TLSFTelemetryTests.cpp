/**
 * @file TLSFTelemetryTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 22, 2026
 *
 * @brief Verify @ref pmm::TLSFTelemetry creation and update logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/telemetry/TLSFTelemetry.h>


/**
 * @addtogroup T_PMM_Telemetry
 * @{
 */

namespace
{
    /**************************************
     *               SETUP                *
     **************************************/

    class TLSFTelemetryTests: public testing::Test
    {
    public:
        std::size_t size{ 10240 };
        pmm::TLSFTelemetry telemetry{ size };
    };



    /**************************************
     *            STATIC TESTS            *
     **************************************/

    namespace static_tests
    {
        namespace telemetry_type
        {
            /// @test Verify that TLSFTelemetryType returns TLSFTelemetry when the telemetry policy is Managed.
            static_assert(std::same_as<pmm::TLSFTelemetryType<pmm::TelPolicy::Enabled>, pmm::TLSFTelemetry> == true);


            /// @test Verify that TLSFTelemetryType returns DummyTLSFTelemetry when the telemetry policy is Disabled.
            static_assert(std::same_as<pmm::TLSFTelemetryType<pmm::TelPolicy::Disabled>, pmm::DummyTLSFTelemetry> ==
                          true);

        } // namespace telemetry_type


        namespace telemetry_helpers
        {
            /// @test Verify that getTelemetryInstance returns a real TLSF when the telemetry policy is Managed.
            [[maybe_unused]] constexpr auto STACK_TEL_MANAGED = pmm::getTelemetryInstance<pmm::TelPolicy::Enabled>(512);
            static_assert(std::is_same_v<decltype(STACK_TEL_MANAGED), const pmm::TLSFTelemetry> == true);


            /// @test Verify that getTelemetryInstance returns a real TLSF when the telemetry policy is Disabled.
            [[maybe_unused]] constexpr auto STACK_TEL_DISABLED =
                pmm::getTelemetryInstance<pmm::TelPolicy::Disabled>(512);
            static_assert(std::is_same_v<decltype(STACK_TEL_DISABLED), const pmm::DummyTLSFTelemetry> == true);

        } // namespace telemetry_helpers
    } // namespace static_tests
} // namespace




/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/** @test Verify that TLSF telemetry is initialized with size and usage defaults. */
TEST_F(TLSFTelemetryTests, IntializesWithSizeAndDefaultStats)
{
    constexpr auto maxValue = std::numeric_limits<size_t>();
    EXPECT_EQ(size, telemetry.getTotalCapacity());
    EXPECT_EQ(0, telemetry.getCurrentBufferUsage());
    EXPECT_EQ(maxValue, telemetry.getMinBufferUsage());
    EXPECT_EQ(0, telemetry.getPeakBufferUsage());

    EXPECT_EQ(0, telemetry.getCurrentMetadataUsage());
    EXPECT_EQ(maxValue, telemetry.getMinMetadataUsage());
    EXPECT_EQ(0, telemetry.getPeakMetadataUsage());

    EXPECT_EQ(0, telemetry.getCurrentPayloadUsage());
    EXPECT_EQ(maxValue, telemetry.getMinPayloadUsage());
    EXPECT_EQ(0, telemetry.getPeakPayloadUsage());

    EXPECT_EQ(0, telemetry.getLargestFreeBlockSize());
    EXPECT_EQ(0, telemetry.getFreeBlockCount());
    EXPECT_EQ(0, telemetry.getActiveAllocations());
    EXPECT_EQ(0, telemetry.getLifetimeAllocations());
    EXPECT_EQ(0, telemetry.getLifetimeFrees());
}


/** @test Verify that TLSF telemetry is incremented with correct usage values. */
TEST_F(TLSFTelemetryTests, IncTelemetryUsage_UpdatesWithCorrectUsage)
{

    telemetry.incTLSFUsage(100, 8);
    telemetry.incTLSFUsage(500, 24);
    telemetry.incTLSFUsage(200, 100);
    telemetry.incTLSFUsage(300, 4);
    telemetry.incTLSFUsage(100, 0);

    EXPECT_EQ(1200, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(100, telemetry.getMinMemoryUsage());
    EXPECT_EQ(500, telemetry.getPeakMemoryUsage());

    EXPECT_EQ(136, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getMinPadding());
    EXPECT_EQ(100, telemetry.getPeakPadding());

    EXPECT_EQ(1336, telemetry.getTotalUsage());
}


TEST(TLSFTelemetryHelpersTests, ManagedPolicy_ReturnRealTLSFTelemetry) {}


/** @test Verify that TLSF telemetry is decrement with correct usage values. */
TEST_F(TLSFTelemetryTests, DecTelemetryUsage_UpdatesWithCorrectUsage)
{

    telemetry.incTLSFUsage(5000, 120);
    telemetry.decTLSFUsage(500, 24);
    telemetry.decTLSFUsage(100, 12);

    EXPECT_EQ(4400, telemetry.getCurrentMemoryUsage());

    EXPECT_EQ(84, telemetry.getCurrentPadding());

    EXPECT_EQ(4484, telemetry.getTotalUsage());
}


/**
 * @test Verify that TLSF telemetry updateMinMemoryUsage updates the minimum memory usage
 *       when passing in a smaller value.
 */
TEST_F(TLSFTelemetryTests, UpdateMinMemoryUsage_UpdateMinimumWhenPassingInASmallValue)
{
    constexpr std::size_t newMin = 5;

    telemetry.incTLSFUsage(10, 5);
    telemetry.incTLSFUsage(50, 2);

    telemetry.updateMinMemoryUsage(newMin);
    EXPECT_EQ(newMin, telemetry.getMinMemoryUsage());
}


/**
 * @test Verify that TLSF telemetry updateMinMemoryUsage does not update the minimum memory usage
 *       when passing in a larger value.
 */
TEST_F(TLSFTelemetryTests, UpdateMinMemoryUsage_DoesNotUpdateMinimumWhenPassingInALargerValue)
{
    constexpr std::size_t newMin = 50;

    telemetry.incTLSFUsage(10, 2);
    telemetry.incTLSFUsage(40, 2);

    const auto oldMin = telemetry.getMinMemoryUsage();

    telemetry.updateMinMemoryUsage(newMin);
    EXPECT_EQ(oldMin, telemetry.getMinMemoryUsage());
}


/**
 * @test Verify that TLSF telemetry updatePeakMemoryUsage updates the peak memory usage
 *       when passing in a larger value.
 */
TEST_F(TLSFTelemetryTests, UpdatePeakMemoryUsage_UpdatePeakUsageWhenPassingInALargerValue)
{
    constexpr std::size_t newPeak = 500;

    telemetry.incTLSFUsage(10, 2);
    telemetry.incTLSFUsage(50, 2);

    telemetry.updatePeakMemoryUsage(newPeak);
    EXPECT_EQ(newPeak, telemetry.getPeakMemoryUsage());
}


/**
 * @test Verify that TLSF telemetry updatePeakMemoryUsage does not update the peak memory usage
 *       when passing in a smaller value.
 */
TEST_F(TLSFTelemetryTests, UpdatePeakMemoryUsage_DoesNotUpdatePeakUsageWhenPassingInASmallerValue)
{
    constexpr std::size_t newPeak = 15;

    telemetry.incTLSFUsage(10, 2);
    telemetry.incTLSFUsage(50, 2);

    const auto oldMin = telemetry.getPeakMemoryUsage();

    telemetry.updatePeakMemoryUsage(newPeak);
    EXPECT_EQ(oldMin, telemetry.getPeakMemoryUsage());
}


/**
 * @test Verify that TLSF telemetry updateMinPaddingUsage updates the minimum padding
 *       when passing in a smaller value.
 */
TEST_F(TLSFTelemetryTests, UpdateMinUsage_UpdateMinimumPaddingWhenPassingInASmallValue)
{
    constexpr std::size_t newMin = 5;

    telemetry.incTLSFUsage(10, 15);
    telemetry.incTLSFUsage(50, 12);

    telemetry.updateMinPaddingUsage(newMin);
    EXPECT_EQ(newMin, telemetry.getMinPadding());
}


/**
 * @test Verify that TLSF telemetry updateMinPaddingUsage does not update the minimum padding
 *       when passing in a larger value.
 */
TEST_F(TLSFTelemetryTests, UpdateMinUsage_DoesNotUpdateMinimumPaddingWhenPassingInALargerValue)
{
    constexpr std::size_t newMin = 50;

    telemetry.incTLSFUsage(10, 2);
    telemetry.incTLSFUsage(40, 2);

    const auto oldMin = telemetry.getMinPadding();

    telemetry.updateMinPaddingUsage(newMin);
    EXPECT_EQ(oldMin, telemetry.getMinPadding());
}


/**
 * @test Verify that TLSF telemetry updatePeakPaddingUsage updates the peak padding
 *       when passing in a larger value.
 */
TEST_F(TLSFTelemetryTests, UpdatePeakUsage_UpdatePeakPaddingUsageWhenPassingInALargerValue)
{
    constexpr std::size_t newPeak = 500;

    telemetry.incTLSFUsage(10, 2);
    telemetry.incTLSFUsage(50, 2);

    telemetry.updatePeakPaddingUsage(newPeak);
    EXPECT_EQ(newPeak, telemetry.getPeakPadding());
}


/**
 * @test Verify that TLSF telemetry updatePeakPaddingUsage does not update the peak padding
 *        when passing in a smaller value.
 */
TEST_F(TLSFTelemetryTests, UpdatePeakUsage_DoesNotUpdatePeakPaddingUsageWhenPassingInASmallerValue)
{
    constexpr std::size_t newPeak = 12;

    telemetry.incTLSFUsage(10, 24);
    telemetry.incTLSFUsage(50, 24);

    const auto oldMin = telemetry.getPeakPadding();

    telemetry.updatePeakPaddingUsage(newPeak);
    EXPECT_EQ(oldMin, telemetry.getPeakPadding());
}


/**
 * @test Verify that resetting the current usage resets current usage
 *       but preserves peak and minimum usage for both memory and padding.
 */
TEST_F(TLSFTelemetryTests, ResetCurrentUsage_OnlyResetsCurrentUsage)
{

    telemetry.incTLSFUsage(10, 4);
    telemetry.incTLSFUsage(50, 12);
    telemetry.incTLSFUsage(20, 24);
    telemetry.incTLSFUsage(30, 12);
    telemetry.incTLSFUsage(10, 12);

    // Reset current usage
    telemetry.resetCurrentUsage();

    // Only reset current usage
    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());

    // But preserves the min and peak usage
    EXPECT_EQ(10, telemetry.getMinMemoryUsage());
    EXPECT_EQ(50, telemetry.getPeakMemoryUsage());

    EXPECT_EQ(4, telemetry.getMinPadding());
    EXPECT_EQ(24, telemetry.getPeakPadding());
}


/** @test Verify that TLSF telemetry reset resets all values to default. */
TEST_F(TLSFTelemetryTests, Reset_ResetsUsages)
{

    telemetry.incTLSFUsage(10, 4);
    telemetry.incTLSFUsage(50, 4);
    telemetry.incTLSFUsage(20, 4);
    telemetry.incTLSFUsage(30, 4);
    telemetry.incTLSFUsage(10, 4);

    telemetry.resetTelemetry();
    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinMemoryUsage());
    EXPECT_EQ(0, telemetry.getPeakMemoryUsage());


    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinPadding());
    EXPECT_EQ(0, telemetry.getPeakPadding());
}

/** @} */
