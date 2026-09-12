/**
 * @file ManagedTLSFTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 08, 2026
 *
 * @brief Verify managed tlsf allocation, free, and helper function logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


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
    class ManagedTLSFTests: public testing::Test
    {
    public:
        size_t tlsfSize{ 2_KB };
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
    class ManagedTLSF_MappingInsertTests: public testing::TestWithParam<TLSFMappingInsertParams>
    {};

    INSTANTIATE_TEST_SUITE_P(
        TLSF_InternalMappingTests, ManagedTLSF_MappingInsertTests,
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
    class ManagedTLSF_MappingSearchTests: public testing::TestWithParam<TLSFMappingInsertParams>
    {};

    INSTANTIATE_TEST_SUITE_P(TLSF_InternalMappingTests, ManagedTLSF_MappingSearchTests,
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
        static_assert(std::is_trivially_destructible_v<pmm::TLSF<pmm::ManagedMemory>> == false);
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
// TEST_F(ManagedTLSFTests, EnabledTelemetry_ReturnsRealTelemetry)
// {
//     [[maybe_unused]] pmm::TLSF<pmm::ManagedMemory, pmm::telemetry::Enabled> telemetryEnabledTLSF(512);
//     [[maybe_unused]] auto telemetry = telemetryEnabledTLSF.getTelemetry();
//     const bool result               = std::is_same_v<decltype(telemetry), pmm::TLSFTelemetry>;
//     EXPECT_TRUE(result);
// }
//
//
// TEST_F(ManagedTLSFTests, DisabledTelemetry_ReturnsDummyTelemetry)
// {
//     [[maybe_unused]] const pmm::TLSF<pmm::ManagedMemory, pmm::telemetry::Disabled> telemetryDisabledTLSF(512);
//     [[maybe_unused]] auto telemetry = telemetryDisabledTLSF.getTelemetry();
//     const bool result               = std::is_same_v<decltype(telemetry), pmm::DummyTLSFTelemetry>;
//     EXPECT_TRUE(result);
// }


TEST_F(ManagedTLSFTests, Ctor_InitializesTLSFWithTheGivenBytes) { EXPECT_EQ(tlsfSize, tlsf.size()); }


TEST_F(ManagedTLSFTests, TLSFHasZeroUsedSizeInitially) { EXPECT_EQ(0, tlsf.usedSize()); }


TEST_F(ManagedTLSFTests, TLSFHasFreeSpaceEqualToSizeInitially) { EXPECT_EQ(tlsfSize, tlsf.freeSize()); }


TEST_F(ManagedTLSFTests, MoveCtor_CopiesAttributesToNewObject)
{
    const pmm::TLSF<> tlsf2 = std::move(tlsf);
    EXPECT_EQ(tlsfSize, tlsf2.freeSize());
    EXPECT_EQ(tlsfSize, tlsf2.size());
    EXPECT_EQ(0, tlsf2.usedSize());
    // TODO: Add back after telemetry
    // EXPECT_EQ(tlsfSize, tlsf2.getTelemetry().getTLSFSize());
}
//
// TODO: Add back after adding telemetry and allocBytes
// TEST_F(ManagedTLSFTests, MoveCtor_MovesTelemetry)
// {
//     static_cast<void>(tlsf.allocBytes(120));
//     static_cast<void>(tlsf.allocBytes(240));
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
// TEST_F(ManagedTLSFTests, MoveAssign_CopiesAttributesToNewObject)
// {
//     constexpr auto sampleAllocation = 50;
//     static_cast<void>(tlsf.allocBytes(sampleAllocation));
//     pmm::TLSF<> tlsf2(256);
//
//     tlsf2 = std::move(tlsf);
//     EXPECT_EQ(tlsfSize - sampleAllocation, tlsf2.freeSize());
//     EXPECT_EQ(tlsfSize, tlsf2.size());
//     EXPECT_EQ(sampleAllocation, tlsf2.usedSize());
// }
//
//
// TEST_F(ManagedTLSFTests, MoveAssign_MovesTelemetry)
// {
//     static_cast<void>(tlsf.allocBytes(120));
//     static_cast<void>(tlsf.allocBytes(240));
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

// /**
//  * @test Verify that allocBytes returns an address aligned to sizeof(void*) bytes
//  *       given no alignment was passed-in.
//  */
// TEST_F(ManagedTLSFTests, AllocBytes_Returns8ByteAlignedAddressByDefault)
// {
//     // Misalign bytes to 2
//     [[maybe_unused]] void* misalignedBytes = tlsf.allocBytes(2, 2);
//
//     void* bytes = tlsf.allocBytes(8);
//
//     const auto address = reinterpret_cast<uintptr_t>(bytes);
//     EXPECT_EQ(0, address % sizeof(void*));
// }
//
//
// TEST_F(ManagedTLSFTests, AllocBytes_ReturnsProvidedByteAlignedAddress)
// {
//     constexpr auto byteAlignment = 32;
//     void* bytes                  = tlsf.allocBytes(128, byteAlignment);
//
//     const auto address = reinterpret_cast<uintptr_t>(bytes);
//     EXPECT_EQ(0, address % byteAlignment);
// }
//
//
// TEST_F(ManagedTLSFTests, AllocBytes_ReturnsNonNullPtrWhenAllocatingMemoryLessThanTLSFSize)
// {
//     void* bytes = tlsf.allocBytes(256);
//
//     EXPECT_NE(nullptr, bytes);
// }
//
//
// TEST_F(ManagedTLSFTests, AllocBytes_ReturnsNonNullPtrWhenAllocatingMemoryEqualTLSFSize)
// {
//     // 7 is used as a worst case aligned requirement which is 8-bytes by default
//     // on a 64-bit machine
//     void* bytes = tlsf.allocBytes(tlsfSize - 7);
//
//     EXPECT_NE(nullptr, bytes);
// }
//
//
// TEST_F(ManagedTLSFTests, AllocBytes_SubsequentAllocationDoNotCorruptMemory)
// {
//     constexpr auto bufferLength = 8;
//     // Given two contiguous block of memory allocated back to back
//     const auto firstAlloc = static_cast<int*>(tlsf.allocBytes(bufferLength * sizeof(int)));
//     for (std::size_t i = 0; i < bufferLength; ++i)
//     {
//         firstAlloc[i] = static_cast<int>(i + 5);
//     }
//
//     const auto secondAlloc = static_cast<int*>(tlsf.allocBytes(bufferLength * sizeof(int)));
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
// TEST_F(ManagedTLSFTests, AllocBytes_UpdatesTelemetry)
// {
//     constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;
//
//     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
//     static_cast<void>(tlsf.allocBytes(byte1));
//     static_cast<void>(tlsf.allocBytes(byte2));
//     static_cast<void>(tlsf.allocBytes(byte3));
//
//     constexpr std::size_t expectedMinUsage  = byte1;
//     constexpr std::size_t expectedPeakUsage = byte3;
//     constexpr std::size_t expectedUsage     = byte1 + byte2 + byte3;
//
//     EXPECT_EQ(expectedMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(expectedPeakUsage, tlsf.getTelemetry().getPeakUsage());
//     EXPECT_EQ(expectedUsage, tlsf.getTelemetry().getUsedSize());
// }
//
//
//
// /**************************************
//  *              ALLOC                 *
//  **************************************/
//
// TEST_F(ManagedTLSFTests, Alloc_AllocatesAnObjectInTheTLSF)
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
// TEST_F(ManagedTLSFTests, Alloc_AlignsToTargetAlignment)
// {
//     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
//     static_cast<void>(tlsf.allocBytes(2, 2));
//
//     constexpr auto expectedAlignment = alignof(Vec4);
//     [[maybe_unused]] const auto vec  = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
//
//     EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vec) % expectedAlignment);
// }
//
//
// TEST_F(ManagedTLSFTests, Alloc_UpdatesTelemetry)
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
//
//
//
// /**************************************
//  *            ALLOC V(ector)           *
//  **************************************/
//
// TEST_F(ManagedTLSFTests, AllocV_ReturnsAContinguousBlockOfMemory)
// {
//     constexpr auto blockCount = 10;
//     const auto vertices       = tlsf.allocV<Vec4>(blockCount);
//
//     EXPECT_EQ(blockCount, vertices.size());
//     EXPECT_EQ(blockCount * sizeof(Vec4), vertices.size_bytes());
// }
//
//
// TEST_F(ManagedTLSFTests, AllocV_SubsequentAllocationDoNotCorruptMemory)
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
//
//
// TEST_F(ManagedTLSFTests, AllocV_UpdatesTelemetry)
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
//
//
//
// /**************************************
//  *             RESIZE                 *
//  **************************************/
//
// TEST_F(ManagedTLSFTests, Resize_NewSizeSmallerThanOldSizeReturnsSameAddress)
// {
//     constexpr auto byteSize   = 128;
//     const auto firstByteChunk = tlsf.allocBytes(byteSize);
//     // Additional allocation
//     [[maybe_unused]] const auto secondByteChunk = tlsf.allocBytes(byteSize);
//
//     const auto data = tlsf.resize(firstByteChunk, byteSize, byteSize / 2, alignof(void*));
//
//     EXPECT_EQ(reinterpret_cast<uintptr_t>(firstByteChunk), reinterpret_cast<uintptr_t>(data));
// }
//
//
// TEST_F(ManagedTLSFTests, Resize_LatestAllocationOnlyResizeByOffsetDifference)
// {
//     constexpr auto byteSize    = 128;
//     constexpr auto newByteSize = byteSize * 2;
//
//     // Allocate the chunk
//     const auto firstByteChunk = tlsf.allocBytes(byteSize);
//     [[maybe_unused]] const auto data =
//         static_cast<int*>(tlsf.resize(firstByteChunk, byteSize, newByteSize, alignof(int)));
//     // Resize it
//     constexpr auto firstArraySize = newByteSize / sizeof(int);
//
//     // Write some data
//     for (std::size_t i = 0; i < firstArraySize; ++i)
//     {
//         data[i] = static_cast<int>(i + 100);
//     }
//
//     // Allocate some more memory
//     [[maybe_unused]] auto vec = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
//
//     // Verify data is not overwritten
//     for (std::size_t i = 0; i < firstArraySize; ++i)
//     {
//         EXPECT_EQ(i + 100, data[i]);
//     }
// }
//
//
// /** @test Verify that resize of allocation before the last allocation returns a new buffer. */
// TEST_F(ManagedTLSFTests, Resize_AllocationPriorToLatestAllocationReturnNewBuffer)
// {
//     constexpr auto byteSize    = 128;
//     constexpr auto newByteSize = byteSize * 2;
//
//     const auto firstByteChunk                   = tlsf.allocBytes(byteSize);
//     [[maybe_unused]] const auto secondByteChunk = tlsf.allocBytes(byteSize);
//
//     [[maybe_unused]] const auto data = tlsf.resize(firstByteChunk, byteSize, newByteSize, alignof(void*));
//
//     EXPECT_NE(reinterpret_cast<uintptr_t>(firstByteChunk), reinterpret_cast<uintptr_t>(data));
// }
//
//
// /** @test Verify that resize of allocation before the last allocation copies old data. */
// TEST_F(ManagedTLSFTests, Resize_AllocationPriorToLatestAllocationCopiesOldData)
// {
//     constexpr auto byteSize    = 128;
//     constexpr auto newByteSize = byteSize * 2;
//
//     // Allocate memory
//     const auto firstByteChunk = static_cast<int*>(tlsf.allocBytes(byteSize));
//     constexpr auto arraySize  = byteSize / sizeof(int);
//
//     // Write some data to the allocated memory
//     for (std::size_t i = 0; i < arraySize; ++i)
//     {
//         firstByteChunk[i] = static_cast<int>(i + 100);
//     }
//
//     // Allocate some more memory
//     [[maybe_unused]] const auto secondByteChunk = tlsf.allocBytes(byteSize);
//
//     // Resize the first buffer
//     const auto data = static_cast<int*>(tlsf.resize(firstByteChunk, byteSize, newByteSize, alignof(int)));
//
//     // Verify data is copied
//     for (std::size_t i = 0; i < arraySize; ++i)
//     {
//         EXPECT_EQ(i + 100, data[i]);
//     }
// }
//
//
// TEST_F(ManagedTLSFTests, Resize_SameMemorySize_DoesNotUpdateTelemetry)
// {
//     constexpr auto byteSize = 128;
//
//     const auto allocatedBytes = tlsf.allocBytes(byteSize);
//
//     const auto oldUsage     = tlsf.getTelemetry().getUsedSize();
//     const auto oldMinUsage  = tlsf.getTelemetry().getMinUsage();
//     const auto oldPeakUsage = tlsf.getTelemetry().getPeakUsage();
//
//
//     [[maybe_unused]] const auto data = tlsf.resize(allocatedBytes, byteSize, byteSize, alignof(void*));
//
//     EXPECT_EQ(oldUsage, tlsf.getTelemetry().getUsedSize());
//     EXPECT_EQ(oldMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(oldPeakUsage, tlsf.getTelemetry().getPeakUsage());
// }
//
//
// TEST_F(ManagedTLSFTests, Resize_SmallerMemorySize_DoesNotUpdateTelemetry)
// {
//     constexpr auto byteSize    = 128;
//     constexpr auto newByteSize = byteSize - 10;
//
//     const auto allocatedBytes = tlsf.allocBytes(byteSize);
//
//     const auto oldUsage     = tlsf.getTelemetry().getUsedSize();
//     const auto oldMinUsage  = tlsf.getTelemetry().getMinUsage();
//     const auto oldPeakUsage = tlsf.getTelemetry().getPeakUsage();
//
//
//     [[maybe_unused]] const auto data = tlsf.resize(allocatedBytes, byteSize, newByteSize, alignof(void*));
//
//     EXPECT_EQ(oldUsage, tlsf.getTelemetry().getUsedSize());
//     EXPECT_EQ(oldMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(oldPeakUsage, tlsf.getTelemetry().getPeakUsage());
// }
//
//
// TEST_F(ManagedTLSFTests, Resize_LatestAllocationResize_UpdatesTelemetry)
// {
//     constexpr auto byteSize       = 128;
//     constexpr auto byteDifference = 100;
//     constexpr auto newByteSize    = byteSize + byteDifference;
//
//     [[maybe_unused]] const auto unusedBytes = tlsf.allocBytes(50);
//     const auto allocatedBytes               = tlsf.allocBytes(byteSize);
//
//     const auto oldUsage                      = tlsf.getTelemetry().getUsedSize();
//     const auto oldMinUsage                   = tlsf.getTelemetry().getMinUsage();
//     [[maybe_unused]] const auto oldPeakUsage = tlsf.getTelemetry().getPeakUsage();
//
//
//     [[maybe_unused]] const auto data = tlsf.resize(allocatedBytes, byteSize, newByteSize, alignof(void*));
//
//     EXPECT_EQ(oldUsage + byteDifference, tlsf.getTelemetry().getUsedSize());
//     EXPECT_EQ(oldMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(oldPeakUsage + byteDifference, tlsf.getTelemetry().getPeakUsage());
// }
//
//
// TEST_F(ManagedTLSFTests, Resize_InBetweenAllocationResize_UpdatesTelemetry)
// {
//     constexpr auto byteSize       = 128;
//     constexpr auto byteDifference = 100;
//     constexpr auto newByteSize    = byteSize + byteDifference;
//
//     const auto allocatedBytes               = tlsf.allocBytes(byteSize);
//     [[maybe_unused]] const auto unusedBytes = tlsf.allocBytes(50);
//
//     const auto oldUsage    = tlsf.getTelemetry().getUsedSize();
//     const auto oldMinUsage = tlsf.getTelemetry().getMinUsage();
//
//
//     [[maybe_unused]] const auto data = tlsf.resize(allocatedBytes, byteSize, newByteSize, alignof(void*));
//
//     EXPECT_EQ(oldUsage + newByteSize, tlsf.getTelemetry().getUsedSize());
//     EXPECT_EQ(oldMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(newByteSize, tlsf.getTelemetry().getPeakUsage());
// }
//
//
// TEST_F(ManagedTLSFTests, ResizeFast_NewSizeSmallerThanOldSizeReturnsNewBufferWithOldData)
// {
//     constexpr auto byteSize   = 128;
//     const auto firstByteChunk = static_cast<size_t*>(tlsf.allocBytes(byteSize));
//     const auto dataCount      = byteSize / sizeof(size_t);
//     for (size_t i = 0; i < dataCount; ++i)
//     {
//         firstByteChunk[i] = i + 11;
//     }
//     // Additional allocation
//     [[maybe_unused]] const auto secondByteChunk = tlsf.allocBytes(byteSize);
//
//     const auto data = static_cast<size_t*>(tlsf.resizeFast(firstByteChunk, byteSize, byteSize / 2, alignof(void*)));
//
//     EXPECT_NE(reinterpret_cast<uintptr_t>(firstByteChunk), reinterpret_cast<uintptr_t>(data));
//     // Verify data is not overwritten
//     for (std::size_t i = 0; i < dataCount; ++i)
//     {
//         EXPECT_EQ(i + 11, data[i]);
//     }
// }
//
//
// TEST_F(ManagedTLSFTests, ResizeFast_LatestAllocationReturnsNewBufferWithOldData)
// {
//     constexpr auto byteSize    = 128;
//     constexpr auto newByteSize = byteSize * 2;
//
//     // Allocate the chunk
//     const auto firstByteChunk = tlsf.allocBytes(byteSize);
//     const auto data           = static_cast<int*>(tlsf.resizeFast(firstByteChunk, byteSize, newByteSize,
//     alignof(int)));
//
//     // Resize it
//     constexpr auto firstArraySize = newByteSize / sizeof(int);
//
//     // Write some data
//     for (std::size_t i = 0; i < firstArraySize; ++i)
//     {
//         data[i] = static_cast<int>(i + 100);
//     }
//
//     // Allocate some more memory
//     [[maybe_unused]] auto vec = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
//
//     // Verify data is not overwritten
//     for (std::size_t i = 0; i < firstArraySize; ++i)
//     {
//         EXPECT_EQ(i + 100, data[i]);
//     }
// }
//
//
// TEST_F(ManagedTLSFTests, ResizeFast_AllocationPriorToLatestAllocationReturnNewBufferWithOldData)
// {
//     constexpr auto byteSize    = 128;
//     constexpr auto newByteSize = byteSize * 2;
//
//     const auto firstByteChunk = static_cast<size_t*>(tlsf.allocBytes(byteSize));
//     const auto dataCount      = byteSize / sizeof(size_t);
//     for (size_t i = 0; i < dataCount; ++i)
//     {
//         firstByteChunk[i] = i + 11;
//     }
//
//     [[maybe_unused]] const auto secondByteChunk = tlsf.allocBytes(byteSize);
//
//     const auto data = static_cast<size_t*>(tlsf.resizeFast(firstByteChunk, byteSize, newByteSize, alignof(size_t)));
//
//     EXPECT_NE(reinterpret_cast<uintptr_t>(firstByteChunk), reinterpret_cast<uintptr_t>(data));
//     for (size_t i = 0; i < dataCount; ++i)
//     {
//         EXPECT_EQ(i + 11, data[i]);
//     }
// }
//
//
// TEST_F(ManagedTLSFTests, ResizeFast_SameMemorySize_UpdatesTelemetry)
// {
//     constexpr auto byteSize = 128;
//
//     const auto allocatedBytes = tlsf.allocBytes(byteSize);
//
//     const auto oldUsage     = tlsf.getTelemetry().getUsedSize();
//     const auto oldMinUsage  = tlsf.getTelemetry().getMinUsage();
//     const auto oldPeakUsage = tlsf.getTelemetry().getPeakUsage();
//
//
//     [[maybe_unused]] const auto data = tlsf.resizeFast(allocatedBytes, byteSize, byteSize, alignof(void*));
//
//     // Since we are allocating twice the usage will also increment by 2x, but the peak stats will remain the same.
//     EXPECT_EQ(2 * oldUsage, tlsf.getTelemetry().getUsedSize());
//     EXPECT_EQ(oldMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(oldPeakUsage, tlsf.getTelemetry().getPeakUsage());
// }
//
//
// TEST_F(ManagedTLSFTests, Resize_SmallerMemorySize_UpdatesTelemetry)
// {
//     constexpr auto byteSize    = 128;
//     constexpr auto newByteSize = byteSize - 10;
//
//     const auto allocatedBytes = tlsf.allocBytes(byteSize);
//
//     const auto oldUsage     = tlsf.getTelemetry().getUsedSize();
//     const auto oldPeakUsage = tlsf.getTelemetry().getPeakUsage();
//
//
//     [[maybe_unused]] const auto data = tlsf.resizeFast(allocatedBytes, byteSize, newByteSize, alignof(void*));
//
//     // Since we are allocating twice the usage will also increment.
//     EXPECT_EQ(oldUsage + newByteSize, tlsf.getTelemetry().getUsedSize());
//     // Since the new allocation is smaller the min usage will decrease to the new size
//     EXPECT_EQ(newByteSize, tlsf.getTelemetry().getMinUsage());
//     // But peak usage doesn't change
//     EXPECT_EQ(oldPeakUsage, tlsf.getTelemetry().getPeakUsage());
// }
//
//
// TEST_F(ManagedTLSFTests, ResizeFast_LatestAllocationResize_UpdatesTelemetry)
// {
//     constexpr auto byteSize       = 128;
//     constexpr auto byteDifference = 100;
//     constexpr auto newByteSize    = byteSize + byteDifference;
//
//     [[maybe_unused]] const auto unusedBytes = tlsf.allocBytes(50);
//     const auto allocatedBytes               = tlsf.allocBytes(byteSize);
//
//     const auto oldUsage                      = tlsf.getTelemetry().getUsedSize();
//     const auto oldMinUsage                   = tlsf.getTelemetry().getMinUsage();
//     [[maybe_unused]] const auto oldPeakUsage = tlsf.getTelemetry().getPeakUsage();
//
//
//     [[maybe_unused]] const auto data = tlsf.resizeFast(allocatedBytes, byteSize, newByteSize, alignof(void*));
//
//     // Since we are creating a new allocation, the usage stats increment
//     EXPECT_EQ(oldUsage + newByteSize, tlsf.getTelemetry().getUsedSize());
//     EXPECT_EQ(oldMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(newByteSize, tlsf.getTelemetry().getPeakUsage());
// }
//
//
// TEST_F(ManagedTLSFTests, ResizeFast_InBetweenAllocationResize_UpdatesTelemetry)
// {
//     constexpr auto byteSize       = 128;
//     constexpr auto byteDifference = 100;
//     constexpr auto newByteSize    = byteSize + byteDifference;
//
//     const auto allocatedBytes               = tlsf.allocBytes(byteSize);
//     [[maybe_unused]] const auto unusedBytes = tlsf.allocBytes(50);
//
//     const auto oldUsage    = tlsf.getTelemetry().getUsedSize();
//     const auto oldMinUsage = tlsf.getTelemetry().getMinUsage();
//
//
//     [[maybe_unused]] const auto data = tlsf.resizeFast(allocatedBytes, byteSize, newByteSize, alignof(void*));
//
//     EXPECT_EQ(oldUsage + newByteSize, tlsf.getTelemetry().getUsedSize());
//     EXPECT_EQ(oldMinUsage, tlsf.getTelemetry().getMinUsage());
//     EXPECT_EQ(newByteSize, tlsf.getTelemetry().getPeakUsage());
// }


/**************************************
 *                                    *
 *           INTERNAL TESTS           *
 *                                    *
 **************************************/

// Namespacing is required for testing internal state
namespace pmm
{

    TEST_F(ManagedTLSFTests, MoveCtor_ClearsMovedTLSFsInternalBuffer)
    {
        [[maybe_unused]] const TLSF<pmm::ManagedMemory> tlsf2 = std::move(tlsf);
        // NOLINT(bugprone-use-after-move)
        EXPECT_EQ(nullptr, tlsf._buffer);
    }


    TEST_F(ManagedTLSFTests, MoveCtor_MovesBufferIntoNewObject)
    {
        const auto initialPointer  = tlsf._buffer;
        const auto initialUsedSize = tlsf._usedSize;
        const auto initialSize     = tlsf._size;
        const auto initialFLMask   = tlsf._flBitmask;
        const auto initialSLMask   = tlsf._slBitmask;

        const TLSF<pmm::ManagedMemory> tlsf2 = std::move(tlsf);
        EXPECT_EQ(initialPointer, tlsf2._buffer);
        EXPECT_EQ(initialUsedSize, tlsf2._usedSize);
        EXPECT_EQ(initialSize, tlsf2._size);
        EXPECT_EQ(initialFLMask, tlsf2._flBitmask);
        EXPECT_EQ(initialSLMask, tlsf2._slBitmask);
    }


    TEST_F(ManagedTLSFTests, MoveAssign_ClearsMovedTLSF)
    {
        [[maybe_unused]] TLSF<pmm::ManagedMemory> tlsf2(256);

        static_cast<void>(tlsf2 = std::move(tlsf));
        EXPECT_EQ(nullptr, tlsf._buffer);
    }


    TEST_F(ManagedTLSFTests, MoveAssign_MovesBufferIntoNewObject)
    {
        const auto initialPointer  = tlsf._buffer;
        const auto initialUsedSize = tlsf._usedSize;
        const auto initialSize     = tlsf._size;
        const auto initialFLMask   = tlsf._flBitmask;
        const auto initialSLMask   = tlsf._slBitmask;
        TLSF<pmm::ManagedMemory> tlsf2(256);

        tlsf2 = std::move(tlsf);

        EXPECT_EQ(initialPointer, tlsf2._buffer);
        EXPECT_EQ(initialPointer, tlsf2._buffer);
        EXPECT_EQ(initialUsedSize, tlsf2._usedSize);
        EXPECT_EQ(initialSize, tlsf2._size);
        EXPECT_EQ(initialFLMask, tlsf2._flBitmask);
        EXPECT_EQ(initialSLMask, tlsf2._slBitmask);
    }


    TEST_F(ManagedTLSFTests, MoveAssign_SelfAssignmentReturnsTheSameTLSF)
    {
        const auto initialAddress = reinterpret_cast<uintptr_t>(tlsf._buffer);
        const auto initialFLMask  = tlsf._flBitmask;
        const auto initialSLMask  = tlsf._slBitmask;
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
        EXPECT_EQ(initialFLMask, tlsf._flBitmask);
        EXPECT_EQ(initialSLMask, tlsf._slBitmask);
    }


    TEST_F(ManagedTLSFTests, MoveAssign_DeletingOriginalTLSFDoNotDeleteTheNewTLSFsMemory)
    {
        TLSF<pmm::ManagedMemory> tlsf2(256);
        constexpr auto scopedTLSFSize = 512;

        // The tlsf being moved is scoped
        {
            TLSF<pmm::ManagedMemory> scopedTLSF(scopedTLSFSize);
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

    TEST_P(ManagedTLSF_MappingInsertTests, ReturnsValidFLAndSLIndices)
    {
        const auto [size, expectedFl, expectedSl] = GetParam();
        const auto [fl, sl]                       = TLSF<>::mappingInsert(size);
        EXPECT_EQ(expectedFl, fl);
        EXPECT_EQ(expectedSl, sl);
    }



    TEST_P(ManagedTLSF_MappingSearchTests, ReturnsValidFLAndSLIndices)
    {
        const auto [size, expectedFl, expectedSl] = GetParam();
        const auto [fl, sl]                       = TLSF<>::mappingSearch(size);
        EXPECT_EQ(expectedFl, fl);
        EXPECT_EQ(expectedSl, sl);
    }


    // /**
    //  * @test Verify that when allocation buffer using allocBytes, prevOffset is
    //  *       moved by relative to the allocated object's size.
    //  */
    // TEST_F(ManagedTLSFTests, AllocBytes_MovesPrevOffset)
    // {
    //     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    //     static_cast<void>(tlsf.allocBytes(2, 2));
    //
    //     // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
    //     constexpr auto alignment           = 128;
    //     constexpr auto bufferSize          = 64;
    //     [[maybe_unused]] const auto buffer = tlsf.allocBytes(bufferSize, alignment);
    //
    //     EXPECT_EQ(bufferSize, tlsf._offset - tlsf._prevOffset);
    // }
    //
    //
    // /**
    //  * @test Verify that when allocation buffer using alloc, prevOffset is
    //  *       moved by relative to the allocated object's size.
    //  */
    // TEST_F(ManagedTLSFTests, Alloc_MovesPrevOffset)
    // {
    //     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    //     static_cast<void>(tlsf.allocBytes(2, 2));
    //
    //     // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
    //     [[maybe_unused]] const auto vec = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    //
    //     EXPECT_EQ(sizeof(Vec4), tlsf._offset - tlsf._prevOffset);
    // }
    //
    //
    // TEST_F(ManagedTLSFTests, AllocBytes_UpdatesTelemetryPadding)
    // {
    //     const auto buffer          = tlsf.allocBytes(128, 128);
    //     const auto expectedPadding = reinterpret_cast<uintptr_t>(buffer) - reinterpret_cast<uintptr_t>(tlsf._buffer);
    //
    //     EXPECT_EQ(expectedPadding, tlsf.getTelemetry().getTotalPadding());
    // }
    //
    //
    // TEST_F(ManagedTLSFTests, Alloc_UpdatesTelemetryPadding)
    // {
    //     const auto vec4            = tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    //     const auto expectedPadding = reinterpret_cast<uintptr_t>(vec4) - reinterpret_cast<uintptr_t>(tlsf._buffer);
    //
    //     EXPECT_EQ(expectedPadding, tlsf.getTelemetry().getTotalPadding());
    // }
    //
    //
    // TEST_F(ManagedTLSFTests, AllocV_UpdatesTelemetryPadding)
    // {
    //     const auto data = tlsf.allocV<Vec4>(10);
    //     const auto expectedPadding =
    //         reinterpret_cast<uintptr_t>(data.data()) - reinterpret_cast<uintptr_t>(tlsf._buffer);
    //
    //     EXPECT_EQ(expectedPadding, tlsf.getTelemetry().getTotalPadding());
    // }
    //
    //
    //
    // TEST_F(ManagedTLSFTests, Clear_ResetsOffsetToZero)
    // {
    //     [[maybe_unused]] const auto chunkOne = tlsf.allocBytes(128);
    //     [[maybe_unused]] const auto chunkTwo = tlsf.allocBytes(128);
    //
    //     // Initially expect offset and prevOffset are not zero
    //     EXPECT_NE(0, tlsf._offset);
    //     EXPECT_NE(0, tlsf._prevOffset);
    //
    //     // After freeing, offsets are reset
    //     tlsf.clear();
    //
    //     // Offsets are reset to zero
    //     EXPECT_EQ(0, tlsf._offset);
    //     EXPECT_EQ(0, tlsf._prevOffset);
    // }
    //
    //
    // TEST_F(ManagedTLSFTests, Clear_OnlyResetsCurrentTelemetryUsage)
    // {
    //     constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;
    //
    //     // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    //     static_cast<void>(tlsf.allocBytes(byte1));
    //     static_cast<void>(tlsf.allocBytes(byte2));
    //     static_cast<void>(tlsf.allocBytes(byte3));
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
    //
    //
    // TEST_F(ManagedTLSFTests, Resize_LatestAllocationResizeBuffer)
    // {
    //     constexpr auto byteSize    = 128;
    //     constexpr auto newByteSize = byteSize * 2;
    //
    //     [[maybe_unused]] const auto firstByteChunk = tlsf.allocBytes(byteSize);
    //     auto secondByteChunk                       = tlsf.allocBytes(byteSize);
    //     const auto offsetBeforeResize              = tlsf._offset;
    //
    //     [[maybe_unused]] const auto data = tlsf.resize(secondByteChunk, byteSize, newByteSize, alignof(void*));
    //
    //     EXPECT_GT(tlsf._offset, offsetBeforeResize);
    //     EXPECT_EQ(reinterpret_cast<uintptr_t>(secondByteChunk), reinterpret_cast<uintptr_t>(data));
    // }
    //
    //
    // TEST_F(ManagedTLSFTests, Resize_LatestAllocationOnlyResizeByOffsetDifference)
    // {
    //     constexpr auto byteSize    = 128;
    //     constexpr auto newByteSize = byteSize * 2;
    //
    //     [[maybe_unused]] const auto firstByteChunk = tlsf.allocBytes(byteSize);
    //     const auto secondByteChunk                 = tlsf.allocBytes(byteSize);
    //     const auto offsetBeforeResize              = tlsf._offset;
    //     const auto expectedOffset                  = offsetBeforeResize + (newByteSize - byteSize);
    //
    //     [[maybe_unused]] const auto data = tlsf.resize(secondByteChunk, byteSize, newByteSize, alignof(void*));
    //
    //     EXPECT_EQ(expectedOffset, tlsf._offset) << "Offset Mismatch";
    // }
    //
    //
    // TEST_F(ManagedTLSFTests, ZeroOut_ZeroesOutTheInternalBuffer)
    // {
    //     // Fill the buffer with arbitrary data
    //     for (size_t i = 0; i < tlsf.size(); ++i)
    //     {
    //         tlsf._buffer[i] = static_cast<uint8_t>(i % 255);
    //     }
    //     // This call unnecessary for testing, but adhering to how the method is supposed to be called,
    //     // we are leaving it here.
    //     tlsf.clear();
    //
    //     // Zero-out
    //     tlsf.zeroOut();
    //
    //     // Assert tlsf's buffer is cleared
    //     for (size_t i = 0; i < tlsf.size(); ++i)
    //     {
    //         EXPECT_EQ(0, tlsf._buffer[i]);
    //     }
    // }

} // namespace pmm
