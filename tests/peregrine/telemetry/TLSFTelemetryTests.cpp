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
    constexpr auto maxValue = std::numeric_limits<size_t>::max();
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

    EXPECT_EQ(0, telemetry.getLifetimeAllocations());
    EXPECT_EQ(0, telemetry.getActiveAllocations());
    EXPECT_EQ(0, telemetry.getLifetimeFrees());
    EXPECT_EQ(0, telemetry.getLargestFreeBlockSize());
    EXPECT_EQ(0, telemetry.getFreeBlockCount());

    EXPECT_FALSE(telemetry.hasMemoryLeak());
}


/** @test Verify that TLSF telemetry is incremented with correct usage values. */
TEST_F(TLSFTelemetryTests, IncTelemetryUsage_UpdatesWithCorrectUsage)
{
    telemetry.incUsage(100, 8);
    telemetry.incUsage(500, 24);
    telemetry.incUsage(200, 100);
    telemetry.incUsage(300, 4);
    telemetry.incUsage(100, 0);
    constexpr auto totalBufferUsage = 1200;
    constexpr auto metaSize         = 136;

    EXPECT_EQ(metaSize + totalBufferUsage, telemetry.getCurrentBufferUsage());
    EXPECT_EQ(100, telemetry.getMinBufferUsage());
    EXPECT_EQ(524, telemetry.getPeakBufferUsage());

    EXPECT_EQ(metaSize, telemetry.getCurrentMetadataUsage());
    EXPECT_EQ(0, telemetry.getMinMetadataUsage());
    EXPECT_EQ(100, telemetry.getPeakMetadataUsage());

    EXPECT_EQ(totalBufferUsage, telemetry.getCurrentPayloadUsage());
    EXPECT_EQ(100, telemetry.getMinPayloadUsage());
    EXPECT_EQ(500, telemetry.getPeakPayloadUsage());

    EXPECT_EQ(5, telemetry.getLifetimeAllocations());
    EXPECT_EQ(5, telemetry.getActiveAllocations());
    EXPECT_EQ(0, telemetry.getLifetimeFrees());
}


/** @test Verify that TLSF telemetry is decrement with correct usage values. */
TEST_F(TLSFTelemetryTests, DecTelemetryUsage_UpdatesWithCorrectUsage)
{
    telemetry.incUsage(2500, 120);
    telemetry.incUsage(2500, 120);
    telemetry.incUsage(1200, 24);
    telemetry.decUsage(2500, 120);
    telemetry.decUsage(1200, 24);

    EXPECT_EQ(2620, telemetry.getCurrentBufferUsage());
    EXPECT_EQ(1224, telemetry.getMinBufferUsage());
    EXPECT_EQ(2620, telemetry.getPeakBufferUsage());

    EXPECT_EQ(120, telemetry.getCurrentMetadataUsage());
    EXPECT_EQ(24, telemetry.getMinMetadataUsage());
    EXPECT_EQ(120, telemetry.getPeakMetadataUsage());

    EXPECT_EQ(2500, telemetry.getCurrentPayloadUsage());
    EXPECT_EQ(1200, telemetry.getMinPayloadUsage());
    EXPECT_EQ(2500, telemetry.getPeakPayloadUsage());

    EXPECT_EQ(3, telemetry.getLifetimeAllocations());
    EXPECT_EQ(1, telemetry.getActiveAllocations());
    EXPECT_EQ(2, telemetry.getLifetimeFrees());

    EXPECT_FALSE(telemetry.hasMemoryLeak());
}


TEST_F(TLSFTelemetryTests, IncFreeBlockCount_IncreasesFreeBlockCountBy1)
{
    telemetry.incFreeBlockCount();
    EXPECT_EQ(1, telemetry.getFreeBlockCount());

    telemetry.incFreeBlockCount();
    EXPECT_EQ(2, telemetry.getFreeBlockCount());

    telemetry.incFreeBlockCount();
    EXPECT_EQ(3, telemetry.getFreeBlockCount());

    telemetry.incFreeBlockCount();
    EXPECT_EQ(4, telemetry.getFreeBlockCount());
}


TEST_F(TLSFTelemetryTests, DecFreeBlockCount_DecreasesFreeBlockCountBy1)
{
    telemetry.incFreeBlockCount();
    telemetry.incFreeBlockCount();
    telemetry.incFreeBlockCount();
    telemetry.incFreeBlockCount();
    telemetry.incFreeBlockCount();

    EXPECT_EQ(5, telemetry.getFreeBlockCount());

    telemetry.decFreeBlockCount();
    EXPECT_EQ(4, telemetry.getFreeBlockCount());

    telemetry.decFreeBlockCount();
    EXPECT_EQ(3, telemetry.getFreeBlockCount());

    telemetry.decFreeBlockCount();
    EXPECT_EQ(2, telemetry.getFreeBlockCount());

    telemetry.decFreeBlockCount();
    EXPECT_EQ(1, telemetry.getFreeBlockCount());
}


TEST_F(TLSFTelemetryTests, HasLeak_ReturnsFalse_ForEqualAllocationsAndDeallocations)
{
    telemetry.incUsage(2500, 120);
    telemetry.incUsage(2500, 120);
    telemetry.incUsage(1200, 24);
    telemetry.decUsage(2500, 120);
    telemetry.decUsage(1200, 24);
    telemetry.decUsage(2500, 120);

    EXPECT_FALSE(telemetry.hasMemoryLeak());
}


// TODO: Add back after figuring out how to cause a leak?
// TEST_F(TLSFTelemetryTests, HasLeak_ReturnsTrue_ForEqualAllocationsAndDeallocations)
// {
//     telemetry.incUsage(2500, 120);
//     telemetry.incUsage(2500, 120);
//     telemetry.incUsage(1200, 24);
//     telemetry.decUsage(2500, 120);
//     telemetry.decUsage(1200, 24);
//     telemetry.decUsage(2500, 120);
//
//     EXPECT_FALSE(telemetry.hasMemoryLeak());
// }


TEST_F(TLSFTelemetryTests, UpdateLargestFreeBlockSize_UpdatesMemberVariable)
{
    telemetry.updateLargestFreeBlockSize(512);
    EXPECT_EQ(512, telemetry.getLargestFreeBlockSize());
    telemetry.updateLargestFreeBlockSize(24);
    EXPECT_EQ(24, telemetry.getLargestFreeBlockSize());
    telemetry.updateLargestFreeBlockSize(746);
    EXPECT_EQ(746, telemetry.getLargestFreeBlockSize());
}

/** @} */
