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

TEST_F(ManagedPoolAllocator, AllocChunk_IncrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());

    EXPECT_EQ(2, telemetry.getUsedAllocationCount());
}


TEST_F(ManagedPoolAllocator, Alloc_IncrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.alloc<size_t>(1));
    static_cast<void>(pool.alloc<size_t>(1));

    EXPECT_EQ(2, telemetry.getUsedAllocationCount());
}


TEST_F(ManagedPoolAllocator, FreeChunk_DecrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    const auto mem = pool.allocChunk();
    pool.freeChunk(mem);

    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(ManagedPoolAllocator, Free_DecrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    const auto mem = pool.alloc<size_t>(1);
    pool.free(mem);

    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(ManagedPoolAllocator, Clear_ClearsUsageInTelemetry)
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

TEST_F(UnmanagedPoolAllocator, AllocChunk_IncrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());

    EXPECT_EQ(2, telemetry.getUsedAllocationCount());
}


TEST_F(UnmanagedPoolAllocator, Alloc_IncrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.alloc<size_t>(1));
    static_cast<void>(pool.alloc<size_t>(1));

    EXPECT_EQ(2, telemetry.getUsedAllocationCount());
}


TEST_F(UnmanagedPoolAllocator, FreeChunk_DecrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    const auto mem = pool.allocChunk();
    pool.freeChunk(mem);

    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(UnmanagedPoolAllocator, Free_DecrementsUsageInTelemetry)
{
    const auto& telemetry = pool.getTelemetry();
    static_cast<void>(pool.allocChunk());
    const auto mem = pool.alloc<size_t>(1);
    pool.free(mem);

    EXPECT_EQ(1, telemetry.getUsedAllocationCount());
}


TEST_F(UnmanagedPoolAllocator, Clear_ClearsUsageInTelemetry)
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

    TEST_F(ManagedPoolAllocator, Ctor_InitializesTelemetryWithCorrectValues)
    {
        const auto& telemetry = pool.getTelemetry();
        EXPECT_EQ(poolSize, telemetry.getPoolSize());
        EXPECT_EQ(chunkSize, telemetry.getChunkSize());
        EXPECT_EQ(alignment, telemetry.getAlignment());

        EXPECT_EQ(pool._chunkSize, telemetry.getAlignedChunkSize());
        EXPECT_EQ(pool._initialAlignmentPadding, telemetry.getPadding());
    }


    TEST_F(UnmanagedPoolAllocator, Ctor_InitializesTelemetryWithCorrectValues)
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
