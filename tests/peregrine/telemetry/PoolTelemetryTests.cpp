/**
 * @file PoolTelemetryTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 03, 2026
 *
 * @brief Verify @ref pmm::PoolTelemtry creation and update logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/telemetry/PoolTelemetry.h>


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

    /// @brief Test fixture for pool telemetry.
    class PoolTelemetryTests: public ::testing::Test
    {
    public:
        std::size_t poolSize{ 10240 }, chunkSize{ 8 }, alignment{ 8 };
        pmm::PoolTelemetry telemetry{ poolSize, chunkSize, alignment };
    };



    /**************************************
     *                                    *
     *            STATIC TESTS            *
     *                                    *
     **************************************/

    namespace static_tests
    {
        /// @test Verify that PoolTelemetryType returns PoolTelemetry when the telemetry policy is Managed.
        static_assert(std::same_as<pmm::PoolTelemetryType<pmm::telemetry::Enabled>, pmm::PoolTelemetry> == true);

        /// @test Verify that PoolTelemetryType returns DummyPoolTelemetry when the telemetry policy is Disabled.
        static_assert(std::same_as<pmm::PoolTelemetryType<pmm::telemetry::Disabled>, pmm::DummyPoolTelemetry> == true);


        /// @test Verify that getTelemetryInstance returns a real pool telemetry
        ///       when the telemetry policy is Enabled.
        [[maybe_unused]] constexpr auto STACK_TEL_ENABLED =
            pmm::getTelemetryInstance<pmm::telemetry::Enabled>(512, 8, 8);
        static_assert(std::is_same_v<decltype(STACK_TEL_ENABLED), const pmm::PoolTelemetry> == true);

        /// @test Verify that getTelemetryInstance returns a real stack when the telemetry policy is Disabled.
        [[maybe_unused]] constexpr auto STACK_TEL_DISABLED =
            pmm::getTelemetryInstance<pmm::telemetry::Disabled>(512, 8, 8);
        static_assert(std::is_same_v<decltype(STACK_TEL_DISABLED), const pmm::DummyPoolTelemetry> == true);

    } // namespace static_tests
} // namespace




/**************************************
 *           RUNTIME TESTS            *
 **************************************/

TEST_F(PoolTelemetryTests, IntializesWithSizeAndDefaultStats)
{
    EXPECT_EQ(poolSize, telemetry.getPoolSize());
    EXPECT_EQ(chunkSize, telemetry.getChunkSize());
    EXPECT_EQ(chunkSize, telemetry.getAlignedChunkSize());
    EXPECT_EQ(poolSize / chunkSize, telemetry.getMaxAllocationCount());
    EXPECT_EQ(0, telemetry.getUsedAllocationCount());
    EXPECT_EQ(poolSize / chunkSize, telemetry.getFreeAllocationCount());
    EXPECT_EQ(alignment, telemetry.getAlignment());
}


TEST_F(PoolTelemetryTests, SetAlignedChunkSize_UpdatesAlignedChunkSize)
{
    constexpr auto alignedChunkSize = 32;
    telemetry.setAlignedChunkSize(alignedChunkSize);
    EXPECT_EQ(alignedChunkSize, telemetry.getAlignedChunkSize());
}


TEST_F(PoolTelemetryTests, SetAlignedChunkSize_UpdatesMaxAllocationCount)
{
    constexpr auto alignedChunkSize = 32;
    telemetry.setAlignedChunkSize(alignedChunkSize);
    EXPECT_EQ((poolSize) / alignedChunkSize, telemetry.getMaxAllocationCount());
}


TEST_F(PoolTelemetryTests, SetPadding_UpdatesPadding)
{
    constexpr auto newPadding = 64;
    telemetry.setPadding(newPadding);
    EXPECT_EQ(newPadding, telemetry.getPadding());
}


TEST_F(PoolTelemetryTests, SetPadding_UpdatesMaxAllocationCount)
{
    constexpr auto newPadding = 64;
    telemetry.setPadding(newPadding);
    EXPECT_EQ((poolSize - newPadding) / chunkSize, telemetry.getMaxAllocationCount());
}


TEST_F(PoolTelemetryTests, GetUsedSize_ReturnsZeroInitially) { EXPECT_EQ(0, telemetry.getUsedSize()); }


TEST_F(PoolTelemetryTests, GetUsedSize_ReturnsUsedMemoryInclusiveOfPadding)
{
    constexpr auto padding = 12;
    telemetry.setPadding(padding);
    telemetry.logAlloc();
    EXPECT_EQ(chunkSize + padding, telemetry.getUsedSize());
}


TEST_F(PoolTelemetryTests, GetWastedSize_ReturnsZeroInitially) { EXPECT_EQ(0, telemetry.getUsedSize()); }


TEST_F(PoolTelemetryTests, GetWastedSize_ReturnPaddingUsed)
{
    constexpr auto chunkPadding = 4;
    constexpr auto padding      = 12;
    // Set the aligned chunk size + padding
    telemetry.setAlignedChunkSize(chunkSize + chunkPadding);
    telemetry.setPadding(padding);
    // Log one allocation
    telemetry.logAlloc();
    // Wasted size must be the sum of padding
    EXPECT_EQ(chunkPadding + padding, telemetry.getWastedSize());
}


TEST_F(PoolTelemetryTests, GetFreeSize_ReturnsPoolSizeInitially) { EXPECT_EQ(poolSize, telemetry.getFreeSize()); }


TEST_F(PoolTelemetryTests, GetFreeSize_ReturnsFreeMemory)
{
    constexpr auto padding = 12;
    telemetry.setPadding(padding);
    telemetry.logAlloc();
    EXPECT_EQ(poolSize - (chunkSize + padding), telemetry.getFreeSize());
}


TEST_F(PoolTelemetryTests, Logging_UpdateFreeAllocationCount)
{
    const auto initialFreeCount = telemetry.getFreeAllocationCount();
    telemetry.logAlloc();
    EXPECT_EQ(1, initialFreeCount - telemetry.getFreeAllocationCount());
}


TEST_F(PoolTelemetryTests, Logging_UpdateUsedAllocationCount)
{
    telemetry.logAlloc();
    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(PoolTelemetryTests, LogAlloc_IncrementsUsedAllocation)
{
    telemetry.logAlloc();
    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(PoolTelemetryTests, LogFree_DecrementsUsedAllocation)
{
    // 4 allocations
    telemetry.logAlloc();
    telemetry.logAlloc();
    telemetry.logAlloc();
    telemetry.logAlloc();
    // 1 Free
    telemetry.logFree();
    // So 3 used allocation expected
    EXPECT_EQ(3, telemetry.getUsedAllocationCount());
}


TEST_F(PoolTelemetryTests, LogFree_DoesNotGoBelowZero)
{
    telemetry.logFree();
    EXPECT_EQ(0, telemetry.getUsedAllocationCount());
}


/** @} */
