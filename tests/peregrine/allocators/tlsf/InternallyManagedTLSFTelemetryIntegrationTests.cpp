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


//
// /**************************************
//  *            ALLOC BYTES             *
//  **************************************/
//
// /**
//  * @test Verify that malloc returns an address aligned to sizeof(void*) bytes
//  *       given no alignment was passed-in.
//  */
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Malloc_Returns8ByteAlignedAddressByDefault)
// {
//     // Misalign bytes to 2
//     [[maybe_unused]] void* misalignedBytes = tlsf.malloc(2, 2);
//
//     void* bytes = tlsf.malloc(8);
//
//     const auto address = reinterpret_cast<uintptr_t>(bytes);
//     EXPECT_EQ(0, address % sizeof(void*));
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Malloc_ReturnsProvidedByteAlignedAddress)
// {
//     constexpr auto byteAlignment = 32;
//     void* bytes                  = tlsf.malloc(128, byteAlignment);
//
//     const auto address = reinterpret_cast<uintptr_t>(bytes);
//     EXPECT_EQ(0, address % byteAlignment);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Malloc_ReturnsNonNullPtrWhenAllocatingMemoryLessThanTLSFSize)
// {
//     void* bytes = tlsf.malloc(256);
//
//     EXPECT_NE(nullptr, bytes);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Malloc_ReturnsNonNullPtrWhenAllocatingMemoryEqualTLSFSize)
// {
//
//     // 15 bytes used for worst case alignment, 16-bytes for header, and 4 bytes for offset.
//     void* bytes = tlsf.malloc(tlsfSize - 64);
//
//     EXPECT_NE(nullptr, bytes);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Malloc_SubsequentAllocationDoNotCorruptMemory)
// {
//     constexpr auto bufferLength = 8;
//     // Given two contiguous block of memory allocated back to back
//     const auto firstAlloc = static_cast<int*>(tlsf.malloc(bufferLength * sizeof(int)));
//     for (std::size_t i = 0; i < bufferLength; ++i)
//     {
//         firstAlloc[i] = static_cast<int>(i + 5);
//     }
//
//     const auto secondAlloc = static_cast<int*>(tlsf.malloc(bufferLength * sizeof(int)));
//     for (std::size_t i = 0; i < bufferLength; ++i)
//     {
//         secondAlloc[i] = static_cast<int>(i + 7);
//     }
//
//     // When read back there is no corruption
//     for (std::size_t i = 0; i < bufferLength; ++i)
//     {
//         EXPECT_EQ(static_cast<int>(i + 5), firstAlloc[i]);
//         EXPECT_EQ(static_cast<int>(i + 7), secondAlloc[i]);
//     }
// }
//
// // TODO: Add more TLSF allocation tests.
//
// // TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Malloc_UpdatesTelemetry)
// // {
// //     constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;
// //
// //     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
// //     static_cast<void>(tlsf.malloc(byte1));
// //     static_cast<void>(tlsf.malloc(byte2));
// //     static_cast<void>(tlsf.malloc(byte3));
// //
// //     constexpr std::size_t expectedMinUsage  = byte1;
// //     constexpr std::size_t expectedPeakUsage = byte3;
// //     constexpr std::size_t expectedUsage     = byte1 + byte2 + byte3;
// //
// //     EXPECT_EQ(expectedMinUsage, tlsf.getTelemetry().getMinUsage());
// //     EXPECT_EQ(expectedPeakUsage, tlsf.getTelemetry().getPeakUsage());
// //     EXPECT_EQ(expectedUsage, tlsf.getTelemetry().getUsedSize());
// // }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Malloc_HeaderIsPreservedInAddressBeforeGivenAddress)
// {
//     // NOTE: This tests works on the premise that the allocated memory follows a
//     // [Header][Padding][OffsetToHeader][Ptr given to user] pattern
//     // and the OffsetToHeader is not itself corrupted.
//     constexpr auto allocSize = 64;
//     auto bytes               = static_cast<uint8_t*>(tlsf.malloc(allocSize));
//     using Offset_t           = pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled>::HeaderOffset_t;
//     using Header_t           = pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled>::Header;
//
//     const auto offset = reinterpret_cast<Offset_t*>(bytes - sizeof(Offset_t));
//     const auto header = reinterpret_cast<Header_t*>(bytes - *offset);
//
//     const auto expectedSize = *offset + allocSize;
//     // Padding equals the size left in the in offset after subtracting size of Header and HeaderOffset
//     const auto expectedPadding = *offset - (sizeof(Offset_t) + sizeof(Header_t));
//     EXPECT_EQ(expectedSize, header->getSize());
//     EXPECT_EQ(expectedPadding, header->padding);
// }
//
// /// @test Verify that free marks the internal buffer as free.
// /// @note While we can't directly test this, we can allocate a near full size
// ///       allocation and requesting a larger allocation after free shouldn't trigger
// ///       an out-of-memory exception.
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MFree_FreeTheBuffer)
// {
//     // TODO: This test can be used for checking if allocations smaller than min chunk size
//     //       cleaves the memory.
//     // Leeway to ensure the allocation passes.
//     constexpr auto leeway = 32;
//     const auto firstMem   = tlsf.malloc(tlsfSize - leeway);
//     tlsf.mfree(firstMem);
//     // If the allocation fails this will trigger an exception in DEBUG
//     // and its UB in Release Mode(without SafeMode)
//     const auto secondMem = tlsf.malloc(tlsfSize - leeway);
//     EXPECT_NE(nullptr, secondMem);
// }
//
// /// @test Verify that free perform right only coalesce (latest allocations are freed in order).
// ///       AllocA, AllocB, AllocC, FreeB, FreeA
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MFree_PerformsRightOnlyCoalesce)
// {
//     // Total Memory size is 2KB so this would around half the memory or more.
//     constexpr size_t firstAllocSize{ 128_KB }, secondAllocSize{ 64_KB }, thirdAllocSize{ 255 };
//     const auto firstAlloc                  = tlsf.malloc(firstAllocSize);
//     const auto secondAlloc                 = tlsf.malloc(secondAllocSize);
//     [[maybe_unused]] const auto thirdAlloc = tlsf.malloc(thirdAllocSize);
//
//     tlsf.mfree(secondAlloc);
//     tlsf.mfree(firstAlloc);
//
//     // There should be two free blocks since we didn't free the middle block
//     // so, we can allocate a buffer of size firstSize + secondSize and another that has the remainingSize
//     // with leeway.
//     // We need a larger leeway here to account for the fact that tlsf rounds up the size requirement to the next
//     nearest
//     // SL boundary and at nearly 2MB, it will be 16_KB((2MB - 1MB) / (2^6)) where 6 is the L value; 16_KB.
//     const auto fourthAlloc = tlsf.malloc(tlsfSize - (firstAllocSize + secondAllocSize + thirdAllocSize + 16_KB));
//     const auto fifthAlloc  = tlsf.malloc(firstAllocSize + secondAllocSize - 32);
//     EXPECT_NE(nullptr, fourthAlloc);
//     EXPECT_NE(nullptr, fifthAlloc);
// }
//
//
// /// @test Verify that free perform left-only coalesce (first allocations are freed in order).
// ///       AllocA, AllocB, AllocC, FreeA, FreeB.
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MFree_PerformsLeftOnlyCoalesce)
// {
//     // Total Memory size is 2KB so this would around half the memory or more.
//     constexpr size_t firstAllocSize{ 128_KB }, secondAllocSize{ 64_KB }, thirdAllocSize{ 255 };
//     const auto firstAlloc                  = tlsf.malloc(firstAllocSize);
//     const auto secondAlloc                 = tlsf.malloc(secondAllocSize);
//     [[maybe_unused]] const auto thirdAlloc = tlsf.malloc(thirdAllocSize);
//
//     tlsf.mfree(firstAlloc);
//     tlsf.mfree(secondAlloc);
//
//     // There should be two free blocks since we didn't free the middle block
//     // so, we can allocate a buffer of size firstSize + secondSize and another that has the remainingSize
//     // with leeway.
//     // We need a larger leeway here to account for the fact that tlsf rounds up the size requirement to the next
//     nearest
//     // SL boundary and at nearly 2MB, it will be 16_KB((2MB - 1MB) / (2^6)) where 6 is the L value; 16_KB.
//     const auto fourthAlloc = tlsf.malloc(tlsfSize - (firstAllocSize + secondAllocSize + thirdAllocSize + 16_KB));
//     const auto fifthAlloc  = tlsf.malloc(firstAllocSize + secondAllocSize - 32);
//     EXPECT_NE(nullptr, fourthAlloc);
//     EXPECT_NE(nullptr, fifthAlloc);
// }
//
//
// /// @test Verify that free perform right only coalesce (allocations freed in a mixed order).
// ///       AllocA, AllocB, AllocC, FreeC, FreeA, FreeB.
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MFree_PerformsMixedCoalesce)
// {
//     // Total Memory size is 2KB so this would around half the memory or more.
//     constexpr auto firstAllocSize{ 512 }, secondAllocSize{ 128 }, thirdAllocSize{ 255 };
//     const auto firstAlloc  = tlsf.malloc(firstAllocSize);
//     const auto secondAlloc = tlsf.malloc(secondAllocSize);
//     const auto thirdAlloc  = tlsf.malloc(thirdAllocSize);
//
//     tlsf.mfree(thirdAlloc);
//     tlsf.mfree(firstAlloc);
//     tlsf.mfree(secondAlloc);
//
//     // Here 128 is leeway
//     const auto fourthAlloc = tlsf.malloc(tlsfSize - 64);
//     EXPECT_NE(nullptr, fourthAlloc);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MFree_PerformRightCoalesceWithMultipleAllocations)
// {
//     std::vector<void*> allocations;
//     constexpr auto leeway            = 32;
//     constexpr auto perAllocationSize = 2_KB;
//     const auto numAllocations        = tlsfSize / (perAllocationSize + leeway);
//     // Allocate memory
//     for (size_t i = 0; i < numAllocations; ++i)
//     {
//         allocations.push_back(tlsf.malloc(perAllocationSize));
//     }
//
//     // Free memory
//     // Note size_t can wrap around when hitting --1, so we can internally use zero index.
//     for (size_t i = numAllocations; i > 0; --i)
//     {
//         tlsf.mfree(allocations[i - 1]);
//     }
//
//     // Try allocating a new full size allocation
//     const auto finalAllocation = tlsf.malloc(tlsfSize - leeway);
//     EXPECT_NE(nullptr, finalAllocation);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MFree_PerformLeftCoalesceWithMultipleAllocations)
// {
//     std::vector<void*> allocations;
//     constexpr auto leeway            = 32;
//     constexpr auto perAllocationSize = 2_KB;
//     const auto numAllocations        = tlsfSize / (perAllocationSize + leeway);
//     // Allocate memory
//     for (size_t i = 0; i < numAllocations; ++i)
//     {
//         allocations.push_back(tlsf.malloc(perAllocationSize));
//     }
//
//     // Free memory
//     // Note size_t can wrap around when hitting --1, so we can internally use zero index.
//     for (size_t i = numAllocations; i > 0; --i)
//     {
//         tlsf.mfree(allocations[i - 1]);
//     }
//
//     // Try allocating a new full size allocation
//     const auto finalAllocation = tlsf.malloc(tlsfSize - leeway);
//     EXPECT_NE(nullptr, finalAllocation);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, MFree_PerformCoalesceWithMixedIntermittentFrees)
// {
//     std::vector<void*> allocations;
//     constexpr auto leeway            = 32;
//     constexpr auto perAllocationSize = 2_KB;
//     const auto numAllocations        = tlsfSize / (perAllocationSize + leeway);
//     // Allocate memory
//     for (size_t i = 0; i < numAllocations; ++i)
//     {
//         allocations.push_back(tlsf.malloc(perAllocationSize));
//     }
//
//     // Free memory
//     // Note size_t can wrap around when hitting --1, so we can internally use zero index.
//     for (size_t i = 0; i < numAllocations; i += 2) // Free even indexed allocations
//     {
//         tlsf.mfree(allocations[i]);
//     }
//     for (size_t i = 1; i < numAllocations; i += 2) // Free odd indexed allocations
//     {
//         tlsf.mfree(allocations[i]);
//     }
//
//     // Try allocating a new full size allocation
//     const auto finalAllocation = tlsf.malloc(tlsfSize - leeway);
//     EXPECT_NE(nullptr, finalAllocation);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Free_CallsClassDestructorForNonTrivialTypes)
// {
//     int numDestructorCalls = 0;
//     const auto nonTrivial  = tlsf.alloc<DestructionTracker>(&numDestructorCalls);
//
//     tlsf.free(nonTrivial);
//     EXPECT_EQ(1, numDestructorCalls);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Free_FreesMemoryForNewAllocations)
// {
//     const auto firstAlloc = tlsf.alloc<LargeData<tlsfSize - 1_KB>>();
//     tlsf.free(firstAlloc);
//
//     // This trigger assertion in debug if the memory is not freed
//     const auto newAlloc = tlsf.alloc<LargeData<tlsfSize - 1_KB>>();
//     EXPECT_NE(nullptr, newAlloc);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, FreeV_FreesMemoryForSubsequentAllocations)
// {
//
//     // NOTE: 64 bytes is some leeway for buffer header and alignment
//     constexpr auto leeway = 64;
//     // Should saturate the buffer as 4 * 1200 = 4800, near buffer size of 5_KB
//     // Allocate some test data
//     const auto listData = tlsf.allocV<int>(1200);
//     // Free it
//     tlsf.freeV(listData);
//
//     const auto intV = tlsf.allocV<int>(tlsfSize / sizeof(int) - leeway);
//
//     // Allocate Memory
//     for (std::size_t i = 0; i < intV.size(); ++i)
//     {
//         intV[i] = static_cast<int>(i + 316);
//     }
//
//     // Verify the allocation is successful with data writes
//     for (std::size_t i = 0; i < intV.size(); ++i)
//     {
//         EXPECT_EQ(static_cast<int>(i + 316), intV[i]);
//     }
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, FreeV_CallsClassDestructorForNonTrivialTypes)
// {
//     // @Warning Not thread safe
//     int numDestructorCalls       = 0;
//     constexpr auto numAllocation = 500;
//     auto nonTrivial              = tlsf.allocV<DestructionTracker>(numAllocation);
//     for (auto& item : nonTrivial)
//     {
//         item.destructorCalledCount = &numDestructorCalls;
//     }
//
//     tlsf.freeV(nonTrivial);
//
//     EXPECT_EQ(numAllocation, numDestructorCalls);
// }
//
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Resize_SameSizeReturnsSameAddress)
// {
//     constexpr auto oldSize = 1_KB, newSize = 1_KB;
//     // Note: While the test uses two variables for holding old and resized memory address, it is not
//     //       recommended for production use since that can lead to dangling pointers and memory corruptions
//     //       (if data is written to it).
//     const auto mem     = tlsf.malloc(oldSize);
//     const auto resized = tlsf.resize(mem, oldSize, newSize);
//
//     EXPECT_NE(nullptr, resized);
//     EXPECT_EQ(mem, resized);
// }
//
//
// /// @test Verify that when resizing a buffer to a smaller size with the size difference
// ///       smaller than split threshold, returns the same memory address.
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
//        Resize_ToSmallerSize_SizeDiffSmallerThanSplitThreshold_ReturnsSameAddress)
// {
//     constexpr auto oldSize = 1_KB, newSize = 1_KB - (pmm::TLSF<pmm::MemPolicy::Internal,
//     pmm::TelPolicy::Enabled>::SPLIT_SIZE_THRESHOLD - 1);
//     // Note: While the test uses two variables for holding old and resized memory address, it is not
//     //       recommended for production use since that can lead to dangling pointers and memory corruptions
//     //       (if data is written to it).
//     const auto mem     = tlsf.malloc(oldSize);
//     const auto resized = tlsf.resize(mem, oldSize, newSize);
//
//     EXPECT_NE(nullptr, resized);
//     EXPECT_EQ(mem, resized);
// }
//
//
// /// @test Verify that when resizing a buffer to a smaller size with the size difference
// ///       equalling split threshold, returns the same memory address.
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
//        Resize_ToSmallerSize_SizeDiffEqualToSplitThreshold_ReturnsSameAddress)
// {
//     constexpr auto oldSize = 1_KB, newSize = 1_KB - (pmm::TLSF<pmm::MemPolicy::Internal,
//     pmm::TelPolicy::Enabled>::SPLIT_SIZE_THRESHOLD);
//     // Note: While the test uses two variables for holding old and resized memory address, it is not
//     //       recommended for production use since that can lead to dangling pointers and memory corruptions
//     //       (if data is written to it).
//
//     const auto mem     = tlsf.malloc(oldSize);
//     const auto resized = tlsf.resize(mem, oldSize, newSize);
//
//     EXPECT_NE(nullptr, resized);
//     EXPECT_EQ(mem, resized);
// }
//
//
// /// @test Verify that when resizing a buffer to a smaller size with the size difference
// ///       greater than split threshold, returns the same memory address.
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests,
//        Resize_ToSmallerSize_SizeDiffGreaterThanSplitThreshold_ReturnsSameAddress)
// {
//     constexpr auto oldSize = 1_KB, newSize = 1_KB - (pmm::TLSF<pmm::MemPolicy::Internal,
//     pmm::TelPolicy::Enabled>::SPLIT_SIZE_THRESHOLD + 1);
//     // Note: While the test uses two variables for holding old and resized memory address, it is not
//     //       recommended for production use since that can lead to dangling pointers and memory corruptions
//     //       (if data is written to it).
//
//     const auto mem     = tlsf.malloc(oldSize);
//     const auto resized = tlsf.resize(mem, oldSize, newSize);
//
//     EXPECT_NE(nullptr, resized);
//     EXPECT_EQ(mem, resized);
// }
//
//
// /// @test Verify that when resizing a buffer to a larger size returns a new memory address.
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Resize_LargerSizeSizeReturnsNewAddress)
// {
//     constexpr auto oldSize = 1_KB, newSize = 5_KB;
//     // Note: While the test uses two variables for holding old and resized memory address, it is not
//     //       recommended for production use since that can lead to dangling pointers and memory corruptions
//     //       (if data is written to it).
//
//     const auto mem     = tlsf.malloc(oldSize);
//     const auto resized = tlsf.resize(mem, oldSize, newSize);
//
//     EXPECT_NE(nullptr, resized);
//     EXPECT_NE(mem, resized);
// }
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Resize_LargerSizeSizeCopiesContentFromOldMemory)
// {
//     constexpr auto oldSize = 1_KB, newSize = 5_KB;
//     constexpr auto elementCount = oldSize / sizeof(int);
//     // Note: While the test uses two variables for holding old and resized memory address, it is not
//     //       recommended for production use since that can lead to dangling pointers and memory corruptions
//     //       (if data is written to it).
//     const auto mem = static_cast<int*>(tlsf.malloc(oldSize));
//     for (size_t i = 0; i < elementCount; ++i)
//     {
//         mem[i] = static_cast<int>(i + 13);
//     }
//
//     const auto resized = static_cast<int*>(tlsf.resize(mem, oldSize, newSize));
//     for (size_t i = 0; i < elementCount; ++i)
//     {
//         EXPECT_EQ(static_cast<int>(i + 13), resized[i]);
//     }
// }
//
//
// /// @test Verify that clear clears the TLSF allowing for new allocations.
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Clear_ClearsTLSFAllowingForNewAllocations)
// {
//     // Make some allocations and free
//     [[maybe_unused]] const auto mem1 = tlsf.malloc(1_KB);
//     [[maybe_unused]] const auto mem2 = tlsf.malloc(11_KB);
//     tlsf.mfree(mem1);
//     [[maybe_unused]] const auto mem3 = tlsf.malloc(15_KB);
//     [[maybe_unused]] const auto mem4 = tlsf.malloc(2_KB);
//     [[maybe_unused]] const auto mem5 = tlsf.malloc(2_KB);
//     tlsf.mfree(mem4);
//
//     // Clear tlsf
//     tlsf.clear();
//
//     // Make an allocation near the buffer size
//     const auto newMem = tlsf.malloc(tlsfSize - 32);
//
//     EXPECT_NE(nullptr, newMem);
// }
//
//
// /**************************************
//  *              ALLOC                 *
//  **************************************/
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Alloc_AllocatesAnObjectInTheTLSF)
// {
//     const auto vec = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
//
//     EXPECT_FLOAT_EQ(1.0f, vec->x);
//     EXPECT_FLOAT_EQ(2.0f, vec->y);
//     EXPECT_FLOAT_EQ(3.0f, vec->z);
//     EXPECT_FLOAT_EQ(4.0f, vec->w);
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Alloc_AlignsToTargetAlignment)
// {
//     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
//     static_cast<void>(tlsf.malloc(2, 2));
//
//     constexpr auto expectedAlignment = alignof(Vec4);
//     [[maybe_unused]] const auto vec  = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
//
//     EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vec) % expectedAlignment);
// }
//
//
// // TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, Alloc_UpdatesTelemetry)
// // {
// //     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
// //     static_cast<void>(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
// //     static_cast<void>(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
// //     static_cast<void>(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
// //     static_cast<void>(tlsf.alloc<int>(1));
// //
// //     constexpr std::size_t expectedMinUsage  = sizeof(int);
// //     constexpr std::size_t expectedPeakUsage = sizeof(Vec4);
// //     constexpr std::size_t expectedUsage     = sizeof(Vec4) * 3 + sizeof(int);
// //
// //     EXPECT_EQ(expectedMinUsage, tlsf.getTelemetry().getMinUsage());
// //     EXPECT_EQ(expectedPeakUsage, tlsf.getTelemetry().getPeakUsage());
// //     EXPECT_EQ(expectedUsage, tlsf.getTelemetry().getUsedSize());
// // }
//
//
//
// /**************************************
//  *            ALLOC V(ector)           *
//  **************************************/
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, AllocV_ReturnsAContinguousBlockOfMemory)
// {
//     constexpr auto blockCount = 10;
//     const auto vertices       = tlsf.allocV<Vec4>(blockCount);
//
//     EXPECT_EQ(blockCount, vertices.size());
//     EXPECT_EQ(blockCount * sizeof(Vec4), vertices.size_bytes());
// }
//
//
// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, AllocV_SubsequentAllocationDoNotCorruptMemory)
// {
//     constexpr auto blockCount       = 5;
//     constexpr std::array vertexData = {
//         1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,  9.0f,  10.0f,
//         11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f,
//     };
//     constexpr std::array edgeData = {
//         101.0f, 102.0f, 103.0f,  104.0f, 105.0f, 106.0f, 107.0f,  108.0f, 109.0f, 110.0f,
//         111.0f, 112.0f, 1013.0f, 114.0f, 115.0f, 116.0f, 1017.0f, 118.0f, 119.0f, 120.0f,
//     };
//
//     auto vertices = tlsf.allocV<Vec4>(blockCount);
//     auto edges    = tlsf.allocV<Vec4>(blockCount);
//
//     // Write into the first allocated span
//     for (std::size_t i = 0; i < blockCount; ++i)
//     {
//         vertices[i] = Vec4{ vertexData[i * 4], vertexData[i * 4 + 1], vertexData[i * 4 + 2], vertexData[i * 4 + 3] };
//     }
//
//     // Write into the second allocated span
//     for (std::size_t i = 0; i < blockCount; ++i)
//     {
//         edges[i] = Vec4{ edgeData[i * 4], edgeData[i * 4 + 1], edgeData[i * 4 + 2], edgeData[i * 4 + 3] };
//     }
//
//
//     // Verify data integrity is maintained for both
//     for (std::size_t i = 0; i < blockCount; ++i)
//     {
//         constexpr auto epsilon = 1e-5;
//         const auto vert        = vertices[i];
//         EXPECT_NEAR(vertexData[i * 4], vert.x, epsilon);
//         EXPECT_NEAR(vertexData[i * 4 + 1], vert.y, epsilon);
//         EXPECT_NEAR(vertexData[i * 4 + 2], vert.z, epsilon);
//         EXPECT_NEAR(vertexData[i * 4 + 3], vert.w, epsilon);
//
//         const auto edge = edges[i];
//         EXPECT_NEAR(edgeData[i * 4], edge.x, epsilon);
//         EXPECT_NEAR(edgeData[i * 4 + 1], edge.y, epsilon);
//         EXPECT_NEAR(edgeData[i * 4 + 2], edge.z, epsilon);
//         EXPECT_NEAR(edgeData[i * 4 + 3], edge.w, epsilon);
//     }
// }


// TEST_F(InternallyManagedTLSFTelemetryIntegrationTests, AllocV_UpdatesTelemetry)
// {
//     constexpr std::size_t count1 = 2, count2 = 4, count3 = 6;
//
//     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
//     static_cast<void>(tlsf.allocV<Vec4>(count1));
//     static_cast<void>(tlsf.allocV<Vec4>(count2));
//     static_cast<void>(tlsf.allocV<Vec4>(count3));
//
//     constexpr std::size_t expectedMinUsage  = count1 * sizeof(Vec4);
//     constexpr std::size_t expectedPeakUsage = count3 * sizeof(Vec4);
//     constexpr std::size_t expectedUsage     = (count1 + count2 + count3) * sizeof(Vec4);
//
//     EXPECT_EQ(expectedMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(expectedPeakUsage, tlsf.getTelemetry().getPeakUsage());
//     EXPECT_EQ(expectedUsage, tlsf.getTelemetry().getUsedSize());
// }


/**************************************
 *                                    *
 *           INTERNAL TESTS           *
 *                                    *
 **************************************/

// Namespacing is required for testing internal state
namespace pmm
{} // namespace pmm
