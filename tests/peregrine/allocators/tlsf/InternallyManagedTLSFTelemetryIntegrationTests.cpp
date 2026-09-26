/**
 * @file InternallyManagedTLSFTelemetryIntegrationTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 24, 2026
 *
 * @brief Verify managed tlsf allocation, free, and helper function logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"
#include "Utils.h"

#include <array>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <peregrine/allocators/TLSF.h>
#include <peregrine/utils/Constants.h>
#include <utility>


/**
 * @addtogroup T_PMM_TLSF_Core
 * @{
 */

namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/
    using namespace pmm::constants;

    /**
     * @brief Test fixture for managed @ref pmm::TLSF.
     */
    class InternallyManagedTLSFTelemetryIntegrationTests: public testing::Test
    {
    public:
        static constexpr size_t tlsfSize{ 2_MB };
        pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled> tlsf{ tlsfSize };
        using Offset_t = pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled>::HeaderOffset_t;
        using Header   = pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled>::Header;
    };
} // namespace



/**************************************
 *           RUNTIME TESTS            *
 **************************************/

/**************************************
 *           INITIALIZATIONS          *
 **************************************/

TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, EnabledTelemetry_ReturnsRealTelemetry)
{
    [[maybe_unused]] const pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled> telemetryEnabledTLSF(512);
    [[maybe_unused]] auto telemetry = telemetryEnabledTLSF.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::TLSFTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, DisabledTelemetry_ReturnsDummyTelemetry)
{
    [[maybe_unused]] const pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Disabled> telemetryDisabledTLSF(512);
    [[maybe_unused]] auto telemetry = telemetryDisabledTLSF.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::DummyTLSFTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, TelemetryIsInitializedToTLSFCapacity)
{ EXPECT_EQ(tlsfSize, tlsf.getTelemetry().getSize()); }


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, HasZeroUsedCapacityInitially)
{
    EXPECT_EQ(0, tlsf.getTelemetry().getCurrentBufferUsage());
    EXPECT_EQ(0, tlsf.getTelemetry().getCurrentMetadataUsage());
    EXPECT_EQ(0, tlsf.getTelemetry().getCurrentPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, HasASingleFreeBlockInitially)
{ EXPECT_EQ(1, tlsf.getTelemetry().getFreeBlockCount()); }


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, LargestBlockSize_EqualsTLSFSizeInitially)
{ EXPECT_EQ(tlsfSize, tlsf.getTelemetry().getLargestFreeBlockSize()); }


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MoveCtor_MovesTelemetry)
{
    static_cast<void>(tlsf.malloc(120));
    static_cast<void>(tlsf.malloc(240));
    // Get the telemetry to ensure that the value is preserved when moving
    // DON'T get by reference as it will change internally
    const auto telemetry = tlsf.getTelemetry();

    const pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled> tlsf2 = std::move(tlsf);

    // Checking for telemetry equality
    EXPECT_EQ(telemetry.getActiveAllocations(), tlsf2.getTelemetry().getActiveAllocations());
    EXPECT_EQ(telemetry.getCurrentBufferUsage(), tlsf2.getTelemetry().getCurrentBufferUsage());
    EXPECT_EQ(telemetry.getCurrentMetadataUsage(), tlsf2.getTelemetry().getCurrentMetadataUsage());
    EXPECT_EQ(telemetry.getCurrentPayloadUsage(), tlsf2.getTelemetry().getCurrentPayloadUsage());
    EXPECT_EQ(telemetry.getFreeBlockCount(), tlsf2.getTelemetry().getFreeBlockCount());
    EXPECT_EQ(telemetry.getLargestFreeBlockSize(), tlsf2.getTelemetry().getLargestFreeBlockSize());
    EXPECT_EQ(telemetry.getLifetimeAllocations(), tlsf2.getTelemetry().getLifetimeAllocations());
    EXPECT_EQ(telemetry.getLifetimeFrees(), tlsf2.getTelemetry().getLifetimeFrees());
    EXPECT_EQ(telemetry.getMinBufferUsage(), tlsf2.getTelemetry().getMinBufferUsage());
    EXPECT_EQ(telemetry.getMinMetadataUsage(), tlsf2.getTelemetry().getMinMetadataUsage());
    EXPECT_EQ(telemetry.getMinPayloadUsage(), tlsf2.getTelemetry().getMinPayloadUsage());
    EXPECT_EQ(telemetry.getPeakMetadataUsage(), tlsf2.getTelemetry().getPeakMetadataUsage());
    EXPECT_EQ(telemetry.getPeakBufferUsage(), tlsf2.getTelemetry().getPeakBufferUsage());
    EXPECT_EQ(telemetry.getPeakPayloadUsage(), tlsf2.getTelemetry().getPeakPayloadUsage());
    EXPECT_EQ(telemetry.getSize(), tlsf2.getTelemetry().getSize());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MoveAssign_MovesTelemetry)
{
    static_cast<void>(tlsf.malloc(120));
    static_cast<void>(tlsf.malloc(240));
    // Get the telemetry to ensure that the value is preserved when moving
    // DON'T get by reference as it will change internally
    const auto telemetry = tlsf.getTelemetry();

    pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled> tlsf2(256);
    tlsf2 = std::move(tlsf);

    // Checking for telemetry equality
    EXPECT_EQ(telemetry.getActiveAllocations(), tlsf2.getTelemetry().getActiveAllocations());
    EXPECT_EQ(telemetry.getCurrentBufferUsage(), tlsf2.getTelemetry().getCurrentBufferUsage());
    EXPECT_EQ(telemetry.getCurrentMetadataUsage(), tlsf2.getTelemetry().getCurrentMetadataUsage());
    EXPECT_EQ(telemetry.getCurrentPayloadUsage(), tlsf2.getTelemetry().getCurrentPayloadUsage());
    EXPECT_EQ(telemetry.getFreeBlockCount(), tlsf2.getTelemetry().getFreeBlockCount());
    EXPECT_EQ(telemetry.getLargestFreeBlockSize(), tlsf2.getTelemetry().getLargestFreeBlockSize());
    EXPECT_EQ(telemetry.getLifetimeAllocations(), tlsf2.getTelemetry().getLifetimeAllocations());
    EXPECT_EQ(telemetry.getLifetimeFrees(), tlsf2.getTelemetry().getLifetimeFrees());
    EXPECT_EQ(telemetry.getMinBufferUsage(), tlsf2.getTelemetry().getMinBufferUsage());
    EXPECT_EQ(telemetry.getMinMetadataUsage(), tlsf2.getTelemetry().getMinMetadataUsage());
    EXPECT_EQ(telemetry.getMinPayloadUsage(), tlsf2.getTelemetry().getMinPayloadUsage());
    EXPECT_EQ(telemetry.getPeakMetadataUsage(), tlsf2.getTelemetry().getPeakMetadataUsage());
    EXPECT_EQ(telemetry.getPeakBufferUsage(), tlsf2.getTelemetry().getPeakBufferUsage());
    EXPECT_EQ(telemetry.getPeakPayloadUsage(), tlsf2.getTelemetry().getPeakPayloadUsage());
    EXPECT_EQ(telemetry.getSize(), tlsf2.getTelemetry().getSize());
}


/**************************************
 *            ALLOC BYTES             *
 **************************************/

// Single allocation tests
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, SingleMalloc_UpdatesAllocationCount)
{
    static_cast<void>(tlsf.malloc(512));
    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(1, tel.getActiveAllocations());
    EXPECT_EQ(1, tel.getLifetimeAllocations());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, SingleMalloc_UpdatesCurrentBufferUsage)
{
    const auto mem    = static_cast<uint8_t*>(tlsf.malloc(512));
    const auto offset = *reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));
    const auto& tel   = tlsf.getTelemetry();
    EXPECT_EQ(512 + offset, tel.getCurrentBufferUsage());
    EXPECT_EQ(offset, tel.getCurrentMetadataUsage());
    EXPECT_EQ(512, tel.getCurrentPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, SingleMalloc_UpdatesMinBufferUsage)
{
    const auto mem    = static_cast<uint8_t*>(tlsf.malloc(512));
    const auto offset = *reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));
    const auto& tel   = tlsf.getTelemetry();
    EXPECT_EQ(512 + offset, tel.getMinBufferUsage());
    EXPECT_EQ(offset, tel.getMinMetadataUsage());
    EXPECT_EQ(512, tel.getMinPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, SingleMalloc_UpdatesPeakBufferUsage)
{
    const auto mem    = static_cast<uint8_t*>(tlsf.malloc(512));
    const auto offset = *reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));
    const auto& tel   = tlsf.getTelemetry();
    EXPECT_EQ(512 + offset, tel.getPeakBufferUsage());
    EXPECT_EQ(offset, tel.getPeakMetadataUsage());
    EXPECT_EQ(512, tel.getPeakPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, SingleMalloc_UpdatesFreeBlockInformation)
{
    const auto mem    = static_cast<uint8_t*>(tlsf.malloc(512));
    const auto offset = *reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));
    const auto header = reinterpret_cast<Header*>(mem - offset);
    const auto& tel   = tlsf.getTelemetry();
    EXPECT_EQ(tlsfSize - header->getSize(), tel.getLargestFreeBlockSize());
    EXPECT_EQ(1, tel.getFreeBlockCount());
}

// Multiple allocation tests
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleMalloc_UpdatesAllocationCount)
{
    static_cast<void>(tlsf.malloc(512));
    static_cast<void>(tlsf.malloc(256_KB));
    static_cast<void>(tlsf.malloc(1_KB));
    static_cast<void>(tlsf.malloc(1_MB));
    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(4, tel.getActiveAllocations());
    EXPECT_EQ(4, tel.getLifetimeAllocations());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleMalloc_UpdatesCurrentBufferUsage)
{
    const auto mem1    = static_cast<uint8_t*>(tlsf.malloc(512));
    const auto offset1 = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2    = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2 = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3    = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3 = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4    = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4 = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    const auto totalMetadataSize    = offset1 + offset2 + offset3 + offset4;
    constexpr auto totalPayloadSize = 512 + 1_KB + 1_MB + 256_KB;

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(totalPayloadSize + totalMetadataSize, tel.getCurrentBufferUsage());
    EXPECT_EQ(totalMetadataSize, tel.getCurrentMetadataUsage());
    EXPECT_EQ(totalPayloadSize, tel.getCurrentPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleMalloc_UpdatesMinBufferUsage)
{
    constexpr auto minPayloadSize = 512ull;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(minPayloadSize, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedMinMetadataUsage = std::min({ offset1, offset2, offset3, offset4 });
    const auto expectedMinBufferUsage =
        std::min({ minPayloadSize + offset1, 1_KB + offset2, 1_MB + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedMinBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(expectedMinMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadSize, tel.getMinPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleMalloc_UpdatesPeakBufferUsage)
{
    constexpr auto maxPayloadSize = 1_MB;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(maxPayloadSize));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedPeakMetadataUsage = std::max({ offset1, offset2, offset3, offset4 });
    const auto expectedPeakBufferUsage =
        std::max({ 512ull + offset1, 1_KB + offset2, maxPayloadSize + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedPeakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(expectedPeakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(maxPayloadSize, tel.getPeakPayloadUsage());
}

TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleMalloc_UpdatesFreeBlockInformation)
{
    const auto mem1    = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1 = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2    = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2 = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3    = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3 = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4    = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4 = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    const auto totalUsedBuffer = 512 + offset1 + 1_KB + offset2 + 1_MB + offset3 + 256_KB + offset4;

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(tlsfSize - totalUsedBuffer, tel.getLargestFreeBlockSize());
    EXPECT_EQ(1, tel.getFreeBlockCount());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleAlloc_UpdatesTelemetryParameters)
{
    constexpr size_t allocCount = 1000;
    std::vector<Vec4*> allocations;
    for (size_t i{ 0 }; i < allocCount; ++i)
    {
        const auto floatI = static_cast<float>(i);
        allocations.push_back(tlsf.alloc<Vec4>(floatI, floatI + 1.0f, floatI + 2.0f, floatI + 3.0f));
    }

    constexpr size_t max    = std::numeric_limits<size_t>::max();
    size_t minMetadataUsage = max, minBufferUsage = max, minPayloadUsage = sizeof(Vec4);
    size_t peakMetadataUsage = 0, peakBufferUsage = 0, peakPayloadUsage = sizeof(Vec4);
    size_t curMetadataUsage = 0, curBufferUsage = 0, curPayloadUsage = allocCount * sizeof(Vec4);

    for (const auto allocation : allocations)
    {
        const auto offset = *reinterpret_cast<Offset_t*>(reinterpret_cast<uint8_t*>(allocation) - sizeof(Offset_t));
        curMetadataUsage += offset;

        minMetadataUsage  = std::min(minMetadataUsage, static_cast<size_t>(offset));
        peakMetadataUsage = std::max(peakMetadataUsage, static_cast<size_t>(offset));

        minBufferUsage  = std::min(minBufferUsage, minPayloadUsage + offset);
        peakBufferUsage = std::max(peakBufferUsage, peakPayloadUsage + offset);
    }
    curBufferUsage = curMetadataUsage + curPayloadUsage;

    const auto& tel                = tlsf.getTelemetry();
    const auto largesFreeBlockSize = tlsfSize - curBufferUsage;

    EXPECT_EQ(allocCount, tel.getActiveAllocations());
    EXPECT_EQ(allocCount, tel.getLifetimeAllocations());

    EXPECT_EQ(1, tel.getFreeBlockCount());
    EXPECT_EQ(largesFreeBlockSize, tel.getLargestFreeBlockSize());
    EXPECT_EQ(0, tel.getLifetimeFrees());

    EXPECT_EQ(curBufferUsage, tel.getCurrentBufferUsage());
    EXPECT_EQ(curMetadataUsage, tel.getCurrentMetadataUsage());
    EXPECT_EQ(curPayloadUsage, tel.getCurrentPayloadUsage());
    EXPECT_EQ(minBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(minMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadUsage, tel.getMinPayloadUsage());
    EXPECT_EQ(peakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(peakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(peakPayloadUsage, tel.getPeakPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleAllocV_UpdatesTelemetryParameters)
{
    constexpr size_t allocCount = 100;
    constexpr size_t arrSize    = 10;
    std::vector<std::span<Vec4>> allocations;
    for (size_t i{ 0 }; i < allocCount; ++i)
    {
        allocations.push_back(tlsf.allocV<Vec4>(arrSize));
    }

    constexpr size_t max    = std::numeric_limits<size_t>::max();
    size_t minMetadataUsage = max, minBufferUsage = max, minPayloadUsage = arrSize * sizeof(Vec4);
    size_t peakMetadataUsage = 0, peakBufferUsage = 0, peakPayloadUsage = arrSize * sizeof(Vec4);
    size_t curMetadataUsage = 0, curBufferUsage = 0, curPayloadUsage = arrSize * allocCount * sizeof(Vec4);

    for (const auto allocation : allocations)
    {
        const auto offset =
            *reinterpret_cast<Offset_t*>(reinterpret_cast<uint8_t*>(allocation.data()) - sizeof(Offset_t));
        curMetadataUsage += offset;

        minMetadataUsage  = std::min(minMetadataUsage, static_cast<size_t>(offset));
        peakMetadataUsage = std::max(peakMetadataUsage, static_cast<size_t>(offset));

        minBufferUsage  = std::min(minBufferUsage, minPayloadUsage + offset);
        peakBufferUsage = std::max(peakBufferUsage, peakPayloadUsage + offset);
    }
    curBufferUsage = curMetadataUsage + curPayloadUsage;

    const auto& tel                = tlsf.getTelemetry();
    const auto largesFreeBlockSize = tlsfSize - curBufferUsage;

    EXPECT_EQ(allocCount, tel.getActiveAllocations());
    EXPECT_EQ(allocCount, tel.getLifetimeAllocations());

    EXPECT_EQ(1, tel.getFreeBlockCount());
    EXPECT_EQ(largesFreeBlockSize, tel.getLargestFreeBlockSize());
    EXPECT_EQ(0, tel.getLifetimeFrees());

    EXPECT_EQ(curBufferUsage, tel.getCurrentBufferUsage());
    EXPECT_EQ(curMetadataUsage, tel.getCurrentMetadataUsage());
    EXPECT_EQ(curPayloadUsage, tel.getCurrentPayloadUsage());
    EXPECT_EQ(minBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(minMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadUsage, tel.getMinPayloadUsage());
    EXPECT_EQ(peakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(peakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(peakPayloadUsage, tel.getPeakPayloadUsage());
}


// Free in ascending order(First allocation are freed first(But not all the allocations)
/// @test Verify that freeing memory in ascending order(first allocation freed first) updates telemetry allocation
/// count.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, AscendingMFree_UpdatesAllocationCount)
{
    const auto mem1                  = tlsf.malloc(512);
    const auto mem2                  = tlsf.malloc(256_KB);
    const auto mem3                  = tlsf.malloc(1_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(1_MB);
    tlsf.mfree(mem1);
    tlsf.mfree(mem2);
    tlsf.mfree(mem3);
    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(1, tel.getActiveAllocations());
    EXPECT_EQ(3, tel.getLifetimeFrees());
    EXPECT_EQ(4, tel.getLifetimeAllocations());
}


/// @test Verify that freeing memory in ascending order(first allocation freed first) updates telemetry buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, AscendingMFree_UpdatesCurrentBufferUsage)
{
    const auto mem1    = static_cast<uint8_t*>(tlsf.malloc(512));
    const auto mem2    = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto mem3    = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto mem4    = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4 = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem2);
    tlsf.mfree(mem3);

    const auto totalMetadataSize    = offset4;
    constexpr auto totalPayloadSize = 256_KB;

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(totalPayloadSize + totalMetadataSize, tel.getCurrentBufferUsage());
    EXPECT_EQ(totalMetadataSize, tel.getCurrentMetadataUsage());
    EXPECT_EQ(totalPayloadSize, tel.getCurrentPayloadUsage());
}


/// @test Verify that freeing memory in ascending order(first allocation freed first) does not
///       update telemetry minimum buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, AscendingMFree_DoesNotUpdateMinBufferUsage)
{
    constexpr auto minPayloadSize = 512ull;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(minPayloadSize, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem2);
    tlsf.mfree(mem3);

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedMinMetadataUsage = std::min({ offset1, offset2, offset3, offset4 });
    const auto expectedMinBufferUsage =
        std::min({ minPayloadSize + offset1, 1_KB + offset2, 1_MB + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedMinBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(expectedMinMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadSize, tel.getMinPayloadUsage());
}


/// @test Verify that freeing memory in ascending order(first allocation freed first) does not
///       update telemetry peak buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, AscendingMFree_DoesNotUpdatePeakBufferUsage)
{
    constexpr auto maxPayloadSize = 1_MB;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(maxPayloadSize));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem2);
    tlsf.mfree(mem3);

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedPeakMetadataUsage = std::max({ offset1, offset2, offset3, offset4 });
    const auto expectedPeakBufferUsage =
        std::max({ 512ull + offset1, 1_KB + offset2, maxPayloadSize + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedPeakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(expectedPeakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(maxPayloadSize, tel.getPeakPayloadUsage());
}


/// @test Verify that freeing memory in ascending order(first allocation freed first) updates
///       free block information.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, AscendingMFree_UpdatesFreeBlockInformation)
{
    const auto mem1    = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1 = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2    = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2 = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3    = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3 = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4    = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4 = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem2);
    tlsf.mfree(mem3);

    const auto totalUsedBuffer = 512 + offset1 + 1_KB + offset2 + 1_MB + offset3 + 256_KB + offset4;

    const auto& tel = tlsf.getTelemetry();
    // NOTE: TLSF Free block won't get update since we are holding onto separate free blocks
    //       1. The largest block since allocations ended
    //       2. One block size of the freed allocations(due to coalescing)
    const auto freedBlockSizes = 512 + offset1 + 1_KB + offset2 + 1_MB + offset3;
    const auto largest         = std::max(tlsfSize - totalUsedBuffer, freedBlockSizes);
    EXPECT_EQ(largest, tel.getLargestFreeBlockSize());
    EXPECT_EQ(2, tel.getFreeBlockCount());
}


/// @test Verify that freeing memory in descending order(last allocation freed first) updates telemetry allocation
/// count.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, DescendingMFree_UpdatesAllocationCount)
{
    const auto mem1                  = tlsf.malloc(512);
    const auto mem2                  = tlsf.malloc(256_KB);
    const auto mem3                  = tlsf.malloc(1_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(1_MB);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem1);
    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(1, tel.getActiveAllocations());
    EXPECT_EQ(3, tel.getLifetimeFrees());
    EXPECT_EQ(4, tel.getLifetimeAllocations());
}


/// @test Verify that freeing memory in descending order(last allocation freed first) updates telemetry buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, DescendingMFree_UpdatesCurrentBufferUsage)
{
    const auto mem1    = static_cast<uint8_t*>(tlsf.malloc(512));
    const auto mem2    = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto mem3    = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto mem4    = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4 = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem1);

    const auto totalMetadataSize    = offset4;
    constexpr auto totalPayloadSize = 256_KB;

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(totalPayloadSize + totalMetadataSize, tel.getCurrentBufferUsage());
    EXPECT_EQ(totalMetadataSize, tel.getCurrentMetadataUsage());
    EXPECT_EQ(totalPayloadSize, tel.getCurrentPayloadUsage());
}


/// @test Verify that freeing memory in descending order(last allocation freed first) does not
///       update telemetry minimum buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, DescendingMFree_DoesNotUpdateMinBufferUsage)
{
    constexpr auto minPayloadSize = 512ull;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(minPayloadSize, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem1);

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedMinMetadataUsage = std::min({ offset1, offset2, offset3, offset4 });
    const auto expectedMinBufferUsage =
        std::min({ minPayloadSize + offset1, 1_KB + offset2, 1_MB + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedMinBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(expectedMinMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadSize, tel.getMinPayloadUsage());
}


/// @test Verify that freeing memory in descending order(last allocation freed first) does not
///       update telemetry peak buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, DescendingMFree_DoesNotUpdatePeakBufferUsage)
{
    constexpr auto maxPayloadSize = 1_MB;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(maxPayloadSize));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem1);

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedPeakMetadataUsage = std::max({ offset1, offset2, offset3, offset4 });
    const auto expectedPeakBufferUsage =
        std::max({ 512ull + offset1, 1_KB + offset2, maxPayloadSize + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedPeakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(expectedPeakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(maxPayloadSize, tel.getPeakPayloadUsage());
}


/// @test Verify that freeing memory in descending order(last allocation freed first) updates
///       free block information.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, DescendingMFree_UpdatesFreeBlockInformation)
{
    const auto mem1    = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1 = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2    = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2 = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3    = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3 = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4    = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4 = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem1);

    const auto totalUsedBuffer = 512 + offset1 + 1_KB + offset2 + 1_MB + offset3 + 256_KB + offset4;

    const auto& tel = tlsf.getTelemetry();
    // NOTE: TLSF Free block won't get update since we are holding onto separate free blocks
    //       1. The largest block since allocations ended
    //       2. One block size of the freed allocations(due to coalescing)
    const auto freedBlockSizes = 512 + offset1 + 1_KB + offset2 + 1_MB + offset3;
    const auto largest         = std::max(tlsfSize - totalUsedBuffer, freedBlockSizes);
    EXPECT_EQ(largest, tel.getLargestFreeBlockSize());
    EXPECT_EQ(2, tel.getFreeBlockCount());
}



/// @test Verify that freeing memory in random order updates telemetry allocation count.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, RandomOrderMFree_UpdatesAllocationCount)
{
    const auto mem1                  = tlsf.malloc(512);
    const auto mem2                  = tlsf.malloc(256_KB);
    const auto mem3                  = tlsf.malloc(1_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(1_MB);
    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(1, tel.getActiveAllocations());
    EXPECT_EQ(3, tel.getLifetimeFrees());
    EXPECT_EQ(4, tel.getLifetimeAllocations());
}


/// @test Verify that freeing memory in random order updates telemetry buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, RandomOrderMFree_UpdatesCurrentBufferUsage)
{
    const auto mem1    = static_cast<uint8_t*>(tlsf.malloc(512));
    const auto mem2    = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto mem3    = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto mem4    = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4 = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);

    const auto totalMetadataSize    = offset4;
    constexpr auto totalPayloadSize = 256_KB;

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(totalPayloadSize + totalMetadataSize, tel.getCurrentBufferUsage());
    EXPECT_EQ(totalMetadataSize, tel.getCurrentMetadataUsage());
    EXPECT_EQ(totalPayloadSize, tel.getCurrentPayloadUsage());
}


/// @test Verify that freeing memory in random order does not update telemetry minimum buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, RandomOrderMFree_DoesNotUpdateMinBufferUsage)
{
    constexpr auto minPayloadSize = 512ull;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(minPayloadSize, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedMinMetadataUsage = std::min({ offset1, offset2, offset3, offset4 });
    const auto expectedMinBufferUsage =
        std::min({ minPayloadSize + offset1, 1_KB + offset2, 1_MB + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedMinBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(expectedMinMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadSize, tel.getMinPayloadUsage());
}


/// @test Verify that freeing memory in random order does not update telemetry peak buffer usages.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, RandomOrderMFree_DoesNotUpdatePeakBufferUsage)
{
    constexpr auto maxPayloadSize = 1_MB;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(maxPayloadSize));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedPeakMetadataUsage = std::max({ offset1, offset2, offset3, offset4 });
    const auto expectedPeakBufferUsage =
        std::max({ 512ull + offset1, 1_KB + offset2, maxPayloadSize + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedPeakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(expectedPeakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(maxPayloadSize, tel.getPeakPayloadUsage());
}


/// @test Verify that freeing memory in random order updates free block information.
TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, RandomOrderMFree_UpdatesFreeBlockInformation)
{
    const auto mem1    = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1 = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2    = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2 = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3    = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3 = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4    = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4 = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);

    const auto totalUsedBuffer = 512 + offset1 + 1_KB + offset2 + 1_MB + offset3 + 256_KB + offset4;

    const auto& tel = tlsf.getTelemetry();
    // NOTE: TLSF Free block won't get update since we are holding onto separate free blocks
    //       1. The largest block since allocations ended
    //       2. One block size of the freed allocations(due to coalescing)
    const auto freedBlockSizes = 512 + offset1 + 1_KB + offset2 + 1_MB + offset3;
    const auto largest         = std::max(tlsfSize - totalUsedBuffer, freedBlockSizes);
    EXPECT_EQ(largest, tel.getLargestFreeBlockSize());
    EXPECT_EQ(2, tel.getFreeBlockCount());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, FreeingAllAllocations_UpdateFreeBlockParameters)
{
    const auto mem1 = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto mem2 = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto mem3 = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto mem4 = static_cast<uint8_t*>(tlsf.malloc(256_KB));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem4);

    const auto& tel = tlsf.getTelemetry();

    EXPECT_EQ(tlsfSize, tel.getLargestFreeBlockSize());
    EXPECT_EQ(1, tel.getFreeBlockCount());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, FreeingAllAllocations_DoesNotUpdateZeroOutLifetimeParameters)
{
    const auto mem1 = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto mem2 = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto mem3 = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto mem4 = static_cast<uint8_t*>(tlsf.malloc(256_KB));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem4);

    const auto& tel = tlsf.getTelemetry();

    EXPECT_EQ(4, tel.getLifetimeFrees());
    EXPECT_EQ(4, tel.getLifetimeAllocations());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, FreeingAllAllocations_UpdatesActiveAllocationsToZero)
{
    const auto mem1 = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto mem2 = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto mem3 = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto mem4 = static_cast<uint8_t*>(tlsf.malloc(256_KB));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem4);

    const auto& tel = tlsf.getTelemetry();

    EXPECT_EQ(0, tel.getActiveAllocations());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, FreeingAllAllocations_CurrentAllocationSizesToZero)
{
    const auto mem1 = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto mem2 = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto mem3 = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto mem4 = static_cast<uint8_t*>(tlsf.malloc(256_KB));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem4);


    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(0, tel.getCurrentBufferUsage());
    EXPECT_EQ(0, tel.getCurrentMetadataUsage());
    EXPECT_EQ(0, tel.getCurrentPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, FreeingAllAllocations_DoesNotUpdateMinBufferUsage)
{
    constexpr auto minPayloadSize = 512ull;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(minPayloadSize, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(1_MB));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem4);

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedMinMetadataUsage = std::min({ offset1, offset2, offset3, offset4 });
    const auto expectedMinBufferUsage =
        std::min({ minPayloadSize + offset1, 1_KB + offset2, 1_MB + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedMinBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(expectedMinMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadSize, tel.getMinPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, FreeingAllAllocations_DoesNotUpdatePeakBufferUsage)
{
    constexpr auto maxPayloadSize = 1_MB;
    const auto mem1               = static_cast<uint8_t*>(tlsf.malloc(512, 128));
    const auto offset1            = *reinterpret_cast<Offset_t*>(mem1 - sizeof(Offset_t));
    const auto mem2               = static_cast<uint8_t*>(tlsf.malloc(1_KB));
    const auto offset2            = *reinterpret_cast<Offset_t*>(mem2 - sizeof(Offset_t));
    const auto mem3               = static_cast<uint8_t*>(tlsf.malloc(maxPayloadSize));
    const auto offset3            = *reinterpret_cast<Offset_t*>(mem3 - sizeof(Offset_t));
    const auto mem4               = static_cast<uint8_t*>(tlsf.malloc(256_KB));
    const auto offset4            = *reinterpret_cast<Offset_t*>(mem4 - sizeof(Offset_t));

    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    tlsf.mfree(mem2);
    tlsf.mfree(mem4);

    // Since we request a large alignment with mem1, we can guarantee a diff buffer usage
    const auto expectedPeakMetadataUsage = std::max({ offset1, offset2, offset3, offset4 });
    const auto expectedPeakBufferUsage =
        std::max({ 512ull + offset1, 1_KB + offset2, maxPayloadSize + offset3, 256_KB + offset4 });

    const auto& tel = tlsf.getTelemetry();
    EXPECT_EQ(expectedPeakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(expectedPeakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(maxPayloadSize, tel.getPeakPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleFree_UpdatesTelemetryParameters)
{
    constexpr size_t allocCount = 1000;
    std::vector<Vec4*> allocations;
    for (size_t i{ 0 }; i < allocCount; ++i)
    {
        const auto floatI = static_cast<float>(i);
        allocations.push_back(tlsf.alloc<Vec4>(floatI, floatI + 1.0f, floatI + 2.0f, floatI + 3.0f));
    }

    // Since max and min usages are not affected by free, update before freeing the allocations
    constexpr size_t max    = std::numeric_limits<size_t>::max();
    size_t minMetadataUsage = max, minBufferUsage = max, minPayloadUsage = sizeof(Vec4);
    size_t peakMetadataUsage = 0, peakBufferUsage = 0, peakPayloadUsage = sizeof(Vec4);
    for (const auto allocation : allocations)
    {
        const auto offset = *reinterpret_cast<Offset_t*>(reinterpret_cast<uint8_t*>(allocation) - sizeof(Offset_t));
        minMetadataUsage  = std::min(minMetadataUsage, static_cast<size_t>(offset));
        peakMetadataUsage = std::max(peakMetadataUsage, static_cast<size_t>(offset));

        minBufferUsage  = std::min(minBufferUsage, minPayloadUsage + offset);
        peakBufferUsage = std::max(peakBufferUsage, peakPayloadUsage + offset);
    }

    /// Free everything except the first allocation
    for (size_t i{ 1 }; i < allocCount; ++i)
    {
        tlsf.free(allocations[i]);
    }

    /// Since we haven't freed the first allocation that will the one only one who will be
    /// registered as used in telemetry.
    const auto offset = *reinterpret_cast<Offset_t*>(reinterpret_cast<uint8_t*>(allocations[0]) - sizeof(Offset_t));
    const auto header = reinterpret_cast<Header*>(reinterpret_cast<uint8_t*>(allocations[0]) - offset);

    size_t curMetadataUsage = offset;
    size_t curPayloadUsage  = sizeof(Vec4);
    size_t curBufferUsage   = header->getSize();

    const auto& tel                = tlsf.getTelemetry();
    const auto largesFreeBlockSize = tlsfSize - curBufferUsage;


    EXPECT_EQ(1, tel.getActiveAllocations());
    EXPECT_EQ(allocCount, tel.getLifetimeAllocations());

    EXPECT_EQ(1, tel.getFreeBlockCount());
    EXPECT_EQ(largesFreeBlockSize, tel.getLargestFreeBlockSize());
    EXPECT_EQ(allocCount - 1, tel.getLifetimeFrees());

    EXPECT_EQ(curBufferUsage, tel.getCurrentBufferUsage());
    EXPECT_EQ(curMetadataUsage, tel.getCurrentMetadataUsage());
    EXPECT_EQ(curPayloadUsage, tel.getCurrentPayloadUsage());
    EXPECT_EQ(minBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(minMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadUsage, tel.getMinPayloadUsage());
    EXPECT_EQ(peakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(peakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(peakPayloadUsage, tel.getPeakPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MultipleFreeV_UpdatesTelemetryParameters)
{
    constexpr size_t allocCount = 100;
    constexpr size_t arrSize    = 10;
    std::vector<std::span<Vec4>> allocations;
    for (size_t i{ 0 }; i < allocCount; ++i)
    {
        allocations.push_back(tlsf.allocV<Vec4>(arrSize));
    }

    constexpr size_t max    = std::numeric_limits<size_t>::max();
    size_t minMetadataUsage = max, minBufferUsage = max, minPayloadUsage = arrSize * sizeof(Vec4);
    size_t peakMetadataUsage = 0, peakBufferUsage = 0, peakPayloadUsage = arrSize * sizeof(Vec4);

    for (const auto allocation : allocations)
    {
        const auto offset =
            *reinterpret_cast<Offset_t*>(reinterpret_cast<uint8_t*>(allocation.data()) - sizeof(Offset_t));

        minMetadataUsage  = std::min(minMetadataUsage, static_cast<size_t>(offset));
        peakMetadataUsage = std::max(peakMetadataUsage, static_cast<size_t>(offset));

        minBufferUsage  = std::min(minBufferUsage, minPayloadUsage + offset);
        peakBufferUsage = std::max(peakBufferUsage, peakPayloadUsage + offset);
    }

    /// Free everything except the first allocation
    for (size_t i{ 1 }; i < allocCount; ++i)
    {
        tlsf.freeV(allocations[i]);
    }

    /// Since we haven't freed the first allocation that will the one only one who will be
    /// registered as used in telemetry.
    const auto offset =
        *reinterpret_cast<Offset_t*>(reinterpret_cast<uint8_t*>(allocations[0].data()) - sizeof(Offset_t));
    const auto header = reinterpret_cast<Header*>(reinterpret_cast<uint8_t*>(allocations[0].data()) - offset);

    size_t curMetadataUsage = offset;
    size_t curPayloadUsage  = sizeof(Vec4) * arrSize;
    size_t curBufferUsage   = header->getSize();

    const auto& tel                = tlsf.getTelemetry();
    const auto largesFreeBlockSize = tlsfSize - curBufferUsage;


    EXPECT_EQ(1, tel.getActiveAllocations());
    EXPECT_EQ(allocCount, tel.getLifetimeAllocations());

    EXPECT_EQ(1, tel.getFreeBlockCount());
    EXPECT_EQ(largesFreeBlockSize, tel.getLargestFreeBlockSize());
    EXPECT_EQ(allocCount - 1, tel.getLifetimeFrees());

    EXPECT_EQ(curBufferUsage, tel.getCurrentBufferUsage());
    EXPECT_EQ(curMetadataUsage, tel.getCurrentMetadataUsage());
    EXPECT_EQ(curPayloadUsage, tel.getCurrentPayloadUsage());
    EXPECT_EQ(minBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(minMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(minPayloadUsage, tel.getMinPayloadUsage());
    EXPECT_EQ(peakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(peakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(peakPayloadUsage, tel.getPeakPayloadUsage());
}



/**************************************
 *             RESIZE                 *
 **************************************/

TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceSmallerThanSplitThreshold_DoesNotUpdateAllocationCount)
{
    constexpr auto size                   = 512;
    const auto mem                        = tlsf.malloc(size);
    const auto& tel                       = tlsf.getTelemetry();
    const auto initialActiveAllocations   = tel.getActiveAllocations();
    const auto initialLifetimeAllocations = tel.getLifetimeAllocations();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD - 1)));

    EXPECT_EQ(initialActiveAllocations, tel.getActiveAllocations());
    EXPECT_EQ(initialLifetimeAllocations, tel.getLifetimeAllocations());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceSmallerThanSplitThreshold_DoesNotUpdateCurrentBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialBufferUsage   = tel.getCurrentBufferUsage();
    const auto initialMetadataUsage = tel.getCurrentMetadataUsage();
    const auto initialPayloadUsage  = tel.getCurrentPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD - 1)));

    EXPECT_EQ(initialBufferUsage, tel.getCurrentBufferUsage());
    EXPECT_EQ(initialMetadataUsage, tel.getCurrentMetadataUsage());
    EXPECT_EQ(initialPayloadUsage, tel.getCurrentPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceSmallerThanSplitThreshold_DoesNotUpdateMinBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialMinBufferUsage   = tel.getMinBufferUsage();
    const auto initialMinMetadataUsage = tel.getMinMetadataUsage();
    const auto initialMinPayloadUsage  = tel.getMinPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD - 1)));

    EXPECT_EQ(initialMinBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(initialMinMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(initialMinPayloadUsage, tel.getMinPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceSmallerThanSplitThreshold_DoesNotUpdatePeakBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialPeakBufferUsage   = tel.getPeakBufferUsage();
    const auto initialPeakMetadataUsage = tel.getPeakMetadataUsage();
    const auto initialPeakPayloadUsage  = tel.getPeakPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD - 1)));

    EXPECT_EQ(initialPeakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(initialPeakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(initialPeakPayloadUsage, tel.getPeakPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceSmallerThanSplitThreshold_DoesNotUpdateFreeBlockInformation)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto offset   = *reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));
    // NOTE: To prevent mutation after the header size is updated on resize, we need to store
    //       the header by value.
    const auto header = *reinterpret_cast<Header*>(mem - offset);
    const auto& tel   = tlsf.getTelemetry();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD - 1)));

    EXPECT_EQ(tlsfSize - header.getSize(), tel.getLargestFreeBlockSize());
    EXPECT_EQ(1, tel.getFreeBlockCount());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceEqualToSplitThreshold_DoesNotUpdateAllocationCount)
{
    constexpr auto size                   = 512;
    const auto mem                        = tlsf.malloc(size);
    const auto& tel                       = tlsf.getTelemetry();
    const auto initialActiveAllocations   = tel.getActiveAllocations();
    const auto initialLifetimeAllocations = tel.getLifetimeAllocations();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD)));

    EXPECT_EQ(initialActiveAllocations, tel.getActiveAllocations());
    EXPECT_EQ(initialLifetimeAllocations, tel.getLifetimeAllocations());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceEqualToSplitThreshold_UpdatesCurrentBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialBufferUsage   = tel.getCurrentBufferUsage();
    const auto initialMetadataUsage = tel.getCurrentMetadataUsage();
    const auto initialPayloadUsage  = tel.getCurrentPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD)));
    // The metadata usage should stay the same since we are only cleaving the left over buffer
    EXPECT_EQ(initialBufferUsage - tlsf.SPLIT_SIZE_THRESHOLD, tel.getCurrentBufferUsage());
    EXPECT_EQ(initialMetadataUsage, tel.getCurrentMetadataUsage());
    EXPECT_EQ(initialPayloadUsage - tlsf.SPLIT_SIZE_THRESHOLD, tel.getCurrentPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceEqualToSplitThreshold_UpdatesMinBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialMinBufferUsage   = tel.getMinBufferUsage();
    const auto initialMinMetadataUsage = tel.getMinMetadataUsage();
    const auto initialMinPayloadUsage  = tel.getMinPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD)));

    // Min usage will be updated but only for buffer and payload usage
    EXPECT_EQ(initialMinBufferUsage - tlsf.SPLIT_SIZE_THRESHOLD, tel.getMinBufferUsage());
    EXPECT_EQ(initialMinMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(initialMinPayloadUsage - tlsf.SPLIT_SIZE_THRESHOLD, tel.getMinPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceEqualToSplitThreshold_DoesNotUpdatePeakBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialPeakBufferUsage   = tel.getPeakBufferUsage();
    const auto initialPeakMetadataUsage = tel.getPeakMetadataUsage();
    const auto initialPeakPayloadUsage  = tel.getPeakPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD)));

    EXPECT_EQ(initialPeakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(initialPeakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(initialPeakPayloadUsage, tel.getPeakPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceEqualToSplitThreshold_UpdatesFreeBlockInformation)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto offset   = *reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));
    // NOTE: To prevent mutation after the header size is updated on resize, we need to store
    //       the header by value.
    const auto header = *reinterpret_cast<Header*>(mem - offset);
    const auto& tel   = tlsf.getTelemetry();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD + 1)));

    // Largest block will be unchanged in our case since the currently freed size is trivial compared to the
    // one we have in store, which almost near the full 2_MB
    EXPECT_EQ(tlsfSize - header.getSize(), tel.getLargestFreeBlockSize());
    EXPECT_EQ(2, tel.getFreeBlockCount());
}



TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceGreaterThanSplitThreshold_DoesNotUpdateAllocationCount)
{
    constexpr auto size                   = 512;
    const auto mem                        = tlsf.malloc(size);
    const auto& tel                       = tlsf.getTelemetry();
    const auto initialActiveAllocations   = tel.getActiveAllocations();
    const auto initialLifetimeAllocations = tel.getLifetimeAllocations();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD + 1)));

    EXPECT_EQ(initialActiveAllocations, tel.getActiveAllocations());
    EXPECT_EQ(initialLifetimeAllocations, tel.getLifetimeAllocations());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceGreaterThanSplitThreshold_UpdatesCurrentBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialBufferUsage   = tel.getCurrentBufferUsage();
    const auto initialMetadataUsage = tel.getCurrentMetadataUsage();
    const auto initialPayloadUsage  = tel.getCurrentPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD + 1)));
    // The metadata usage should stay the same since we are only cleaving the left over buffer
    EXPECT_EQ(initialBufferUsage - (tlsf.SPLIT_SIZE_THRESHOLD + 1), tel.getCurrentBufferUsage());
    EXPECT_EQ(initialMetadataUsage, tel.getCurrentMetadataUsage());
    EXPECT_EQ(initialPayloadUsage - (tlsf.SPLIT_SIZE_THRESHOLD + 1), tel.getCurrentPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceGreaterThanSplitThreshold_UpdatesMinBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialMinBufferUsage   = tel.getMinBufferUsage();
    const auto initialMinMetadataUsage = tel.getMinMetadataUsage();
    const auto initialMinPayloadUsage  = tel.getMinPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD + 1)));

    // Min usage will be updated but only for buffer and payload usage
    EXPECT_EQ(initialMinBufferUsage - (tlsf.SPLIT_SIZE_THRESHOLD + 1), tel.getMinBufferUsage());
    EXPECT_EQ(initialMinMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(initialMinPayloadUsage - (tlsf.SPLIT_SIZE_THRESHOLD + 1), tel.getMinPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceGreaterThanSplitThreshold_DoesNotUpdatePeakBufferUsage)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel     = tlsf.getTelemetry();

    const auto initialPeakBufferUsage   = tel.getPeakBufferUsage();
    const auto initialPeakMetadataUsage = tel.getPeakMetadataUsage();
    const auto initialPeakPayloadUsage  = tel.getPeakPayloadUsage();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD + 1)));

    EXPECT_EQ(initialPeakBufferUsage, tel.getPeakBufferUsage());
    EXPECT_EQ(initialPeakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(initialPeakPayloadUsage, tel.getPeakPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
       ResizeToSmallerSize_WithSizeDifferenceGreaterThanSplitThreshold_UpdatesFreeBlockInformation)
{
    constexpr auto size = 512;
    const auto mem      = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto offset   = *reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));
    // NOTE: To prevent mutation after the header size is updated on resize, we need to store
    //       the header by value.
    const auto header = *reinterpret_cast<Header*>(mem - offset);
    const auto& tel   = tlsf.getTelemetry();

    static_cast<void>(tlsf.resize(mem, size, size - (tlsf.SPLIT_SIZE_THRESHOLD + 1)));

    // Largest block will be unchanged in our case since the currently freed size is trivial compared to the
    // one we have in store, which almost near the full 2_MB
    EXPECT_EQ(tlsfSize - header.getSize(), tel.getLargestFreeBlockSize());
    EXPECT_EQ(2, tel.getFreeBlockCount());
}


// TODO: When resizing to a smaller size that split threshold try to coalesce forward
//       Backward coalesce is not possible since that block is being used and handed back to the user

TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, ResizeToLargerSize_UpdatesLifetimeAllocationCount)
{
    constexpr auto size                   = 512;
    const auto mem                        = tlsf.malloc(size);
    const auto& tel                       = tlsf.getTelemetry();
    const auto initialLifetimeAllocations = tel.getLifetimeAllocations();

    static_cast<void>(tlsf.resize(mem, size, 768));
    // NOTE: There will only be one active allocation but 2 lifetime allocations
    //       since for larger allocations we are freeing and then allocating a new block
    EXPECT_EQ(initialLifetimeAllocations + 1, tel.getLifetimeAllocations());
}

TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, ResizeToLargerSize_DoesNotUpdateActiveAllocationCount)
{
    constexpr auto size                 = 512;
    const auto mem                      = tlsf.malloc(size);
    const auto& tel                     = tlsf.getTelemetry();
    const auto initialActiveAllocations = tel.getActiveAllocations();

    static_cast<void>(tlsf.resize(mem, size, 768));
    // NOTE: There will only be one active allocation but 2 lifetime allocations
    //       since for larger allocations we are freeing and then allocating a new block
    EXPECT_EQ(initialActiveAllocations, tel.getActiveAllocations());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, ResizeToLargerSize_UpdatesCurrentBufferUsage)
{
    constexpr auto size    = 512;
    constexpr auto newSize = 768;
    auto mem               = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel        = tlsf.getTelemetry();

    mem               = static_cast<uint8_t*>(tlsf.resize(mem, size, newSize));
    const auto offset = reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));

    EXPECT_EQ(newSize + *offset, tel.getCurrentBufferUsage());
    EXPECT_EQ(*offset, tel.getCurrentMetadataUsage());
    EXPECT_EQ(newSize, tel.getCurrentPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, ResizeToLargerSize_DoesNotUpdateMinBufferUsage)
{
    constexpr auto size    = 512;
    constexpr auto newSize = 768;
    auto mem               = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel        = tlsf.getTelemetry();

    const auto initialMinBufferUsage   = tel.getMinBufferUsage();
    const auto initialMinMetadataUsage = tel.getMinMetadataUsage();
    const auto initialMinPayloadUsage  = tel.getMinPayloadUsage();

    mem               = static_cast<uint8_t*>(tlsf.resize(mem, size, newSize));
    const auto offset = reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));

    const auto minMetadataUsage = std::min(static_cast<size_t>(*offset), initialMinMetadataUsage);

    // NOTE: Min metadata usage must not update in a ideal situation, i.e,
    //       the padding requirement for both the allocations are the same.
    // And since we are increasing by a larger amount compared to any sizeable difference in padding
    // which can waver only by a maximum of 7(8 being default alignment), we can safely assume min buffer
    // size is unchanged.
    EXPECT_EQ(initialMinBufferUsage, tel.getMinBufferUsage());
    EXPECT_EQ(minMetadataUsage, tel.getMinMetadataUsage());
    EXPECT_EQ(initialMinPayloadUsage, tel.getMinPayloadUsage());
}


TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, ResizeToLargerSize_UpdatesPeakBufferUsage)
{
    constexpr auto size    = 512;
    constexpr auto newSize = 768;
    auto mem               = static_cast<uint8_t*>(tlsf.malloc(size));
    const auto& tel        = tlsf.getTelemetry();

    const auto initialPeakMetadataUsage = tel.getPeakMetadataUsage();

    mem               = static_cast<uint8_t*>(tlsf.resize(mem, size, newSize));
    const auto offset = reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));

    const auto peakMetadataUsage = std::max(static_cast<size_t>(*offset), initialPeakMetadataUsage);

    EXPECT_EQ(newSize + *offset, tel.getPeakBufferUsage());
    EXPECT_EQ(peakMetadataUsage, tel.getPeakMetadataUsage());
    EXPECT_EQ(newSize, tel.getPeakPayloadUsage());
}

// TODO: RENAME THIS TO CONTIGUOUS BLOCK RESIZE
//       ADD NON-CONTIGUOUS BLOCK RESIZE
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, ResizeToLargerSize_UpdatesFreeBlockInformation)
// {
//     constexpr auto size    = 512_KB;
//     constexpr auto newSize = 1200_KB;
//     auto mem               = static_cast<uint8_t*>(tlsf.malloc(size));
//     const auto& tel        = tlsf.getTelemetry();
//
//     mem               = static_cast<uint8_t*>(tlsf.resize(mem, size, newSize));
//     const auto offset = *reinterpret_cast<Offset_t*>(mem - sizeof(Offset_t));
//
//     // Largest Block will be the one we freed with coalesce so left over size + old size
//     const auto expectedFreeBlockSize = tlsfSize - (newSize + offset);
//     EXPECT_EQ(expectedFreeBlockSize, tel.getLargestFreeBlockSize());
//     EXPECT_EQ(2, tel.getFreeBlockCount());
// }



/**************************************
 *                                    *
 *           INTERNAL TESTS           *
 *                                    *
 **************************************/

// Namespacing is required for testing internal state
namespace pmm
{} // namespace pmm
