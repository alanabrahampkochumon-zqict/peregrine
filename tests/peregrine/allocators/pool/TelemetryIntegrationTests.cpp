/**
 * @file TelemetryIntegrationTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Verifies telemetry integration across both managed and unmanaged pool allocators.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "PoolTestSetup.h"

/**
 * @addtogroup T_PMM_Pool
 * @{
 */


/**************************************
 *            MANAGED POOL            *
 **************************************/

TEST_F(ManagedPoolAllocatorTests, EnabledTelemetry_ReturnsRealTelemetry)
{
    [[maybe_unused]] pmm::Pool<pmm::ManagedMemory, pmm::telemetry::Enabled> telemetryEnabledPool(512, 16, 8);

    [[maybe_unused]] auto telemetry = telemetryEnabledPool.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::PoolTelemetry>;

    EXPECT_TRUE(result);
}


TEST_F(ManagedPoolAllocatorTests, DisabledTelemetry_ReturnsDummyTelemetry)
{
    [[maybe_unused]] pmm::Pool<pmm::ManagedMemory, pmm::telemetry::Disabled> telemetryDisabledPool(512, 16, 8);

    [[maybe_unused]] auto telemetry = telemetryDisabledPool.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::DummyPoolTelemetry>;

    EXPECT_TRUE(result);
}


TEST_F(ManagedPoolAllocatorTests, IsTelemetryEnabled_ReturnsTrue) { EXPECT_TRUE(pool.isTelemetryEnabled()); }


TEST(ManagedPoolAllocatorTests_TelemetryDisabled, IsTelemetryEnabled_ReturnsFalse)
{
    const pmm::Pool<pmm::ManagedMemory, pmm::telemetry::Disabled> pool(512, 24, 4);
    EXPECT_FALSE(pool.isTelemetryEnabled());
}


TEST_F(ManagedPoolAllocatorTests, AllocChunk_IncrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());

    EXPECT_EQ(2, telemetry.getUsedAllocationCount());
}


TEST_F(ManagedPoolAllocatorTests, Alloc_IncrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.alloc<size_t>(1));
    static_cast<void>(pool.alloc<size_t>(1));

    EXPECT_EQ(2, telemetry.getUsedAllocationCount());
}


TEST_F(ManagedPoolAllocatorTests, FreeChunk_DecrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    const auto mem = pool.allocChunk();
    pool.freeChunk(mem);

    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(ManagedPoolAllocatorTests, Free_DecrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    const auto mem = pool.alloc<size_t>(1);
    pool.free(mem);

    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(ManagedPoolAllocatorTests, Clear_ClearsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());
    pool.clear();

    EXPECT_EQ(0, telemetry.getUsedAllocationCount());
}



/**************************************
 *           UNMANAGED POOL           *
 **************************************/

TEST_F(UnmanagedPoolAllocatorTests, EnabledTelemetry_ReturnsRealTelemetry)
{
    const auto backingBuffer = new uint8_t[512];

    [[maybe_unused]] const pmm::Pool<pmm::UnmanagedMemory, pmm::telemetry::Enabled> telemetryEnabledPool(backingBuffer,
                                                                                                         512, 16, 8);

    [[maybe_unused]] auto telemetry = telemetryEnabledPool.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::PoolTelemetry>;
    EXPECT_TRUE(result);

    delete[] backingBuffer;
}


TEST_F(UnmanagedPoolAllocatorTests, DisabledTelemetry_ReturnsDummyTelemetry)
{
    const auto backingBuffer = new uint8_t[512];

    [[maybe_unused]] const pmm::Pool<pmm::UnmanagedMemory, pmm::telemetry::Disabled> telemetryDisabledPool(
        backingBuffer, 512, 16, 8);
    [[maybe_unused]] auto telemetry = telemetryDisabledPool.getTelemetry();

    const bool result               = std::is_same_v<decltype(telemetry), pmm::DummyPoolTelemetry>;
    EXPECT_TRUE(result);

    delete[] backingBuffer;
}


TEST_F(UnmanagedPoolAllocatorTests, IsTelemetryEnabled_ReturnsTrue) { EXPECT_TRUE(pool.isTelemetryEnabled()); }


TEST(UnmanagedPoolAllocator_TelemetryDisabledTests, IsTelemetryEnabled_ReturnsFalse)
{
    constexpr size_t size{ 512 };
    const auto buffer = new uint8_t[size];

    const pmm::Pool<pmm::UnmanagedMemory, pmm::telemetry::Disabled> pool(buffer, 512, 24, 4);
    EXPECT_FALSE(pool.isTelemetryEnabled());

    delete[] buffer;
}

TEST_F(UnmanagedPoolAllocatorTests, AllocChunk_IncrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());

    EXPECT_EQ(2, telemetry.getUsedAllocationCount());
}


TEST_F(UnmanagedPoolAllocatorTests, Alloc_IncrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.alloc<size_t>(1));
    static_cast<void>(pool.alloc<size_t>(1));

    EXPECT_EQ(2, telemetry.getUsedAllocationCount());
}


TEST_F(UnmanagedPoolAllocatorTests, FreeChunk_DecrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    const auto mem = pool.allocChunk();
    pool.freeChunk(mem);

    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(UnmanagedPoolAllocatorTests, Free_DecrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    const auto mem = pool.alloc<size_t>(1);
    pool.free(mem);

    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(UnmanagedPoolAllocatorTests, Clear_ClearsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());
    pool.clear();

    EXPECT_EQ(0, telemetry.getUsedAllocationCount());
}



/**************************************
 *                                    *
 *            FRIEND TESTS            *
 *                                    *
 **************************************/

namespace pmm
{

    TEST_F(ManagedPoolAllocatorTests, Ctor_InitializesTelemetryWithCorrectValues)
    {
        const auto& telemetry = pool.getTelemetry();
        EXPECT_EQ(poolSize, telemetry.getPoolSize());
        EXPECT_EQ(chunkSize, telemetry.getChunkSize());
        EXPECT_EQ(alignment, telemetry.getAlignment());

        EXPECT_EQ(pool._chunkSize, telemetry.getAlignedChunkSize());
        EXPECT_EQ(pool._initialAlignmentPadding, telemetry.getPadding());
    }


    TEST_F(UnmanagedPoolAllocatorTests, Ctor_InitializesTelemetryWithCorrectValues)
    {
        const auto& telemetry = pool.getTelemetry();
        EXPECT_EQ(bufferSize, telemetry.getPoolSize());
        EXPECT_EQ(chunkSize, telemetry.getChunkSize());
        EXPECT_EQ(alignment, telemetry.getAlignment());

        EXPECT_EQ(pool._chunkSize, telemetry.getAlignedChunkSize());
        EXPECT_EQ(pool._initialAlignmentPadding, telemetry.getPadding());
    }
} // namespace pmm

/** @} */
