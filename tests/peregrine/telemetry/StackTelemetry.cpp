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

namespace
{
    /**************************************
     *                                    *
     *               SETUP                *
     *                                    *
     **************************************/

    class StackTelemetry: public ::testing::Test
    {
    public:
        std::size_t size{ 10240 };
        pmm::StackTelemetry telemetry{ size };
    };



    /**************************************
     *                                    *
     *            STATIC TESTS            *
     *                                    *
     **************************************/

    namespace static_tests
    {
        namespace telemetry_type
        {
            /// @test Verify that StackTelemetryType returns StackTelemetry when the telemetry policy is Managed.
            static_assert(std::same_as<pmm::StackTelemetryType<pmm::telemetry::Enabled>, pmm::StackTelemetry> == true);


            /// @test Verify that StackTelemetryType returns DummyStackTelemetry when the telemetry policy is Disabled.
            static_assert(std::same_as<pmm::StackTelemetryType<pmm::telemetry::Disabled>, pmm::DummyStackTelemetry> ==
                          true);

        } // namespace telemetry_type


        namespace telemetry_helpers
        {
            /// @test Verify that getTelemetryInstance returns a real stack when the telemetry policy is Managed.
            [[maybe_unused]] constexpr auto STACK_TEL_MANAGED = pmm::getTelemetryInstance<pmm::telemetry::Enabled>(512);
            static_assert(std::is_same_v<decltype(STACK_TEL_MANAGED), const pmm::StackTelemetry> == true);


            /// @test Verify that getTelemetryInstance returns a real stack when the telemetry policy is Disabled.
            [[maybe_unused]] constexpr auto STACK_TEL_DISABLED =
                pmm::getTelemetryInstance<pmm::telemetry::Disabled>(512);
            static_assert(std::is_same_v<decltype(STACK_TEL_DISABLED), const pmm::DummyStackTelemetry> == true);

        } // namespace telemetry_helpers
    } // namespace static_tests
} // namespace




/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/** @test Verify that stack telemetry is initialized with size and usage defaults. */
TEST_F(StackTelemetry, IntializesWithSizeAndDefaultStats)
{
    EXPECT_EQ(size, telemetry.getStackSize());
    EXPECT_EQ(0, telemetry.getTotalUsage());

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinMemoryUsage());

    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getPeakPadding());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinPadding());
}


/** @test Verify that stack telemetry is incremented with correct usage values. */
TEST_F(StackTelemetry, IncTelemetryUsage_UpdatesWithCorrectUsage)
{

    telemetry.incStackUsage(100, 8);
    telemetry.incStackUsage(500, 24);
    telemetry.incStackUsage(200, 100);
    telemetry.incStackUsage(300, 4);
    telemetry.incStackUsage(100, 0);

    EXPECT_EQ(1200, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(100, telemetry.getMinMemoryUsage());
    EXPECT_EQ(500, telemetry.getPeakMemoryUsage());

    EXPECT_EQ(136, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getMinPadding());
    EXPECT_EQ(100, telemetry.getPeakPadding());

    EXPECT_EQ(1336, telemetry.getTotalUsage());
}


TEST(StackTelemetryHelpers, ManagedPolicy_ReturnRealStackTelemetry) {}


/** @test Verify that stack telemetry is decrement with correct usage values. */
TEST_F(StackTelemetry, DecTelemetryUsage_UpdatesWithCorrectUsage)
{

    telemetry.incStackUsage(5000, 120);
    telemetry.decStackUsage(500, 24);
    telemetry.decStackUsage(100, 12);

    EXPECT_EQ(4400, telemetry.getCurrentMemoryUsage());

    EXPECT_EQ(84, telemetry.getCurrentPadding());

    EXPECT_EQ(4484, telemetry.getTotalUsage());
}


/**
 * @test Verify that stack telemetry updateMinMemoryUsage updates the minimum memory usage
 *       when passing in a smaller value.
 */
TEST_F(StackTelemetry, UpdateMinMemoryUsage_UpdateMinimumWhenPassingInASmallValue)
{
    constexpr std::size_t newMin = 5;

    telemetry.incStackUsage(10, 5);
    telemetry.incStackUsage(50, 2);

    telemetry.updateMinMemoryUsage(newMin);
    EXPECT_EQ(newMin, telemetry.getMinMemoryUsage());
}


/**
 * @test Verify that stack telemetry updateMinMemoryUsage does not update the minimum memory usage
 *       when passing in a larger value.
 */
TEST_F(StackTelemetry, UpdateMinMemoryUsage_DoesNotUpdateMinimumWhenPassingInALargerValue)
{
    constexpr std::size_t newMin = 50;

    telemetry.incStackUsage(10, 2);
    telemetry.incStackUsage(40, 2);

    const auto oldMin = telemetry.getMinMemoryUsage();

    telemetry.updateMinMemoryUsage(newMin);
    EXPECT_EQ(oldMin, telemetry.getMinMemoryUsage());
}


/**
 * @test Verify that stack telemetry updatePeakMemoryUsage updates the peak memory usage
 *       when passing in a larger value.
 */
TEST_F(StackTelemetry, UpdatePeakMemoryUsage_UpdatePeakUsageWhenPassingInALargerValue)
{
    constexpr std::size_t newPeak = 500;

    telemetry.incStackUsage(10, 2);
    telemetry.incStackUsage(50, 2);

    telemetry.updatePeakMemoryUsage(newPeak);
    EXPECT_EQ(newPeak, telemetry.getPeakMemoryUsage());
}


/**
 * @test Verify that stack telemetry updatePeakMemoryUsage does not update the peak memory usage
 *       when passing in a smaller value.
 */
TEST_F(StackTelemetry, UpdatePeakMemoryUsage_DoesNotUpdatePeakUsageWhenPassingInASmallerValue)
{
    constexpr std::size_t newPeak = 15;

    telemetry.incStackUsage(10, 2);
    telemetry.incStackUsage(50, 2);

    const auto oldMin = telemetry.getPeakMemoryUsage();

    telemetry.updatePeakMemoryUsage(newPeak);
    EXPECT_EQ(oldMin, telemetry.getPeakMemoryUsage());
}


/**
 * @test Verify that stack telemetry updateMinPaddingUsage updates the minimum padding
 *       when passing in a smaller value.
 */
TEST_F(StackTelemetry, UpdateMinUsage_UpdateMinimumPaddingWhenPassingInASmallValue)
{
    constexpr std::size_t newMin = 5;

    telemetry.incStackUsage(10, 15);
    telemetry.incStackUsage(50, 12);

    telemetry.updateMinPaddingUsage(newMin);
    EXPECT_EQ(newMin, telemetry.getMinPadding());
}


/**
 * @test Verify that stack telemetry updateMinPaddingUsage does not update the minimum padding
 *       when passing in a larger value.
 */
TEST_F(StackTelemetry, UpdateMinUsage_DoesNotUpdateMinimumPaddingWhenPassingInALargerValue)
{
    constexpr std::size_t newMin = 50;

    telemetry.incStackUsage(10, 2);
    telemetry.incStackUsage(40, 2);

    const auto oldMin = telemetry.getMinPadding();

    telemetry.updateMinPaddingUsage(newMin);
    EXPECT_EQ(oldMin, telemetry.getMinPadding());
}


/**
 * @test Verify that stack telemetry updatePeakPaddingUsage updates the peak padding
 *       when passing in a larger value.
 */
TEST_F(StackTelemetry, UpdatePeakUsage_UpdatePeakPaddingUsageWhenPassingInALargerValue)
{
    constexpr std::size_t newPeak = 500;

    telemetry.incStackUsage(10, 2);
    telemetry.incStackUsage(50, 2);

    telemetry.updatePeakPaddingUsage(newPeak);
    EXPECT_EQ(newPeak, telemetry.getPeakPadding());
}


/**
 * @test Verify that stack telemetry updatePeakPaddingUsage does not update the peak padding
 *        when passing in a smaller value.
 */
TEST_F(StackTelemetry, UpdatePeakUsage_DoesNotUpdatePeakPaddingUsageWhenPassingInASmallerValue)
{
    constexpr std::size_t newPeak = 12;

    telemetry.incStackUsage(10, 24);
    telemetry.incStackUsage(50, 24);

    const auto oldMin = telemetry.getPeakPadding();

    telemetry.updatePeakPaddingUsage(newPeak);
    EXPECT_EQ(oldMin, telemetry.getPeakPadding());
}


/**
 * @test Verify that resetting the current usage resets current usage
 *       but preserves peak and minimum usage for both memory and padding.
 */
TEST_F(StackTelemetry, ResetCurrentUsage_OnlyResetsCurrentUsage)
{

    telemetry.incStackUsage(10, 4);
    telemetry.incStackUsage(50, 12);
    telemetry.incStackUsage(20, 24);
    telemetry.incStackUsage(30, 12);
    telemetry.incStackUsage(10, 12);

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


/** @test Verify that stack telemetry reset resets all values to default. */
TEST_F(StackTelemetry, Reset_ResetsUsages)
{

    telemetry.incStackUsage(10, 4);
    telemetry.incStackUsage(50, 4);
    telemetry.incStackUsage(20, 4);
    telemetry.incStackUsage(30, 4);
    telemetry.incStackUsage(10, 4);

    telemetry.resetTelemetry();
    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinMemoryUsage());
    EXPECT_EQ(0, telemetry.getPeakMemoryUsage());


    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(std::numeric_limits<std::size_t>::max(), telemetry.getMinPadding());
    EXPECT_EQ(0, telemetry.getPeakPadding());
}

/** @} */
