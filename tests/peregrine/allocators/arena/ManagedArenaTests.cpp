/**
 * @file ManagedArenaTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 7, 2026
 *
 * @brief Verify managed arena allocation, free, and helper function logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */



#include "Utils.h"

#include <array>
#include <gtest/gtest.h>
#include <peregrine/allocators/Arena.h>
#include <peregrine/utils/Constants.h>
#include <utility>



/**
 * @addtogroup T_PMM_Arena
 * @{
 */

namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/
    using namespace pmm::constants;

    /**
     * @brief Test fixture for managed @ref pmm::Arena.
     */
    class ManagedArenaTests: public testing::Test
    {
    public:
        size_t arenaSize{ 2_KB };
        pmm::Arena<> arena{ arenaSize };
    };

} // namespace



/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/**************************************
 *           INITIALIZATIONS          *
 **************************************/

TEST_F(ManagedArenaTests, Ctor_InitializesArenaWithTheGivenBytes) { EXPECT_EQ(arenaSize, arena.size()); }


TEST_F(ManagedArenaTests, ArenaHasZeroUsedSizeInitially) { EXPECT_EQ(0, arena.usedSize()); }


TEST_F(ManagedArenaTests, ArenaHasFreeSpaceEqualToSizeInitially) { EXPECT_EQ(arenaSize, arena.freeSize()); }


TEST_F(ManagedArenaTests, MoveCtor_CopiesAttributesToNewObject)
{
    const pmm::Arena<> arena2 = std::move(arena);
    EXPECT_EQ(arenaSize, arena2.freeSize());
    EXPECT_EQ(arenaSize, arena2.size());
    EXPECT_EQ(0, arena2.usedSize());
    EXPECT_EQ(arenaSize, arena2.getTelemetry().getArenaSize());
}


TEST_F(ManagedArenaTests, MoveCtor_MovesTelemetry)
{
    static_cast<void>(arena.allocBytes(120));
    static_cast<void>(arena.allocBytes(240));
    // Get the telemetry to ensure that the value is preserved when moving
    // DON'T get by reference as it will change internally
    const auto telemetry = arena.getTelemetry();

    const pmm::Arena<> arena2 = std::move(arena);

    // Checking for telemetry equality
    EXPECT_EQ(telemetry.getCurrentUsage(), arena2.getTelemetry().getCurrentUsage());
    EXPECT_EQ(telemetry.getPeakUsage(), arena2.getTelemetry().getPeakUsage());
    EXPECT_EQ(telemetry.getArenaSize(), arena2.getTelemetry().getArenaSize());
    EXPECT_EQ(telemetry.getMinUsage(), arena2.getTelemetry().getMinUsage());
}


TEST_F(ManagedArenaTests, MoveAssign_CopiesAttributesToNewObject)
{
    constexpr auto sampleAllocation = 50;
    static_cast<void>(arena.allocBytes(sampleAllocation));
    pmm::Arena<> arena2(256);

    arena2 = std::move(arena);
    EXPECT_EQ(arenaSize - sampleAllocation, arena2.freeSize());
    EXPECT_EQ(arenaSize, arena2.size());
    EXPECT_EQ(sampleAllocation, arena2.usedSize());
}


TEST_F(ManagedArenaTests, MoveAssign_MovesTelemetry)
{
    static_cast<void>(arena.allocBytes(120));
    static_cast<void>(arena.allocBytes(240));
    // Get the telemetry to ensure that the value is preserved when moving
    // DON'T get by reference as it will change internally
    const auto telemetry = arena.getTelemetry();

    pmm::Arena<> arena2(256);
    arena2 = std::move(arena);

    // Checking for telemetry equality
    EXPECT_EQ(telemetry.getCurrentUsage(), arena2.getTelemetry().getCurrentUsage());
    EXPECT_EQ(telemetry.getPeakUsage(), arena2.getTelemetry().getPeakUsage());
    EXPECT_EQ(telemetry.getArenaSize(), arena2.getTelemetry().getArenaSize());
    EXPECT_EQ(telemetry.getMinUsage(), arena2.getTelemetry().getMinUsage());
}



/**************************************
 *            ALLOC BYTES             *
 **************************************/

/**
 * @test Verify that allocBytes returns an address aligned to sizeof(void*) bytes
 *       given no alignment was passed-in.
 */
TEST_F(ManagedArenaTests, AllocBytes_Returns8ByteAlignedAddressByDefault)
{
    // Misalign bytes to 2
    [[maybe_unused]] void* misalignedBytes = arena.allocBytes(2, 2);

    void* bytes = arena.allocBytes(8);

    const auto address = reinterpret_cast<uintptr_t>(bytes);
    EXPECT_EQ(0, address % sizeof(void*));
}


TEST_F(ManagedArenaTests, AllocBytes_ReturnsProvidedByteAlignedAddress)
{
    constexpr auto byteAlignment = 32;
    void* bytes                  = arena.allocBytes(128, byteAlignment);

    const auto address = reinterpret_cast<uintptr_t>(bytes);
    EXPECT_EQ(0, address % byteAlignment);
}


TEST_F(ManagedArenaTests, AllocBytes_ReturnsNonNullPtrWhenAllocatingMemoryLessThanArenaSize)
{
    void* bytes = arena.allocBytes(256);

    EXPECT_NE(nullptr, bytes);
}


TEST_F(ManagedArenaTests, AllocBytes_ReturnsNonNullPtrWhenAllocatingMemoryEqualArenaSize)
{
    // 7 is used as a worst case aligned requirement which is 8-bytes by default
    // on a 64-bit machine
    void* bytes = arena.allocBytes(arenaSize - 7);

    EXPECT_NE(nullptr, bytes);
}


TEST_F(ManagedArenaTests, AllocBytes_SubsequentAllocationDoNotCorruptMemory)
{
    constexpr auto bufferLength = 8;
    // Given two contiguous block of memory allocated back to back
    const auto firstAlloc = static_cast<int*>(arena.allocBytes(bufferLength * sizeof(int)));
    for (std::size_t i = 0; i < bufferLength; ++i)
    {
        firstAlloc[i] = static_cast<int>(i + 5);
    }

    const auto secondAlloc = static_cast<int*>(arena.allocBytes(bufferLength * sizeof(int)));
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

TEST_F(ManagedArenaTests, AllocBytes_UpdatesTelemetry)
{
    constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;

    // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    static_cast<void>(arena.allocBytes(byte1));
    static_cast<void>(arena.allocBytes(byte2));
    static_cast<void>(arena.allocBytes(byte3));

    constexpr std::size_t expectedMinUsage  = byte1;
    constexpr std::size_t expectedPeakUsage = byte3;
    constexpr std::size_t expectedUsage     = byte1 + byte2 + byte3;

    EXPECT_EQ(expectedMinUsage, arena.getTelemetry().getMinUsage());
    EXPECT_EQ(expectedPeakUsage, arena.getTelemetry().getPeakUsage());
    EXPECT_EQ(expectedUsage, arena.getTelemetry().getCurrentUsage());
}



/**************************************
 *              ALLOC                 *
 **************************************/

TEST_F(ManagedArenaTests, Alloc_AllocatesAnObjectInTheArena)
{
    const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_FLOAT_EQ(1.0f, vec->x);
    EXPECT_FLOAT_EQ(2.0f, vec->y);
    EXPECT_FLOAT_EQ(3.0f, vec->z);
    EXPECT_FLOAT_EQ(4.0f, vec->w);
}


TEST_F(ManagedArenaTests, Alloc_AlignsToTargetAlignment)
{
    // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    static_cast<void>(arena.allocBytes(2, 2));

    constexpr auto expectedAlignment = alignof(Vec4);
    [[maybe_unused]] const auto vec  = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vec) % expectedAlignment);
}


TEST_F(ManagedArenaTests, Alloc_UpdatesTelemetry)
{
    // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    static_cast<void>(arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
    static_cast<void>(arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
    static_cast<void>(arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
    static_cast<void>(arena.alloc<int>(1));

    constexpr std::size_t expectedMinUsage  = sizeof(int);
    constexpr std::size_t expectedPeakUsage = sizeof(Vec4);
    constexpr std::size_t expectedUsage     = sizeof(Vec4) * 3 + sizeof(int);

    EXPECT_EQ(expectedMinUsage, arena.getTelemetry().getMinUsage());
    EXPECT_EQ(expectedPeakUsage, arena.getTelemetry().getPeakUsage());
    EXPECT_EQ(expectedUsage, arena.getTelemetry().getCurrentUsage());
}



/**************************************
 *            ALLOC V(ector)           *
 **************************************/

TEST_F(ManagedArenaTests, AllocV_ReturnsAContinguousBlockOfMemory)
{
    constexpr auto blockCount = 10;
    const auto vertices       = arena.allocV<Vec4>(blockCount);

    EXPECT_EQ(blockCount, vertices.size());
    EXPECT_EQ(blockCount * sizeof(Vec4), vertices.size_bytes());
}


TEST_F(ManagedArenaTests, AllocV_SubsequentAllocationDoNotCorruptMemory)
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

    auto vertices = arena.allocV<Vec4>(blockCount);
    auto edges    = arena.allocV<Vec4>(blockCount);

    // Write into the first allocated span
    for (std::size_t i = 0; i < blockCount; ++i)
    {
        vertices[i] = Vec4{ vertexData[i * 4], vertexData[(i * 4) + 1], vertexData[i * 4 + 2], vertexData[i * 4 + 3] };
    }

    // Write into the second allocated span
    for (std::size_t i = 0; i < blockCount; ++i)
    {
        edges[i] = Vec4{ edgeData[i * 4], edgeData[(i * 4) + 1], edgeData[i * 4 + 2], edgeData[i * 4 + 3] };
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


TEST_F(ManagedArenaTests, AllocV_UpdatesTelemetry)
{
    constexpr std::size_t count1 = 2, count2 = 4, count3 = 6;

    // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
    static_cast<void>(arena.allocV<Vec4>(count1));
    static_cast<void>(arena.allocV<Vec4>(count2));
    static_cast<void>(arena.allocV<Vec4>(count3));

    constexpr std::size_t expectedMinUsage  = count1 * sizeof(Vec4);
    constexpr std::size_t expectedPeakUsage = count3 * sizeof(Vec4);
    constexpr std::size_t expectedUsage     = (count1 + count2 + count3) * sizeof(Vec4);

    EXPECT_EQ(expectedMinUsage, arena.getTelemetry().getMinUsage());
    EXPECT_EQ(expectedPeakUsage, arena.getTelemetry().getPeakUsage());
    EXPECT_EQ(expectedUsage, arena.getTelemetry().getCurrentUsage());
}



/**************************************
 *             RESIZE                 *
 **************************************/

TEST_F(ManagedArenaTests, Resize_NewSizeLowerThanOldSizeReturnsSameAddress)
{
    constexpr auto byteSize   = 128;
    const auto firstByteChunk = arena.allocBytes(byteSize);
    // Additional allocation
    [[maybe_unused]] const auto secondByteChunk = arena.allocBytes(byteSize);

    const auto data = arena.resize(firstByteChunk, byteSize, byteSize / 2, alignof(void*));

    EXPECT_EQ(reinterpret_cast<uintptr_t>(firstByteChunk), reinterpret_cast<uintptr_t>(data));
}


TEST_F(ManagedArenaTests, Resize_LatestAllocationOnlyResizeByOffsetDifference)
{
    constexpr auto byteSize    = 128;
    constexpr auto newByteSize = byteSize * 2;

    // Allocate the chunk
    const auto firstByteChunk = arena.allocBytes(byteSize);
    [[maybe_unused]] const auto data =
        static_cast<int*>(arena.resize(firstByteChunk, byteSize, newByteSize, alignof(int)));
    // Resize it
    constexpr auto firstArraySize = newByteSize / sizeof(int);

    // Write some data
    for (std::size_t i = 0; i < firstArraySize; ++i)
    {
        data[i] = static_cast<int>(i + 100);
    }

    // Allocate some more memory
    [[maybe_unused]] auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    // Verify data is not overwritten
    for (std::size_t i = 0; i < firstArraySize; ++i)
    {
        EXPECT_EQ(i + 100, data[i]);
    }
}


/** @test Verify that resize of allocation before the last allocation returns a new buffer. */
TEST_F(ManagedArenaTests, Resize_AllocationPriorToLatestAllocationReturnNewBuffer)
{
    constexpr auto byteSize    = 128;
    constexpr auto newByteSize = byteSize * 2;

    const auto firstByteChunk                   = arena.allocBytes(byteSize);
    [[maybe_unused]] const auto secondByteChunk = arena.allocBytes(byteSize);

    [[maybe_unused]] const auto data = arena.resize(firstByteChunk, byteSize, newByteSize, alignof(void*));

    EXPECT_NE(reinterpret_cast<uintptr_t>(firstByteChunk), reinterpret_cast<uintptr_t>(data));
}


/** @test Verify that resize of allocation before the last allocation copies old data. */
TEST_F(ManagedArenaTests, Resize_AllocationPriorToLatestAllocationCopiesOldData)
{
    ;
    constexpr auto byteSize    = 128;
    constexpr auto newByteSize = byteSize * 2;

    // Allocate memory
    const auto firstByteChunk = static_cast<int*>(arena.allocBytes(byteSize));
    constexpr auto arraySize  = byteSize / sizeof(int);

    // Write some data to the allocated memory
    for (std::size_t i = 0; i < arraySize; ++i)
    {
        firstByteChunk[i] = static_cast<int>(i + 100);
    }

    // Allocate some more memory
    [[maybe_unused]] const auto secondByteChunk = arena.allocBytes(byteSize);

    // Resize the first buffer
    const auto data = static_cast<int*>(arena.resize(firstByteChunk, byteSize, newByteSize, alignof(int)));

    // Verify data is copied
    for (std::size_t i = 0; i < arraySize; ++i)
    {
        EXPECT_EQ(i + 100, data[i]);
    }
}


TEST_F(ManagedArenaTests, Resize_SameMemorySize_DoesNotUpdateTelemetry)
{
    constexpr auto byteSize = 128;

    const auto allocatedBytes = arena.allocBytes(byteSize);

    const auto oldUsage     = arena.getTelemetry().getCurrentUsage();
    const auto oldMinUsage  = arena.getTelemetry().getMinUsage();
    const auto oldPeakUsage = arena.getTelemetry().getPeakUsage();


    [[maybe_unused]] const auto data = arena.resize(allocatedBytes, byteSize, byteSize, alignof(void*));

    EXPECT_EQ(oldUsage, arena.getTelemetry().getCurrentUsage());
    EXPECT_EQ(oldMinUsage, arena.getTelemetry().getMinUsage());
    EXPECT_EQ(oldPeakUsage, arena.getTelemetry().getPeakUsage());
}


TEST_F(ManagedArenaTests, Resize_SmallerMemorySize_DoesNotUpdateTelemetry)
{
    constexpr auto byteSize    = 128;
    constexpr auto newByteSize = byteSize - 10;

    const auto allocatedBytes = arena.allocBytes(byteSize);

    const auto oldUsage     = arena.getTelemetry().getCurrentUsage();
    const auto oldMinUsage  = arena.getTelemetry().getMinUsage();
    const auto oldPeakUsage = arena.getTelemetry().getPeakUsage();


    [[maybe_unused]] const auto data = arena.resize(allocatedBytes, byteSize, newByteSize, alignof(void*));

    EXPECT_EQ(oldUsage, arena.getTelemetry().getCurrentUsage());
    EXPECT_EQ(oldMinUsage, arena.getTelemetry().getMinUsage());
    EXPECT_EQ(oldPeakUsage, arena.getTelemetry().getPeakUsage());
}


TEST_F(ManagedArenaTests, Resize_LatestAllocationResize_UpdatesTelemetry)
{
    constexpr auto byteSize       = 128;
    constexpr auto byteDifference = 100;
    constexpr auto newByteSize    = byteSize + byteDifference;

    [[maybe_unused]] const auto unusedBytes = arena.allocBytes(50);
    const auto allocatedBytes               = arena.allocBytes(byteSize);

    const auto oldUsage                      = arena.getTelemetry().getCurrentUsage();
    const auto oldMinUsage                   = arena.getTelemetry().getMinUsage();
    [[maybe_unused]] const auto oldPeakUsage = arena.getTelemetry().getPeakUsage();


    [[maybe_unused]] const auto data = arena.resize(allocatedBytes, byteSize, newByteSize, alignof(void*));

    EXPECT_EQ(oldUsage + byteDifference, arena.getTelemetry().getCurrentUsage());
    EXPECT_EQ(oldMinUsage, arena.getTelemetry().getMinUsage());
    EXPECT_EQ(oldPeakUsage + byteDifference, arena.getTelemetry().getPeakUsage());
}


TEST_F(ManagedArenaTests, Resize_InBetweenAllocationResize_UpdatesTelemetry)
{
    constexpr auto byteSize       = 128;
    constexpr auto byteDifference = 100;
    constexpr auto newByteSize    = byteSize + byteDifference;

    const auto allocatedBytes               = arena.allocBytes(byteSize);
    [[maybe_unused]] const auto unusedBytes = arena.allocBytes(50);

    const auto oldUsage    = arena.getTelemetry().getCurrentUsage();
    const auto oldMinUsage = arena.getTelemetry().getMinUsage();


    [[maybe_unused]] const auto data = arena.resize(allocatedBytes, byteSize, newByteSize, alignof(void*));

    EXPECT_EQ(oldUsage + newByteSize, arena.getTelemetry().getCurrentUsage());
    EXPECT_EQ(oldMinUsage, arena.getTelemetry().getMinUsage());
    EXPECT_EQ(newByteSize, arena.getTelemetry().getPeakUsage());
}



/**************************************
 *                                    *
 *           INTERNAL TESTS           *
 *                                    *
 **************************************/

// Namespacing is required for testing internal state
namespace pmm
{

    TEST_F(ManagedArenaTests, MoveCtor_ClearsMovedArena)
    {
        const Arena<> arena2 = std::move(arena);
        // NOLINT(bugprone-use-after-move)
        EXPECT_EQ(nullptr, arena._buffer);
        EXPECT_EQ(0, arena._offset);
        EXPECT_EQ(0, arena._prevOffset);
        EXPECT_EQ(0, arena._sizeInBytes);
        EXPECT_EQ(0, arena.getTelemetry().getCurrentUsage());
    }


    TEST_F(ManagedArenaTests, MoveCtor_MovesBufferIntoNewObject)
    {
        const auto initialPointer    = arena._buffer;
        const auto initialOffset     = arena._offset;
        const auto initialPrevOffset = arena._prevOffset;

        const Arena<> arena2 = std::move(arena);
        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(initialOffset, arena2._offset);
        EXPECT_EQ(initialPrevOffset, arena2._prevOffset);
        EXPECT_EQ(arenaSize, arena2._sizeInBytes);
    }


    TEST_F(ManagedArenaTests, MoveAssign_ClearsMovedArena)
    {
        [[maybe_unused]] Arena<> arena2(256);

        static_cast<void>(arena2 = std::move(arena));
        EXPECT_EQ(nullptr, arena._buffer);
        EXPECT_EQ(0, arena._offset);
        EXPECT_EQ(0, arena._prevOffset);
        EXPECT_EQ(0, arena._sizeInBytes);
    }


    TEST_F(ManagedArenaTests, MoveAssign_MovesBufferIntoNewObject)
    {
        const auto initialPointer = arena._buffer;
        Arena<> arena2(256);

        arena2 = std::move(arena);

        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(0, arena2._offset);
        EXPECT_EQ(arenaSize, arena2._sizeInBytes);
        EXPECT_EQ(0, arena2._prevOffset);
    }


    TEST_F(ManagedArenaTests, MoveAssign_SelfAssignmentReturnsTheSameArena)
    {
        const auto initialAddress    = reinterpret_cast<uintptr_t>(arena._buffer);
        const auto initialOffset     = arena._offset;
        const auto initialPrevOffset = arena._prevOffset;
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wself-move"
#endif
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-move"
#endif
        arena = std::move(arena);
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

        EXPECT_EQ(initialAddress, reinterpret_cast<uintptr_t>(arena._buffer));
        EXPECT_EQ(initialOffset, arena._offset);
        EXPECT_EQ(initialPrevOffset, arena._prevOffset);
    }


    TEST_F(ManagedArenaTests, MoveAssign_DeletingOriginalArenaDoNotDeleteTheNewArenasMemory)
    {
        Arena<> arena2(256);
        constexpr auto scopedArenaSize = 512;

        // The arena being moved is scoped
        {
            Arena<> scopedArena(scopedArenaSize);
            arena2 = std::move(scopedArena);
        }
        EXPECT_NE(nullptr, arena2._buffer);

        // Write arbitrary data into the buffer
        // NOTE: i % 128 ensures that uint8_t does not overflow
        for (uint32_t i = 0; i < scopedArenaSize; ++i)
        {
            arena2._buffer[i] = i % 255;
        }

        // Read the value from buffer
        for (uint32_t i = 0; i < scopedArenaSize / 4; i += 4)
        {
            EXPECT_EQ(i % 255, arena2._buffer[i]);
        }
    }


    /**
     * @test Verify that when allocation buffer using allocBytes, prevOffset is
     *       moved by relative to the allocated object's size.
     */
    TEST_F(ManagedArenaTests, AllocBytes_MovesPrevOffset)
    {
        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(2, 2));

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        constexpr auto alignment           = 128;
        constexpr auto bufferSize          = 64;
        [[maybe_unused]] const auto buffer = arena.allocBytes(bufferSize, alignment);

        EXPECT_EQ(bufferSize, arena._offset - arena._prevOffset);
    }


    /**
     * @test Verify that when allocation buffer using alloc, prevOffset is
     *       moved by relative to the allocated object's size.
     */
    TEST_F(ManagedArenaTests, Alloc_MovesPrevOffset)
    {
        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(2, 2));

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        [[maybe_unused]] const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_EQ(sizeof(Vec4), arena._offset - arena._prevOffset);
    }


    TEST_F(ManagedArenaTests, Clear_ResetsOffsetToZero)
    {
        [[maybe_unused]] const auto chunkOne = arena.allocBytes(128);
        [[maybe_unused]] const auto chunkTwo = arena.allocBytes(128);

        // Initially expect offset and prevOffset are not zero
        EXPECT_NE(0, arena._offset);
        EXPECT_NE(0, arena._prevOffset);

        // After freeing, offsets are reset
        arena.clear();

        // Offsets are reset to zero
        EXPECT_EQ(0, arena._offset);
        EXPECT_EQ(0, arena._prevOffset);
    }


    TEST_F(ManagedArenaTests, Clear_OnlyResetsCurrentTelemetryUsage)
    {
        constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;

        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(byte1));
        static_cast<void>(arena.allocBytes(byte2));
        static_cast<void>(arena.allocBytes(byte3));

        constexpr std::size_t expectedMinUsage  = byte1;
        constexpr std::size_t expectedPeakUsage = byte3;

        arena.clear();

        EXPECT_EQ(expectedMinUsage, arena.getTelemetry().getMinUsage());
        EXPECT_EQ(expectedPeakUsage, arena.getTelemetry().getPeakUsage());
        EXPECT_EQ(0, arena.getTelemetry().getCurrentUsage());
    }


    TEST_F(ManagedArenaTests, Resize_LatestAllocationResizeBuffer)
    {
        constexpr auto byteSize    = 128;
        constexpr auto newByteSize = byteSize * 2;

        [[maybe_unused]] const auto firstByteChunk = arena.allocBytes(byteSize);
        auto secondByteChunk                       = arena.allocBytes(byteSize);
        const auto offsetBeforeResize              = arena._offset;

        [[maybe_unused]] const auto data = arena.resize(secondByteChunk, byteSize, newByteSize, alignof(void*));

        EXPECT_GT(arena._offset, offsetBeforeResize);
        EXPECT_EQ(reinterpret_cast<uintptr_t>(secondByteChunk), reinterpret_cast<uintptr_t>(data));
    }


    TEST_F(ManagedArenaTests, Resize_LatestAllocationOnlyResizeByOffsetDifference)
    {
        constexpr auto byteSize    = 128;
        constexpr auto newByteSize = byteSize * 2;

        [[maybe_unused]] const auto firstByteChunk = arena.allocBytes(byteSize);
        const auto secondByteChunk                 = arena.allocBytes(byteSize);
        const auto offsetBeforeResize              = arena._offset;
        const auto expectedOffset                  = offsetBeforeResize + (newByteSize - byteSize);

        [[maybe_unused]] const auto data = arena.resize(secondByteChunk, byteSize, newByteSize, alignof(void*));

        EXPECT_EQ(expectedOffset, arena._offset) << "Offset Mismatch";
    }




} // namespace pmm
