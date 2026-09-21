/**
 * @file InternallyManagedTLSFTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 08, 2026
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
    class InternallyManagedTLSFTests: public testing::Test
    {
    public:
        static constexpr size_t tlsfSize{ 2_MB };
        pmm::TLSF<> tlsf{ tlsfSize };
    };


    struct TLSFMappingInsertParams
    {
        size_t allocationSize, flIndex, slIndex;

        friend std::ostream& operator<<(std::ostream& os, const TLSFMappingInsertParams& params)
        {
            os << std::format("Size: {}, Expected FL: {}, Expected SL: {}", params.allocationSize, params.flIndex,
                              params.slIndex);
            return os;
        }
    };


    /// @brief Test fixture for TLSF mapping insert function.
    class InternallyManagedTLSF_MappingInsertTests: public testing::TestWithParam<TLSFMappingInsertParams>
    {};

    INSTANTIATE_TEST_SUITE_P(
        TLSF_InternalMappingTests, InternallyManagedTLSF_MappingInsertTests,
        ::testing::Values(TLSFMappingInsertParams{ .allocationSize = 0, .flIndex = 0, .slIndex = 0 },
                          // Values that are clamped to the minimum allocation size of 64 bytes(2^6)
                          TLSFMappingInsertParams{ .allocationSize = 15, .flIndex = 0, .slIndex = 0 },
                          TLSFMappingInsertParams{ .allocationSize = 64, .flIndex = 0, .slIndex = 0 },
                          TLSFMappingInsertParams{ .allocationSize = 65, .flIndex = 0, .slIndex = 1 },
                          TLSFMappingInsertParams{ .allocationSize = 66, .flIndex = 0, .slIndex = 2 },
                          TLSFMappingInsertParams{ .allocationSize = 127, .flIndex = 0, .slIndex = 63 },
                          TLSFMappingInsertParams{ .allocationSize = 128, .flIndex = 1, .slIndex = 0 },
                          TLSFMappingInsertParams{ .allocationSize = 256, .flIndex = 2, .slIndex = 0 },
                          TLSFMappingInsertParams{ .allocationSize = 512, .flIndex = 3, .slIndex = 0 },
                          TLSFMappingInsertParams{ .allocationSize = 1024, .flIndex = 4, .slIndex = 0 },
                          TLSFMappingInsertParams{ .allocationSize = 513, .flIndex = 3, .slIndex = 0 },
                          TLSFMappingInsertParams{ .allocationSize = 520, .flIndex = 3, .slIndex = 1 },
                          TLSFMappingInsertParams{ .allocationSize = 1000, .flIndex = 3, .slIndex = 61 },
                          TLSFMappingInsertParams{ .allocationSize = 1_MB, .flIndex = 14, .slIndex = 0 },
                          TLSFMappingInsertParams{ .allocationSize = 1_MB + 16_KB, .flIndex = 14, .slIndex = 1 },
                          TLSFMappingInsertParams{ .allocationSize = 1_GB, .flIndex = 24, .slIndex = 0 }));


    /// @brief Test fixture for TLSF mapping search function.
    class InternallyManagedTLSF_MappingSearchTests: public testing::TestWithParam<TLSFMappingInsertParams>
    {};

    INSTANTIATE_TEST_SUITE_P(TLSF_InternalMappingTests, InternallyManagedTLSF_MappingSearchTests,
                             ::testing::Values(
                                 // Minimum value index (our LSB is considered to be 64 or 2^6)
                                 // 0000 0001 -> <FL=0, SL=0>
                                 TLSFMappingInsertParams{ .allocationSize = 64, .flIndex = 0, .slIndex = 0 },
                                 // Values that are clamped to the minimum allocation size of 64 bytes(2^6)
                                 // 0000 0100 -> <FL=2, SL=0>
                                 TLSFMappingInsertParams{ .allocationSize = 256, .flIndex = 2, .slIndex = 0 },
                                 // 0000 0101 -> <FL=2, SL=1>
                                 TLSFMappingInsertParams{ .allocationSize = 257, .flIndex = 2, .slIndex = 1 },
                                 // 0000 0111 Rounded to next block(0000 1000)-> <FL=3, SL=0>
                                 TLSFMappingInsertParams{ .allocationSize = 511, .flIndex = 3, .slIndex = 0 },
                                 TLSFMappingInsertParams{ .allocationSize = 66, .flIndex = 0, .slIndex = 2 },
                                 // FL-4, SL-61 as the buckets are 61 [1000, 1008), 62 [1008, 1016), 63 [1016, 1024)
                                 TLSFMappingInsertParams{ .allocationSize = 1000, .flIndex = 3, .slIndex = 61 },
                                 TLSFMappingInsertParams{ .allocationSize = 1018, .flIndex = 4, .slIndex = 0 },
                                 TLSFMappingInsertParams{ .allocationSize = 1024, .flIndex = 4, .slIndex = 0 },
                                 TLSFMappingInsertParams{ .allocationSize = 1025, .flIndex = 4, .slIndex = 1 },
                                 TLSFMappingInsertParams{ .allocationSize = 1_MB, .flIndex = 14, .slIndex = 0 },
                                 // 1 MB nicely packs into the 14nth fl index
                                 // And our sl index range at than size is 2^20(1MB) / 64(buckets) = 16KB(2^14)
                                 // so it will fall into the [1MB_16KB, 1MB_32KB) bucket at index 1, due to rounding.
                                 TLSFMappingInsertParams{ .allocationSize = 1_MB + 15_KB, .flIndex = 14, .slIndex = 1 },
                                 TLSFMappingInsertParams{ .allocationSize = 1_MB + 16_KB, .flIndex = 14, .slIndex = 1 },
                                 TLSFMappingInsertParams{ .allocationSize = 1_MB + 17_KB, .flIndex = 14, .slIndex = 2 },
                                 TLSFMappingInsertParams{ .allocationSize = 1_GB, .flIndex = 24, .slIndex = 0 }));

    /**************************************
     *           STATIC TESTS             *
     **************************************/

    namespace static_tests
    {
        /** @test Verify that manged tlsf frees buffer it allocates.
         *  @note Since we cant really confirm confirm if a buffer is freed and we only delete[] buffer in the dtor of
         *        TLSF, we can check if it is trivially destructible to ensure memory is freed in the tlsf in unmanaged
         * mode and opposite otherwise.
         */
        static_assert(std::is_trivially_destructible_v<pmm::TLSF<pmm::MemPolicy::Internal>> == false);
    } // namespace static_tests

} // namespace



/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/**************************************
 *           INITIALIZATIONS          *
 **************************************/
//
// TEST_F(InternallyManagedTLSFTests, EnabledTelemetry_ReturnsRealTelemetry)
// {
//     [[maybe_unused]] pmm::TLSF<pmm::MemPolicy::Internal, pmm::telemetry::Enabled> telemetryEnabledTLSF(512);
//     [[maybe_unused]] auto telemetry = telemetryEnabledTLSF.getTelemetry();
//     const bool result               = std::is_same_v<decltype(telemetry), pmm::TLSFTelemetry>;
//     EXPECT_TRUE(result);
// }
//
//
// TEST_F(InternallyManagedTLSFTests, DisabledTelemetry_ReturnsDummyTelemetry)
// {
//     [[maybe_unused]] const pmm::TLSF<pmm::MemPolicy::Internal, pmm::telemetry::Disabled> telemetryDisabledTLSF(512);
//     [[maybe_unused]] auto telemetry = telemetryDisabledTLSF.getTelemetry();
//     const bool result               = std::is_same_v<decltype(telemetry), pmm::DummyTLSFTelemetry>;
//     EXPECT_TRUE(result);
// }


TEST_F(InternallyManagedTLSFTests, Ctor_InitializesTLSFWithTheGivenBytes) { EXPECT_EQ(tlsfSize, tlsf.size()); }


TEST_F(InternallyManagedTLSFTests, TLSFHasZeroUsedSizeInitially) { EXPECT_EQ(0, tlsf.usedSize()); }


TEST_F(InternallyManagedTLSFTests, TLSFHasFreeSpaceEqualToSizeInitially) { EXPECT_EQ(tlsfSize, tlsf.freeSize()); }


TEST_F(InternallyManagedTLSFTests, MoveCtor_CopiesAttributesToNewObject)
{
    const pmm::TLSF<> tlsf2 = std::move(tlsf);
    EXPECT_EQ(tlsfSize, tlsf2.freeSize());
    EXPECT_EQ(tlsfSize, tlsf2.size());
    EXPECT_EQ(0, tlsf2.usedSize());
    // TODO: Add back after telemetry
    // EXPECT_EQ(tlsfSize, tlsf2.getTelemetry().getTLSFSize());
}
//
// TODO: Add back after adding telemetry and malloc
// TEST_F(InternallyManagedTLSFTests, MoveCtor_MovesTelemetry)
// {
//     static_cast<void>(tlsf.malloc(120));
//     static_cast<void>(tlsf.malloc(240));
//     // Get the telemetry to ensure that the value is preserved when moving
//     // DON'T get by reference as it will change internally
//     const auto telemetry = tlsf.getTelemetry();
//
//     const pmm::TLSF<> tlsf2 = std::move(tlsf);
//
//     // Checking for telemetry equality
//     EXPECT_EQ(telemetry.getUsedSize(), tlsf2.getTelemetry().getUsedSize());
//     EXPECT_EQ(telemetry.getPeakUsage(), tlsf2.getTelemetry().getPeakUsage());
//     EXPECT_EQ(telemetry.getTLSFSize(), tlsf2.getTelemetry().getTLSFSize());
//     EXPECT_EQ(telemetry.getMinUsage(), tlsf2.getTelemetry().getMinUsage());
//     EXPECT_EQ(telemetry.getTotalPadding(), tlsf2.getTelemetry().getTotalPadding());
// }
//
//
// TEST_F(InternallyManagedTLSFTests, MoveAssign_CopiesAttributesToNewObject)
// {
//     constexpr auto sampleAllocation = 50;
//     static_cast<void>(tlsf.malloc(sampleAllocation));
//     pmm::TLSF<> tlsf2(256);
//
//     tlsf2 = std::move(tlsf);
//     EXPECT_EQ(tlsfSize - sampleAllocation, tlsf2.freeSize());
//     EXPECT_EQ(tlsfSize, tlsf2.size());
//     EXPECT_EQ(sampleAllocation, tlsf2.usedSize());
// }
//
//
// TEST_F(InternallyManagedTLSFTests, MoveAssign_MovesTelemetry)
// {
//     static_cast<void>(tlsf.malloc(120));
//     static_cast<void>(tlsf.malloc(240));
//     // Get the telemetry to ensure that the value is preserved when moving
//     // DON'T get by reference as it will change internally
//     const auto telemetry = tlsf.getTelemetry();
//
//     pmm::TLSF<> tlsf2(256);
//     tlsf2 = std::move(tlsf);
//
//     // Checking for telemetry equality
//     EXPECT_EQ(telemetry.getUsedSize(), tlsf2.getTelemetry().getUsedSize());
//     EXPECT_EQ(telemetry.getPeakUsage(), tlsf2.getTelemetry().getPeakUsage());
//     EXPECT_EQ(telemetry.getTLSFSize(), tlsf2.getTelemetry().getTLSFSize());
//     EXPECT_EQ(telemetry.getMinUsage(), tlsf2.getTelemetry().getMinUsage());
//     EXPECT_EQ(telemetry.getTotalPadding(), tlsf2.getTelemetry().getTotalPadding());
// }



/**************************************
 *            ALLOC BYTES             *
 **************************************/

/**
 * @test Verify that malloc returns an address aligned to sizeof(void*) bytes
 *       given no alignment was passed-in.
 */
TEST_F(InternallyManagedTLSFTests, Malloc_Returns8ByteAlignedAddressByDefault)
{
    // Misalign bytes to 2
    [[maybe_unused]] void* misalignedBytes = tlsf.malloc(2, 2);

    void* bytes = tlsf.malloc(8);

    const auto address = reinterpret_cast<uintptr_t>(bytes);
    EXPECT_EQ(0, address % sizeof(void*));
}


TEST_F(InternallyManagedTLSFTests, Malloc_ReturnsProvidedByteAlignedAddress)
{
    constexpr auto byteAlignment = 32;
    void* bytes                  = tlsf.malloc(128, byteAlignment);

    const auto address = reinterpret_cast<uintptr_t>(bytes);
    EXPECT_EQ(0, address % byteAlignment);
}


TEST_F(InternallyManagedTLSFTests, Malloc_ReturnsNonNullPtrWhenAllocatingMemoryLessThanTLSFSize)
{
    void* bytes = tlsf.malloc(256);

    EXPECT_NE(nullptr, bytes);
}


TEST_F(InternallyManagedTLSFTests, Malloc_ReturnsNonNullPtrWhenAllocatingMemoryEqualTLSFSize)
{

    // 15 bytes used for worst case alignment, 16-bytes for header, and 4 bytes for offset.
    void* bytes = tlsf.malloc(tlsfSize - 64);

    EXPECT_NE(nullptr, bytes);
}


TEST_F(InternallyManagedTLSFTests, Malloc_SubsequentAllocationDoNotCorruptMemory)
{
    constexpr auto bufferLength = 8;
    // Given two contiguous block of memory allocated back to back
    const auto firstAlloc = static_cast<int*>(tlsf.malloc(bufferLength * sizeof(int)));
    for (std::size_t i = 0; i < bufferLength; ++i)
    {
        firstAlloc[i] = static_cast<int>(i + 5);
    }

    const auto secondAlloc = static_cast<int*>(tlsf.malloc(bufferLength * sizeof(int)));
    for (std::size_t i = 0; i < bufferLength; ++i)
    {
        secondAlloc[i] = static_cast<int>(i + 7);
    }

    // When read back there is no corruption
    for (std::size_t i = 0; i < bufferLength; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 5), firstAlloc[i]);
        EXPECT_EQ(static_cast<int>(i + 7), secondAlloc[i]);
    }
}

// TODO: Add more TLSF allocation tests.

// TEST_F(InternallyManagedTLSFTests, Malloc_UpdatesTelemetry)
// {
//     constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;
//
//     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
//     static_cast<void>(tlsf.malloc(byte1));
//     static_cast<void>(tlsf.malloc(byte2));
//     static_cast<void>(tlsf.malloc(byte3));
//
//     constexpr std::size_t expectedMinUsage  = byte1;
//     constexpr std::size_t expectedPeakUsage = byte3;
//     constexpr std::size_t expectedUsage     = byte1 + byte2 + byte3;
//
//     EXPECT_EQ(expectedMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(expectedPeakUsage, tlsf.getTelemetry().getPeakUsage());
//     EXPECT_EQ(expectedUsage, tlsf.getTelemetry().getUsedSize());
// }


TEST_F(InternallyManagedTLSFTests, Malloc_HeaderIsPreservedInAddressBeforeGivenAddress)
{
    // NOTE: This tests works on the premise that the allocated memory follows a
    // [Header][Padding][OffsetToHeader][Ptr given to user] pattern
    // and the OffsetToHeader is not itself corrupted.
    constexpr auto allocSize = 64;
    auto bytes               = static_cast<uint8_t*>(tlsf.malloc(allocSize));
    using Offset_t           = pmm::TLSF<>::HeaderOffset_t;
    using Header_t           = pmm::TLSF<>::Header;

    const auto offset = reinterpret_cast<Offset_t*>(bytes - sizeof(Offset_t));
    const auto header = reinterpret_cast<Header_t*>(bytes - *offset);

    const auto expectedSize = *offset + allocSize;
    // Padding equals the size left in the in offset after subtracting size of Header and HeaderOffset
    const auto expectedPadding = *offset - (sizeof(Offset_t) + sizeof(Header_t));
    EXPECT_EQ(expectedSize, header->getSize());
    EXPECT_EQ(expectedPadding, header->padding);
}

/// @test Verify that free marks the internal buffer as free.
/// @note While we can't directly test this, we can allocate a near full size
///       allocation and requesting a larger allocation after free shouldn't trigger
///       an out-of-memory exception.
TEST_F(InternallyManagedTLSFTests, MFree_FreeTheBuffer)
{
    // TODO: This test can be used for checking if allocations smaller than min chunk size
    //       cleaves the memory.
    // Leeway to ensure the allocation passes.
    constexpr auto leeway = 32;
    const auto firstMem   = tlsf.malloc(tlsfSize - leeway);
    tlsf.mfree(firstMem);
    // If the allocation fails this will trigger an exception in DEBUG
    // and its UB in Release Mode(without SafeMode)
    const auto secondMem = tlsf.malloc(tlsfSize - leeway);
    EXPECT_NE(nullptr, secondMem);
}

/// @test Verify that free perform right only coalesce (latest allocations are freed in order).
///       AllocA, AllocB, AllocC, FreeB, FreeA
TEST_F(InternallyManagedTLSFTests, MFree_PerformsRightOnlyCoalesce)
{
    // Total Memory size is 2KB so this would around half the memory or more.
    constexpr size_t firstAllocSize{ 128_KB }, secondAllocSize{ 64_KB }, thirdAllocSize{ 255 };
    const auto firstAlloc                  = tlsf.malloc(firstAllocSize);
    const auto secondAlloc                 = tlsf.malloc(secondAllocSize);
    [[maybe_unused]] const auto thirdAlloc = tlsf.malloc(thirdAllocSize);

    tlsf.mfree(secondAlloc);
    tlsf.mfree(firstAlloc);

    // There should be two free blocks since we didn't free the middle block
    // so, we can allocate a buffer of size firstSize + secondSize and another that has the remainingSize
    // with leeway.
    // We need a larger leeway here to account for the fact that tlsf rounds up the size requirement to the next nearest
    // SL boundary and at nearly 2MB, it will be 16_KB((2MB - 1MB) / (2^6)) where 6 is the L value; 16_KB.
    const auto fourthAlloc = tlsf.malloc(tlsfSize - (firstAllocSize + secondAllocSize + thirdAllocSize + 16_KB));
    const auto fifthAlloc  = tlsf.malloc(firstAllocSize + secondAllocSize - 32);
    EXPECT_NE(nullptr, fourthAlloc);
    EXPECT_NE(nullptr, fifthAlloc);
}


/// @test Verify that free perform left-only coalesce (first allocations are freed in order).
///       AllocA, AllocB, AllocC, FreeA, FreeB.
TEST_F(InternallyManagedTLSFTests, MFree_PerformsLeftOnlyCoalesce)
{
    // Total Memory size is 2KB so this would around half the memory or more.
    constexpr size_t firstAllocSize{ 128_KB }, secondAllocSize{ 64_KB }, thirdAllocSize{ 255 };
    const auto firstAlloc                  = tlsf.malloc(firstAllocSize);
    const auto secondAlloc                 = tlsf.malloc(secondAllocSize);
    [[maybe_unused]] const auto thirdAlloc = tlsf.malloc(thirdAllocSize);

    tlsf.mfree(firstAlloc);
    tlsf.mfree(secondAlloc);

    // There should be two free blocks since we didn't free the middle block
    // so, we can allocate a buffer of size firstSize + secondSize and another that has the remainingSize
    // with leeway.
    // We need a larger leeway here to account for the fact that tlsf rounds up the size requirement to the next nearest
    // SL boundary and at nearly 2MB, it will be 16_KB((2MB - 1MB) / (2^6)) where 6 is the L value; 16_KB.
    const auto fourthAlloc = tlsf.malloc(tlsfSize - (firstAllocSize + secondAllocSize + thirdAllocSize + 16_KB));
    const auto fifthAlloc  = tlsf.malloc(firstAllocSize + secondAllocSize - 32);
    EXPECT_NE(nullptr, fourthAlloc);
    EXPECT_NE(nullptr, fifthAlloc);
}


/// @test Verify that free perform right only coalesce (allocations freed in a mixed order).
///       AllocA, AllocB, AllocC, FreeC, FreeA, FreeB.
TEST_F(InternallyManagedTLSFTests, MFree_PerformsMixedCoalesce)
{
    // Total Memory size is 2KB so this would around half the memory or more.
    constexpr auto firstAllocSize{ 512 }, secondAllocSize{ 128 }, thirdAllocSize{ 255 };
    const auto firstAlloc  = tlsf.malloc(firstAllocSize);
    const auto secondAlloc = tlsf.malloc(secondAllocSize);
    const auto thirdAlloc  = tlsf.malloc(thirdAllocSize);

    tlsf.mfree(thirdAlloc);
    tlsf.mfree(firstAlloc);
    tlsf.mfree(secondAlloc);

    // Here 128 is leeway
    const auto fourthAlloc = tlsf.malloc(tlsfSize - 64);
    EXPECT_NE(nullptr, fourthAlloc);
}


TEST_F(InternallyManagedTLSFTests, MFree_PerformRightCoalesceWithMultipleAllocations)
{
    std::vector<void*> allocations;
    constexpr auto leeway            = 32;
    constexpr auto perAllocationSize = 2_KB;
    const auto numAllocations        = tlsfSize / (perAllocationSize + leeway);
    // Allocate memory
    for (size_t i = 0; i < numAllocations; ++i)
    {
        allocations.push_back(tlsf.malloc(perAllocationSize));
    }

    // Free memory
    // Note size_t can wrap around when hitting --1, so we can internally use zero index.
    for (size_t i = numAllocations; i > 0; --i)
    {
        tlsf.mfree(allocations[i - 1]);
    }

    // Try allocating a new full size allocation
    const auto finalAllocation = tlsf.malloc(tlsfSize - leeway);
    EXPECT_NE(nullptr, finalAllocation);
}


TEST_F(InternallyManagedTLSFTests, MFree_PerformLeftCoalesceWithMultipleAllocations)
{
    std::vector<void*> allocations;
    constexpr auto leeway            = 32;
    constexpr auto perAllocationSize = 2_KB;
    const auto numAllocations        = tlsfSize / (perAllocationSize + leeway);
    // Allocate memory
    for (size_t i = 0; i < numAllocations; ++i)
    {
        allocations.push_back(tlsf.malloc(perAllocationSize));
    }

    // Free memory
    // Note size_t can wrap around when hitting --1, so we can internally use zero index.
    for (size_t i = numAllocations; i > 0; --i)
    {
        tlsf.mfree(allocations[i - 1]);
    }

    // Try allocating a new full size allocation
    const auto finalAllocation = tlsf.malloc(tlsfSize - leeway);
    EXPECT_NE(nullptr, finalAllocation);
}


TEST_F(InternallyManagedTLSFTests, MFree_PerformCoalesceWithMixedIntermittentFrees)
{
    std::vector<void*> allocations;
    constexpr auto leeway            = 32;
    constexpr auto perAllocationSize = 2_KB;
    const auto numAllocations        = tlsfSize / (perAllocationSize + leeway);
    // Allocate memory
    for (size_t i = 0; i < numAllocations; ++i)
    {
        allocations.push_back(tlsf.malloc(perAllocationSize));
    }

    // Free memory
    // Note size_t can wrap around when hitting --1, so we can internally use zero index.
    for (size_t i = 0; i < numAllocations; i += 2) // Free even indexed allocations
    {
        tlsf.mfree(allocations[i]);
    }
    for (size_t i = 1; i < numAllocations; i += 2) // Free odd indexed allocations
    {
        tlsf.mfree(allocations[i]);
    }

    // Try allocating a new full size allocation
    const auto finalAllocation = tlsf.malloc(tlsfSize - leeway);
    EXPECT_NE(nullptr, finalAllocation);
}


TEST_F(InternallyManagedTLSFTests, Free_CallsClassDestructorForNonTrivialTypes)
{
    int numDestructorCalls = 0;
    const auto nonTrivial  = tlsf.alloc<DestructionTracker>(&numDestructorCalls);

    tlsf.free(nonTrivial);
    EXPECT_EQ(1, numDestructorCalls);
}


TEST_F(InternallyManagedTLSFTests, Free_FreesMemoryForNewAllocations)
{
    const auto firstAlloc = tlsf.alloc<LargeData<tlsfSize - 1_KB>>();
    tlsf.free(firstAlloc);

    // This trigger assertion in debug if the memory is not freed
    const auto newAlloc = tlsf.alloc<LargeData<tlsfSize - 1_KB>>();
    EXPECT_NE(nullptr, newAlloc);
}




TEST_F(InternallyManagedTLSFTests, Resize_SameSizeReturnsSameAddress)
{
    constexpr auto oldSize = 1_KB, newSize = 1_KB;
    // Note: While the test uses two variables for holding old and resized memory address, it is not
    //       recommended for production use since that can lead to dangling pointers and memory corruptions
    //       (if data is written to it).
    const auto mem     = tlsf.malloc(oldSize);
    const auto resized = tlsf.resize(mem, oldSize, newSize);

    EXPECT_NE(nullptr, resized);
    EXPECT_EQ(mem, resized);
}


/// @test Verify that when resizing a buffer to a smaller size with the size difference
///       smaller than split threshold, returns the same memory address.
TEST_F(InternallyManagedTLSFTests, Resize_ToSmallerSize_SizeDiffSmallerThanSplitThreshold_ReturnsSameAddress)
{
    constexpr auto oldSize = 1_KB, newSize = 1_KB - (pmm::TLSF<>::SPLIT_SIZE_THRESHOLD - 1);
    // Note: While the test uses two variables for holding old and resized memory address, it is not
    //       recommended for production use since that can lead to dangling pointers and memory corruptions
    //       (if data is written to it).
    const auto mem     = tlsf.malloc(oldSize);
    const auto resized = tlsf.resize(mem, oldSize, newSize);

    EXPECT_NE(nullptr, resized);
    EXPECT_EQ(mem, resized);
}


/// @test Verify that when resizing a buffer to a smaller size with the size difference
///       equalling split threshold, returns the same memory address.
TEST_F(InternallyManagedTLSFTests, Resize_ToSmallerSize_SizeDiffEqualToSplitThreshold_ReturnsSameAddress)
{
    constexpr auto oldSize = 1_KB, newSize = 1_KB - (pmm::TLSF<>::SPLIT_SIZE_THRESHOLD);
    // Note: While the test uses two variables for holding old and resized memory address, it is not
    //       recommended for production use since that can lead to dangling pointers and memory corruptions
    //       (if data is written to it).

    const auto mem     = tlsf.malloc(oldSize);
    const auto resized = tlsf.resize(mem, oldSize, newSize);

    EXPECT_NE(nullptr, resized);
    EXPECT_EQ(mem, resized);
}


/// @test Verify that when resizing a buffer to a smaller size with the size difference
///       greater than split threshold, returns the same memory address.
TEST_F(InternallyManagedTLSFTests, Resize_ToSmallerSize_SizeDiffGreaterThanSplitThreshold_ReturnsSameAddress)
{
    constexpr auto oldSize = 1_KB, newSize = 1_KB - (pmm::TLSF<>::SPLIT_SIZE_THRESHOLD + 1);
    // Note: While the test uses two variables for holding old and resized memory address, it is not
    //       recommended for production use since that can lead to dangling pointers and memory corruptions
    //       (if data is written to it).

    const auto mem     = tlsf.malloc(oldSize);
    const auto resized = tlsf.resize(mem, oldSize, newSize);

    EXPECT_NE(nullptr, resized);
    EXPECT_EQ(mem, resized);
}


/// @test Verify that when resizing a buffer to a larger size returns a new memory address.
TEST_F(InternallyManagedTLSFTests, Resize_LargerSizeSizeReturnsNewAddress)
{
    constexpr auto oldSize = 1_KB, newSize = 5_KB;
    // Note: While the test uses two variables for holding old and resized memory address, it is not
    //       recommended for production use since that can lead to dangling pointers and memory corruptions
    //       (if data is written to it).

    const auto mem     = tlsf.malloc(oldSize);
    const auto resized = tlsf.resize(mem, oldSize, newSize);

    EXPECT_NE(nullptr, resized);
    EXPECT_NE(mem, resized);
}

TEST_F(InternallyManagedTLSFTests, Resize_LargerSizeSizeCopiesContentFromOldMemory)
{
    constexpr auto oldSize = 1_KB, newSize = 5_KB;
    constexpr auto elementCount = oldSize / sizeof(int);
    // Note: While the test uses two variables for holding old and resized memory address, it is not
    //       recommended for production use since that can lead to dangling pointers and memory corruptions
    //       (if data is written to it).
    const auto mem = static_cast<int*>(tlsf.malloc(oldSize));
    for (size_t i = 0; i < elementCount; ++i)
    {
        mem[i] = static_cast<int>(i + 13);
    }

    const auto resized = static_cast<int*>(tlsf.resize(mem, oldSize, newSize));
    for (size_t i = 0; i < elementCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 13), resized[i]);
    }
}


/// @test Verify that clear clears the TLSF allowing for new allocations.
TEST_F(InternallyManagedTLSFTests, Clear_ClearsTLSFAllowingForNewAllocations)
{
    // Make some allocations and free
    [[maybe_unused]] const auto mem1 = tlsf.malloc(1_KB);
    [[maybe_unused]] const auto mem2 = tlsf.malloc(11_KB);
    tlsf.mfree(mem1);
    [[maybe_unused]] const auto mem3 = tlsf.malloc(15_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(2_KB);
    [[maybe_unused]] const auto mem5 = tlsf.malloc(2_KB);
    tlsf.mfree(mem4);

    // Clear tlsf
    tlsf.clear();

    // Make an allocation near the buffer size
    const auto newMem = tlsf.malloc(tlsfSize - 32);

    EXPECT_NE(nullptr, newMem);
}


/**************************************
 *              ALLOC                 *
 **************************************/

TEST_F(InternallyManagedTLSFTests, Alloc_AllocatesAnObjectInTheTLSF)
{
    const auto vec = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_FLOAT_EQ(1.0f, vec->x);
    EXPECT_FLOAT_EQ(2.0f, vec->y);
    EXPECT_FLOAT_EQ(3.0f, vec->z);
    EXPECT_FLOAT_EQ(4.0f, vec->w);
}


TEST_F(InternallyManagedTLSFTests, Alloc_AlignsToTargetAlignment)
{
    // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    static_cast<void>(tlsf.malloc(2, 2));

    constexpr auto expectedAlignment = alignof(Vec4);
    [[maybe_unused]] const auto vec  = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vec) % expectedAlignment);
}


// TEST_F(InternallyManagedTLSFTests, Alloc_UpdatesTelemetry)
// {
//     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
//     static_cast<void>(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
//     static_cast<void>(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
//     static_cast<void>(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
//     static_cast<void>(tlsf.alloc<int>(1));
//
//     constexpr std::size_t expectedMinUsage  = sizeof(int);
//     constexpr std::size_t expectedPeakUsage = sizeof(Vec4);
//     constexpr std::size_t expectedUsage     = sizeof(Vec4) * 3 + sizeof(int);
//
//     EXPECT_EQ(expectedMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(expectedPeakUsage, tlsf.getTelemetry().getPeakUsage());
//     EXPECT_EQ(expectedUsage, tlsf.getTelemetry().getUsedSize());
// }



/**************************************
 *            ALLOC V(ector)           *
 **************************************/

TEST_F(InternallyManagedTLSFTests, AllocV_ReturnsAContinguousBlockOfMemory)
{
    constexpr auto blockCount = 10;
    const auto vertices       = tlsf.allocV<Vec4>(blockCount);

    EXPECT_EQ(blockCount, vertices.size());
    EXPECT_EQ(blockCount * sizeof(Vec4), vertices.size_bytes());
}


TEST_F(InternallyManagedTLSFTests, AllocV_SubsequentAllocationDoNotCorruptMemory)
{
    constexpr auto blockCount       = 5;
    constexpr std::array vertexData = {
        1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,  9.0f,  10.0f,
        11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f,
    };
    constexpr std::array edgeData = {
        101.0f, 102.0f, 103.0f,  104.0f, 105.0f, 106.0f, 107.0f,  108.0f, 109.0f, 110.0f,
        111.0f, 112.0f, 1013.0f, 114.0f, 115.0f, 116.0f, 1017.0f, 118.0f, 119.0f, 120.0f,
    };

    auto vertices = tlsf.allocV<Vec4>(blockCount);
    auto edges    = tlsf.allocV<Vec4>(blockCount);

    // Write into the first allocated span
    for (std::size_t i = 0; i < blockCount; ++i)
    {
        vertices[i] = Vec4{ vertexData[i * 4], vertexData[i * 4 + 1], vertexData[i * 4 + 2], vertexData[i * 4 + 3] };
    }

    // Write into the second allocated span
    for (std::size_t i = 0; i < blockCount; ++i)
    {
        edges[i] = Vec4{ edgeData[i * 4], edgeData[i * 4 + 1], edgeData[i * 4 + 2], edgeData[i * 4 + 3] };
    }


    // Verify data integrity is maintained for both
    for (std::size_t i = 0; i < blockCount; ++i)
    {
        constexpr auto epsilon = 1e-5;
        const auto vert        = vertices[i];
        EXPECT_NEAR(vertexData[i * 4], vert.x, epsilon);
        EXPECT_NEAR(vertexData[i * 4 + 1], vert.y, epsilon);
        EXPECT_NEAR(vertexData[i * 4 + 2], vert.z, epsilon);
        EXPECT_NEAR(vertexData[i * 4 + 3], vert.w, epsilon);

        const auto edge = edges[i];
        EXPECT_NEAR(edgeData[i * 4], edge.x, epsilon);
        EXPECT_NEAR(edgeData[i * 4 + 1], edge.y, epsilon);
        EXPECT_NEAR(edgeData[i * 4 + 2], edge.z, epsilon);
        EXPECT_NEAR(edgeData[i * 4 + 3], edge.w, epsilon);
    }
}


// TEST_F(InternallyManagedTLSFTests, AllocV_UpdatesTelemetry)
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
{

    // NOTE: For CTOR tests we are using the fixture allocated tlsf.
    TEST_F(InternallyManagedTLSFTests, Ctor_CreatesValidFLAndSLBitmaps)
    {
        // Get the FL and SL bitmaps corresponding to our size.
        const auto [flIndex, slIndex] = tlsf.mappingInsert(tlsfSize);
        const auto expectedFLBitmap   = 1ULL << flIndex;
        const auto expectedSLBitmap   = 1ULL << slIndex;

        const auto flBitmap = tlsf._flBitmap;
        const auto slBitmap = tlsf._slBitmap[flIndex];
        EXPECT_EQ(expectedFLBitmap, flBitmap);
        EXPECT_EQ(expectedSLBitmap, slBitmap);
    }


    TEST_F(InternallyManagedTLSFTests, Ctor_SingleAllocation_FLBitmapIsSingleBit)
    { EXPECT_TRUE(std::has_single_bit(tlsf._flBitmap)); }


    TEST_F(InternallyManagedTLSFTests, Ctor_SingleAllocation_SLBitmapHasOnlyOneNonZeroEntry)
    {
        size_t nonZeroEntry{ 0 };

        for (const auto slBitmap : tlsf._slBitmap)
        {
            if (slBitmap != 0)
            {
                ++nonZeroEntry;
            }
        }

        EXPECT_EQ(1, nonZeroEntry);
    }


    TEST_F(InternallyManagedTLSFTests, Ctor_SingleAllocation_OnlySingleFreeListIsPopulated)
    {
        size_t nonNullFLCount{}, nonNullSLCount{};

        for (size_t i = 0; i < tlsf.FL_SIZE; ++i)
        {
            for (size_t j = 0; j < tlsf.SL_SIZE; ++j)
            {
                if (tlsf._freeList[i][j] != nullptr)
                {
                    nonNullFLCount++;
                    nonNullSLCount++;
                }
            }
        }

        EXPECT_EQ(1, nonNullFLCount);
        EXPECT_EQ(1, nonNullSLCount);
    }

    TEST_F(InternallyManagedTLSFTests, Ctor_WritesAppropriateHeaderToBuffer)
    {
        TLSF<>::TLSFFreeNode* freeNode;
        // While we can directly query the buffer(_buffer member variable), it is better to iterate and get the buffer
        // since a) there is only one TLSFFreeNode that is non-null and b) _buffer internal variable may get removed
        // due to its redundancy.
        for (const auto& flList : tlsf._freeList)
        {
            for (auto& slList : flList)
            {
                if (slList != nullptr)
                {
                    freeNode = slList;
                }
            }
        }

        ASSERT_NE(nullptr, freeNode);
        const auto header = TLSF<>::getHeader(freeNode);

        EXPECT_EQ(tlsfSize, header->getSize());
        EXPECT_TRUE(header->isFree());
    }


    TEST_F(InternallyManagedTLSFTests, MoveCtor_ClearsMovedTLSFsInternalBuffer)
    {
        [[maybe_unused]] const TLSF<pmm::MemPolicy::Internal> tlsf2 = std::move(tlsf);
        // NOLINT(bugprone-use-after-move)
        EXPECT_EQ(nullptr, tlsf._buffer);
    }


    TEST_F(InternallyManagedTLSFTests, MoveCtor_MovesBufferIntoNewObject)
    {
        const auto initialPointer  = tlsf._buffer;
        const auto initialUsedSize = tlsf._usedSize;
        const auto initialSize     = tlsf._size;
        const auto initialFLMask   = tlsf._flBitmap;
        const auto initialSLMask   = tlsf._slBitmap;

        const TLSF<pmm::MemPolicy::Internal> tlsf2 = std::move(tlsf);
        EXPECT_EQ(initialPointer, tlsf2._buffer);
        EXPECT_EQ(initialUsedSize, tlsf2._usedSize);
        EXPECT_EQ(initialSize, tlsf2._size);
        EXPECT_EQ(initialFLMask, tlsf2._flBitmap);
        EXPECT_EQ(initialSLMask, tlsf2._slBitmap);
    }


    TEST_F(InternallyManagedTLSFTests, MoveAssign_ClearsMovedTLSF)
    {
        [[maybe_unused]] TLSF<pmm::MemPolicy::Internal> tlsf2(256);

        static_cast<void>(tlsf2 = std::move(tlsf));
        EXPECT_EQ(nullptr, tlsf._buffer);
    }


    TEST_F(InternallyManagedTLSFTests, MoveAssign_MovesBufferIntoNewObject)
    {
        const auto initialPointer  = tlsf._buffer;
        const auto initialUsedSize = tlsf._usedSize;
        const auto initialSize     = tlsf._size;
        const auto initialFLMask   = tlsf._flBitmap;
        const auto initialSLMask   = tlsf._slBitmap;
        TLSF<pmm::MemPolicy::Internal> tlsf2(256);

        tlsf2 = std::move(tlsf);

        EXPECT_EQ(initialPointer, tlsf2._buffer);
        EXPECT_EQ(initialPointer, tlsf2._buffer);
        EXPECT_EQ(initialUsedSize, tlsf2._usedSize);
        EXPECT_EQ(initialSize, tlsf2._size);
        EXPECT_EQ(initialFLMask, tlsf2._flBitmap);
        EXPECT_EQ(initialSLMask, tlsf2._slBitmap);
    }


    TEST_F(InternallyManagedTLSFTests, MoveAssign_SelfAssignmentReturnsTheSameTLSF)
    {
        const auto initialAddress = reinterpret_cast<uintptr_t>(tlsf._buffer);
        const auto initialFLMask  = tlsf._flBitmap;
        const auto initialSLMask  = tlsf._slBitmap;
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wself-move"
#endif
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-move"
#endif
        tlsf = std::move(tlsf);
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

        EXPECT_EQ(initialAddress, reinterpret_cast<uintptr_t>(tlsf._buffer));
        EXPECT_EQ(initialFLMask, tlsf._flBitmap);
        EXPECT_EQ(initialSLMask, tlsf._slBitmap);
    }


    TEST_F(InternallyManagedTLSFTests, MoveAssign_DeletingOriginalTLSFDoNotDeleteTheNewTLSFsMemory)
    {
        TLSF<pmm::MemPolicy::Internal> tlsf2(256);
        constexpr auto scopedTLSFSize = 512;

        // The tlsf being moved is scoped
        {
            TLSF<pmm::MemPolicy::Internal> scopedTLSF(scopedTLSFSize);
            tlsf2 = std::move(scopedTLSF);
        }
        EXPECT_NE(nullptr, tlsf2._buffer);

        // Write arbitrary data into the buffer
        // NOTE: i % 255 ensures that uint8_t does not overflow
        for (uint32_t i = 0; i < scopedTLSFSize; ++i)
        {
            tlsf2._buffer[i] = i % 255;
        }

        // Read the value from buffer
        for (uint32_t i = 0; i < scopedTLSFSize / 4; i += 4)
        {
            EXPECT_EQ(i % 255, tlsf2._buffer[i]);
        }
    }

    TEST_P(InternallyManagedTLSF_MappingInsertTests, ReturnsValidFLAndSLIndices)
    {
        const auto [size, expectedFl, expectedSl] = GetParam();
        const auto [fl, sl]                       = TLSF<>::mappingInsert(size);
        EXPECT_EQ(expectedFl, fl);
        EXPECT_EQ(expectedSl, sl);
    }



    TEST_P(InternallyManagedTLSF_MappingSearchTests, ReturnsValidFLAndSLIndices)
    {
        const auto [size, expectedFl, expectedSl] = GetParam();
        const auto [fl, sl]                       = TLSF<>::mappingSearch(size);
        EXPECT_EQ(expectedFl, fl);
        EXPECT_EQ(expectedSl, sl);
    }

    // TODO: Correct header is created(malloc back navigation)

    TEST_F(InternallyManagedTLSFTests, Malloc_NullsOutInitialBitmap)
    {
        // Store the initial FL bitmap
        const auto initialBitmap = tlsf._flBitmap;
        // Make an allocation
        static_cast<void>(tlsf.malloc(32));
        // Get the new bitmap
        const auto newBitmap = tlsf._flBitmap;

        // Ensure both are not equal
        EXPECT_NE(initialBitmap, newBitmap);

        // And together both bitmaps, if they don't have any equal bits
        // result should be zero
        // 0010 0000 & 1000 0000 = 0000 0000 (PASS)
        // 1010 0000 & 1000 0000 = 1000 0000 (FAIL)
        EXPECT_EQ(0, initialBitmap & newBitmap);
    }


    TEST_F(InternallyManagedTLSFTests, Malloc_SingleAllocation_FLBitmapIsSingleBit)
    {
        static_cast<void>(tlsf.malloc(32));
        EXPECT_TRUE(std::has_single_bit(tlsf._flBitmap));
    }


    TEST_F(InternallyManagedTLSFTests, Malloc_SingleAllocation_SLBitmapHasOnlyOneNonZeroEntry)
    {
        static_cast<void>(tlsf.malloc(32));
        size_t nonZeroEntries = 0;

        for (const auto mask : tlsf._slBitmap)
        {
            if (mask != 0)
            {
                nonZeroEntries++;
            }
        }

        EXPECT_EQ(1, nonZeroEntries);
    }


    TEST_F(InternallyManagedTLSFTests, Malloc_SingleAllocation_OnlySingleFreeListIsPopulated)
    {
        static_cast<void>(tlsf.malloc(32));
        size_t nonNullEntries = 0;

        for (const auto& list : tlsf._freeList)
        {
            for (const auto& entry : list)
            {
                if (entry != nullptr)
                {
                    nonNullEntries++;
                }
            }
        }

        EXPECT_EQ(1, nonNullEntries);
    }



    TEST_F(InternallyManagedTLSFTests, Malloc_SingleAllocation_FreeListIsUpdatedAfterAllocation)
    {
        // To check where the FL and SL entries in the free is updated
        // we can make 1 allocation and ensure that the entries are updated
        // from the initial location to the new location.
        size_t initialNonNullFL, initialNonNullSL, updatedNonNullFL, updatedNonNullSL;

        // Gather initial indices
        for (size_t i = 0; i < tlsf.FL_SIZE; ++i)
        {
            for (size_t j = 0; j < tlsf.SL_SIZE; ++j)
            {
                if (tlsf._freeList[i][j] != nullptr)
                {
                    initialNonNullFL = i;
                    initialNonNullSL = j;
                }
            }
        }
        // Make an allocation
        static_cast<void>(tlsf.malloc(tlsfSize / 2));
        // Gather indices prior to allocation
        for (size_t i = 0; i < tlsf.FL_SIZE; ++i)
        {
            for (size_t j = 0; j < tlsf.SL_SIZE; ++j)
            {
                if (tlsf._freeList[i][j] != nullptr)
                {
                    updatedNonNullFL = i;
                    updatedNonNullSL = j;
                }
            }
        }
        // Check if they are unequal
        EXPECT_NE(initialNonNullFL, updatedNonNullFL);
        EXPECT_NE(initialNonNullSL, updatedNonNullSL);

        // We can also verify that the updated FL is always less than the initial one
        // since are using nearly half the 2_KB space, but this may fail if updated with
        // new parameters.
        EXPECT_GT(initialNonNullFL, updatedNonNullFL);
    }


    /// @test Verify that malloc write cleaves the remaining buffer and writes appropriate
    ///       header after allocation.
    TEST_F(InternallyManagedTLSFTests, Malloc_WritesAppropriateHeaderToBuffer_AfterFirstAllocation)
    {
        using Header_t             = TLSF<>::Header;
        using Offset_t             = TLSF<>::HeaderOffset_t;
        constexpr size_t allocSize = 128;

        // Allocate some buffer and query the header for its size by walking backwards
        // with the memory address
        const auto bytes  = static_cast<uint8_t*>(tlsf.malloc(allocSize));
        const auto offset = reinterpret_cast<Offset_t*>(bytes - sizeof(Offset_t));
        auto allocHeader  = reinterpret_cast<Header_t*>(bytes - *offset);

        // Get the internal free node by iterating the freelist
        // Invariant: After first allocation there should only be 1 buffer in freelist.
        TLSF<>::TLSFFreeNode* freeNode;
        for (const auto& flList : tlsf._freeList)
        {
            for (auto& slList : flList)
            {
                if (slList != nullptr)
                {
                    freeNode = slList;
                }
            }
        }

        ASSERT_NE(nullptr, freeNode);
        const auto freeHeader = TLSF<>::getHeader(freeNode);

        // Then the size of the header must be totalsize - allocatedSize(this will not be actual size
        // requested by the user, due to padding and metadata requirements).
        EXPECT_EQ(tlsfSize - allocHeader->getSize(), freeHeader->getSize());
        EXPECT_TRUE(freeHeader->isFree());
    }


    /// @test Verify that when trying to resizing to the same size, FL and SL bitmasks doesn't update.
    TEST_F(InternallyManagedTLSFTests, Resize_SameSizeDoesNotUpdateFLAndSLBitmaps)
    {
        constexpr auto oldSize = 1_KB, newSize = 1_KB;
        // Note: While the test uses two variables for holding old and resized memory address, it is not
        //       recommended for production use since that can lead to dangling pointers and memory corruptions
        //       (if data is written to it).
        // Allocate initial buffer
        const auto mem = tlsf.malloc(oldSize);
        // Get the FL and SL bitmasks
        const auto oldFL = tlsf._flBitmap;
        const auto oldSL = tlsf._slBitmap;
        // Resize the buffer
        [[maybe_unused]] const auto resized = tlsf.resize(mem, oldSize, newSize);

        EXPECT_EQ(oldFL, tlsf._flBitmap);
        EXPECT_EQ(oldSL, tlsf._slBitmap);
    }


    /// @test Verify that when resizing a buffer to a smaller size with the size difference
    ///       smaller than split threshold, does not update the fl and sl bitmasks.
    TEST_F(InternallyManagedTLSFTests,
           Resize_ToSmallerSize_SizeDiffSmallerThanSplitThreshold_DoesNotUpdateFLAndSLBitmaps)
    {
        constexpr auto oldSize = 1_KB, newSize = 1_KB - (TLSF<>::SPLIT_SIZE_THRESHOLD - 1);
        // Note: While the test uses two variables for holding old and resized memory address, it is not
        //       recommended for production use since that can lead to dangling pointers and memory corruptions
        //       (if data is written to it).

        // Allocate initial buffer
        const auto mem = tlsf.malloc(oldSize);
        // Get the FL and SL bitmasks
        const auto oldFL = tlsf._flBitmap;
        const auto oldSL = tlsf._slBitmap;
        // Resize the buffer
        [[maybe_unused]] const auto resized = tlsf.resize(mem, oldSize, newSize);

        EXPECT_EQ(oldFL, tlsf._flBitmap);
        EXPECT_EQ(oldSL, tlsf._slBitmap);
    }


    /// @test Verify that when resizing a buffer to a smaller size with the size difference
    ///       equalling split threshold, updates the fl and sl bitmasks.
    TEST_F(InternallyManagedTLSFTests, Resize_ToSmallerSize_SizeDiffEqualToSplitThreshold_UpdatesFLAndSLBitmaps)
    {
        constexpr auto oldSize = 1_KB, newSize = 1_KB - (TLSF<>::SPLIT_SIZE_THRESHOLD);
        // Note: While the test uses two variables for holding old and resized memory address, it is not
        //       recommended for production use since that can lead to dangling pointers and memory corruptions
        //       (if data is written to it).

        // Allocate initial buffer
        const auto mem = tlsf.malloc(oldSize);
        // Get the FL and SL bitmasks
        const auto oldFL = tlsf._flBitmap;
        const auto oldSL = tlsf._slBitmap;
        // Resize the buffer
        [[maybe_unused]] const auto resized = tlsf.resize(mem, oldSize, newSize);

        EXPECT_NE(oldFL, tlsf._flBitmap);
        EXPECT_NE(oldSL, tlsf._slBitmap);
    }


    /// @test Verify that when resizing a buffer to a smaller size with the size difference
    ///       greater than split threshold, updates the fl and sl bitmasks.
    TEST_F(InternallyManagedTLSFTests, Resize_ToSmallerSize_SizeDiffGreaterThanSplitThreshold_UpdatesFLAndSLBitmaps)
    {
        constexpr auto oldSize = 1_KB, newSize = 1_KB - (TLSF<>::SPLIT_SIZE_THRESHOLD + 1);
        // Note: While the test uses two variables for holding old and resized memory address, it is not
        //       recommended for production use since that can lead to dangling pointers and memory corruptions
        //       (if data is written to it).

        // Allocate initial buffer
        const auto mem = tlsf.malloc(oldSize);
        // Get the FL and SL bitmasks
        const auto oldFL = tlsf._flBitmap;
        const auto oldSL = tlsf._slBitmap;
        // Resize the buffer
        [[maybe_unused]] const auto resized = tlsf.resize(mem, oldSize, newSize);

        EXPECT_NE(oldFL, tlsf._flBitmap);
        EXPECT_NE(oldSL, tlsf._slBitmap);
    }

    /// @test Verify that when resizing a buffer to a larger size updates the fl and sl masks.
    TEST_F(InternallyManagedTLSFTests, Resize_LargerSizeSizeUpdatesFLAndSLBitmaps)
    {
        constexpr auto oldSize = 1_KB, newSize = 5_KB;
        // Note: While the test uses two variables for holding old and resized memory address, it is not
        //       recommended for production use since that can lead to dangling pointers and memory corruptions
        //       (if data is written to it).

        // Allocate initial buffer
        const auto mem = tlsf.malloc(oldSize);
        // Get the FL and SL bitmasks
        const auto oldFL = tlsf._flBitmap;
        const auto oldSL = tlsf._slBitmap;
        // Resize the buffer
        [[maybe_unused]] const auto resized = tlsf.resize(mem, oldSize, newSize);

        EXPECT_NE(oldFL, tlsf._flBitmap);
        EXPECT_NE(oldSL, tlsf._slBitmap);
    }


    TEST_F(InternallyManagedTLSFTests, Clear_ResetsFLAndSLBitmaps)
    {
        // Store the initial FL and SL bitmasks
        const auto oldFL = tlsf._flBitmap;
        const auto oldSL = tlsf._slBitmap;

        // Make some allocations and free
        [[maybe_unused]] const auto mem1 = tlsf.malloc(1_KB);
        [[maybe_unused]] const auto mem2 = tlsf.malloc(11_KB);
        tlsf.mfree(mem1);
        [[maybe_unused]] const auto mem3 = tlsf.malloc(15_KB);
        [[maybe_unused]] const auto mem4 = tlsf.malloc(2_KB);
        [[maybe_unused]] const auto mem5 = tlsf.malloc(2_KB);
        tlsf.mfree(mem4);

        // Clear tlsf
        tlsf.clear();

        EXPECT_EQ(oldFL, tlsf._flBitmap);
        EXPECT_EQ(oldSL, tlsf._slBitmap);
    }



    TEST_F(InternallyManagedTLSFTests, Clear_ResetsFreeList)
    {
        // Store initial state
        auto oldFreeListSize              = 0;
        auto newFreeListSize              = 0;
        TLSF<>::TLSFFreeNode* oldFreeNode = nullptr;
        TLSF<>::TLSFFreeNode* newFreeNode = nullptr;

        for (size_t i = 0; i < TLSF<>::FL_SIZE; ++i)
        {
            for (size_t j = 0; j < TLSF<>::SL_SIZE; ++j)
            {
                if (tlsf._freeList[i][j] != nullptr)
                {
                    ++oldFreeListSize;
                    oldFreeNode = tlsf._freeList[i][j];
                }
            }
        }

        // Make some allocations and free
        [[maybe_unused]] const auto mem1 = tlsf.malloc(1_KB);
        [[maybe_unused]] const auto mem2 = tlsf.malloc(11_KB);
        tlsf.mfree(mem1);
        [[maybe_unused]] const auto mem3 = tlsf.malloc(15_KB);
        [[maybe_unused]] const auto mem4 = tlsf.malloc(2_KB);
        [[maybe_unused]] const auto mem5 = tlsf.malloc(2_KB);
        tlsf.mfree(mem4);

        // Clear tlsf
        tlsf.clear();

        // Query the state again
        for (size_t i = 0; i < TLSF<>::FL_SIZE; ++i)
        {
            for (size_t j = 0; j < TLSF<>::SL_SIZE; ++j)
            {
                if (tlsf._freeList[i][j] != nullptr)
                {
                    ++newFreeListSize;
                    newFreeNode = tlsf._freeList[i][j];
                }
            }
        }

        EXPECT_EQ(1, newFreeListSize);
        EXPECT_EQ(oldFreeListSize, newFreeListSize);
        EXPECT_EQ(oldFreeNode, newFreeNode);
    }

    // TEST_F(InternallyManagedTLSFTests, Malloc_UpdatesTelemetryPadding)
    // {
    //     const auto buffer          = tlsf.malloc(128, 128);
    //     const auto expectedPadding = reinterpret_cast<uintptr_t>(buffer) - reinterpret_cast<uintptr_t>(tlsf._buffer);
    //
    //     EXPECT_EQ(expectedPadding, tlsf.getTelemetry().getTotalPadding());
    // }


    // TEST_F(InternallyManagedTLSFTests, Alloc_UpdatesTelemetryPadding)
    // {
    //     const auto vec4            = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    //     const auto expectedPadding = reinterpret_cast<uintptr_t>(vec4) - reinterpret_cast<uintptr_t>(tlsf._buffer);
    //
    //     EXPECT_EQ(expectedPadding, tlsf.getTelemetry().getTotalPadding());
    // }


    // TEST_F(InternallyManagedTLSFTests, AllocV_UpdatesTelemetryPadding)
    // {
    //     const auto data = tlsf.allocV<Vec4>(10);
    //     const auto expectedPadding =
    //         reinterpret_cast<uintptr_t>(data.data()) - reinterpret_cast<uintptr_t>(tlsf._buffer);
    //
    //     EXPECT_EQ(expectedPadding, tlsf.getTelemetry().getTotalPadding());
    // }


    // TEST_F(InternallyManagedTLSFTests, Clear_OnlyResetsCurrentTelemetryUsage)
    // {
    //     constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;
    //
    //     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    //     static_cast<void>(tlsf.malloc(byte1));
    //     static_cast<void>(tlsf.malloc(byte2));
    //     static_cast<void>(tlsf.malloc(byte3));
    //
    //     constexpr std::size_t expectedMinUsage  = byte1;
    //     constexpr std::size_t expectedPeakUsage = byte3;
    //
    //     tlsf.clear();
    //
    //     EXPECT_EQ(expectedMinUsage, tlsf.getTelemetry().getMinUsage());
    //     EXPECT_EQ(expectedPeakUsage, tlsf.getTelemetry().getPeakUsage());
    //     EXPECT_EQ(0, tlsf.getTelemetry().getUsedSize());
    //     EXPECT_EQ(0, tlsf.getTelemetry().getTotalPadding());
    // }


} // namespace pmm
