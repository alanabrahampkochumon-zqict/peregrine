/**
 * @file ArenaTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Verify Arena allocator logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Utils.h"

#include <gtest/gtest.h>
#include <peregrine/allocators/Arena.h>
#include <utility>


class AlignedArenaInitialization: public testing::TestWithParam<std::size_t>
{};
/** @brief Test fixture for @ref pmm::Arena initialization with an initial alignment. */
INSTANTIATE_TEST_SUITE_P(ArenaInitialization, AlignedArenaInitialization, testing::Values(2, 4, 8, 16, 32, 64, 128));



/**
 * @addtogroup T_PMM_Arena
 * @{
 */

/**
 * @brief Verify that Arena gets initialized with the correct size.
 */
TEST(ArenaInitialization, InitializesArenaWithTheGivenBytes)
{
    constexpr std::size_t arenaSize = 512;
    const pmm::Arena arena(arenaSize);
    EXPECT_EQ(arenaSize, arena.size());
    auto telemetry = arena.getTelemetry();

    EXPECT_EQ(arenaSize, telemetry.getArenaSize());
}

/**
 * @brief Verify that Arena gets initialized with the passed in telemetry instance.
 */
TEST(ArenaInitialization, InitializesArenaWithTheGivenTelemetryInstance)
{

    constexpr std::size_t arenaSize   = 512;
    constexpr std::size_t markerBytes = 213;

    pmm::ArenaTelemetry telemetry{ arenaSize };
    telemetry.updateAllocationUsage(markerBytes);

    const pmm::Arena arena(arenaSize, &telemetry);
    EXPECT_EQ(arenaSize, arena.size());

    EXPECT_EQ(markerBytes, arena.getTelemetry().getCurrentUsage());
}


/**
 * @brief Verify that Arena gets initialized with the passed in telemetry instance.
 */
TEST(ArenaInitialization, AlignedArena_InitializesArenaWithTheGivenTelemetryInstance)
{

    constexpr std::size_t arenaSize   = 512;
    constexpr std::size_t markerBytes = 213;
    constexpr std::size_t alignment   = 4;

    pmm::ArenaTelemetry telemetry{ arenaSize };
    telemetry.updateAllocationUsage(markerBytes);

    const pmm::Arena arena(arenaSize, alignment, &telemetry);
    EXPECT_EQ(arenaSize, arena.size());

    EXPECT_EQ(markerBytes, arena.getTelemetry().getCurrentUsage());
}


/**
 * @brief Verify that Arena does not copy the passed in telemetry instance.
 */
TEST(ArenaInitialization, DoesNotOwnAPassedInTelemetry)
{
    constexpr std::size_t arenaSize   = 512;
    constexpr std::size_t markerBytes = 213;

    pmm::ArenaTelemetry telemetry{ arenaSize };
    telemetry.updateAllocationUsage(markerBytes);

    const pmm::Arena arena(arenaSize, &telemetry);
    telemetry.updateAllocationUsage(markerBytes);

    // Since we can't directly verify the telemetry memory address
    // We can update the telemetry instance from outside, and it should get reflected in arena's instance
    EXPECT_EQ(2 * markerBytes, arena.getTelemetry().getCurrentUsage());
}


/**
 * @brief Verify that an aligned Arena does not copy the passed in telemetry instance.
 */
TEST(ArenaInitialization, AlignedArena_DoesNotOwnAPassedInTelemetry)
{
    constexpr std::size_t arenaSize   = 512;
    constexpr std::size_t markerBytes = 213;
    constexpr std::size_t alignment   = 4;

    pmm::ArenaTelemetry telemetry{ arenaSize };
    telemetry.updateAllocationUsage(markerBytes);

    const pmm::Arena arena(arenaSize, alignment, &telemetry);
    telemetry.updateAllocationUsage(markerBytes);

    // Since we can't directly verify the telemetry memory address
    // We can update the telemetry instance from outside, and it should get reflected in arena's instance
    EXPECT_EQ(2 * markerBytes, arena.getTelemetry().getCurrentUsage());
}


/**
 * @brief Verify that usedSize is equal to zero size before any allocations.
 */
TEST(ArenaInitialization, ArenaHasZeroUsedSize)
{
    const pmm::Arena arena(512);
    EXPECT_EQ(0, arena.usedSize());
}


/**
 * @brief Verify that freeSize is equal to Arena size before any allocations.
 */
TEST(ArenaInitialization, ArenaHasFreeSpaceEqualToSize)
{
    const pmm::Arena arena(512);
    EXPECT_EQ(512, arena.freeSize());
}


/**
 * @brief Verify that move constructor copies data members to new object.
 */
TEST(ArenaMoveConstructor, CopiesAttributesToNewObject)
{
    constexpr auto size = 512;
    pmm::Arena arena(size);

    const pmm::Arena arena2 = std::move(arena);
    EXPECT_EQ(size, arena2.freeSize());
    EXPECT_EQ(size, arena2.size());
    EXPECT_EQ(0, arena2.usedSize());
    EXPECT_EQ(size, arena2.getTelemetry().getArenaSize());
}


/**************************************
 *                                    *
 *            ALLOCV(ector)           *
 *                                    *
 **************************************/


/**
 * @brief Verify that @ref pmm::Arena::allocV allocates a block of memory.
 */
TEST(ArenaAllocV, ReturnsAContinguousBlockOfMemory)
{
    constexpr auto size       = 1024;
    constexpr auto blockCount = 10;
    pmm::Arena arena(size);

    const auto vertices = arena.allocV<Vec4>(blockCount);

    EXPECT_EQ(blockCount, vertices.size());
    EXPECT_EQ(blockCount * sizeof(Vec4), vertices.size_bytes());
}


/**
 * @brief Verify that @ref pmm::Arena::allocV returns an empty span when allocating in out-of-memory arena.
 */
TEST(ArenaAllocV, FullArenaReturnsEmptySpan)
{
    constexpr auto size       = 1024;
    constexpr auto blockCount = 10;
    pmm::Arena arena(size);
    static_cast<void>(arena.allocBytes(size - 1));

    const auto vertices = arena.allocV<Vec4>(blockCount);

    EXPECT_EQ(0, vertices.size());
    EXPECT_EQ(0, vertices.size_bytes());
}

/**
 * @brief Verify that the memory allocated with @ref pmm::Arena::allocV prevents memory overrides.
 */
TEST(ArenaAllocV, DataIsNotOverriden)
{
    constexpr auto size          = 1024;
    constexpr auto blockCount    = 5;
    constexpr float vertexData[] = {
        1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,  9.0f,  10.0f,
        11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f,
    };
    constexpr float edgeData[] = {
        101.0f, 102.0f, 103.0f,  104.0f, 105.0f, 106.0f, 107.0f,  108.0f, 109.0f, 110.0f,
        111.0f, 112.0f, 1013.0f, 114.0f, 115.0f, 116.0f, 1017.0f, 118.0f, 119.0f, 120.0f,
    };

    pmm::Arena arena(size);

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
    constexpr auto epsilon = 1e-5;
    for (std::size_t i = 0; i < blockCount; ++i)
    {
        const auto vert = vertices[i];
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

/**
 * @brief Verify that allocation using @ref pmm::Arena::allocV, updates the telemetry.
 */
TEST(ArenaAllocV, UpdatesTelemetry)
{
    constexpr std::size_t count1 = 2, count2 = 4, count3 = 6;
    constexpr auto size = 512;
    pmm::Arena arena(size);

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



// Namespacing is required for testing internal state
namespace pmm
{

    /**
     * @brief Verify that Arena's telemetry is owned by arena if telemetry is not passed-in.
     */
    TEST(ArenaInitialization, NoPassedInTelemetry_ArenaOwnsTelemetry)
    {
        constexpr auto size = 512;
        const Arena arena(size);

        EXPECT_TRUE(arena._ownedTelemetry);
    }


    /**
     * @brief Verify that Arena's telemetry is not owned by arena if telemetry is passed-in.
     */
    TEST(ArenaInitialization, PassedInTelemetry_ArenaDoesNotOwnTelemetry)
    {
        constexpr auto size = 512;
        ArenaTelemetry telemetry{ size };
        const Arena arena(size, &telemetry);

        EXPECT_FALSE(arena._ownedTelemetry);
    }


    /**
     * @brief Verify that Aligned Arena's telemetry is owned by arena if telemetry is not passed-in.
     */
    TEST(ArenaInitialization, AlignedArena_NoPassedInTelemetry_ArenaOwnsTelemetry)
    {
        constexpr auto size     = 512;
        constexpr int alignment = 4;
        const Arena arena(size, alignment);

        EXPECT_TRUE(arena._ownedTelemetry);
    }


    /**
     * @brief Verify that Aligned Arena's telemetry is not owned by arena if telemetry is passed-in.
     */
    TEST(ArenaInitialization, AlignedArena_PassedInTelemetry_ArenaDoesNotOwnTelemetry)
    {
        constexpr auto size      = 512;
        constexpr auto alignment = 4;
        ArenaTelemetry telemetry{ size };
        const Arena arena(size, alignment, &telemetry);

        EXPECT_FALSE(arena._ownedTelemetry);
    }



    /** @brief Verify that the initializer with default alignment creates an offset for alignment. */
    TEST_P(AlignedArenaInitialization, InternalState_AlignBaseOffset)
    {
        constexpr auto size  = 512;
        const auto alignment = GetParam();
        Arena arena(size, alignment);

        const auto baseAddress        = reinterpret_cast<uintptr_t>(arena._buffer);
        const auto absoluteOffset     = baseAddress + arena._offset;
        const auto absolutePrevOffset = baseAddress + arena._prevOffset;
        // Since the alignment is a power of two, we can extract the lower bits
        // to find out if the memory address is aligned
        // E.g: 1000 1000 -> 4 byte aligned since AND it with 0000 0111(31) will result 0
        // If misaligned 1000 1001 -> not 4 byte aligned since the op will result in 0000 0001
        EXPECT_TRUE((absoluteOffset & (alignment - 1)) == 0);
        EXPECT_TRUE((absolutePrevOffset & (alignment - 1)) == 0);
    }


    /**
     * @brief Verify that move constructor nulls out current object internal buffer and related data members.
     */
    TEST(ArenaMoveConstructor, NullsOutInternalBuffer)
    {
        constexpr auto size = 512;
        Arena arena(size);

        const Arena arena2 = std::move(arena);
        // NOLINT(bugprone-use-after-move)
        EXPECT_EQ(nullptr, arena._buffer);
        EXPECT_EQ(0, arena._offset);
        EXPECT_EQ(0, arena._prevOffset);
        EXPECT_EQ(0, arena._sizeInBytes);
        EXPECT_EQ(0, arena._defaultAlignment);
        EXPECT_FALSE(arena._ownedTelemetry);
        // Since, we are dereferencing a nullptr internally, it will cause SEH
        // UB
        // EXPECT_DEATH(static_cast<void>(arena.getTelemetry()), "");
    }


    /**
     * @brief Verify that move constructor moves all data members, including buffer into new object.
     */
    TEST(ArenaMoveConstructor, MovesBufferIntoNewObject)
    {
        constexpr auto size = 512;
        Arena arena(size);
        const auto initialPointer            = arena._buffer;
        const auto initialOffset             = arena._offset;
        const auto initialPrevOffset         = arena._prevOffset;
        const auto initialAlignment          = arena._defaultAlignment;
        const auto initialTelemetry          = arena.getTelemetry();
        const auto initialTelemetryOwnership = arena._ownedTelemetry;

        const Arena arena2 = std::move(arena);
        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(initialOffset, arena2._offset);
        EXPECT_EQ(initialPrevOffset, arena2._prevOffset);
        EXPECT_EQ(initialAlignment, arena2._defaultAlignment);
        EXPECT_EQ(size, arena2._sizeInBytes);
        EXPECT_EQ(initialTelemetryOwnership, arena2._ownedTelemetry);

        // Checking for telemetry equality
        EXPECT_EQ(initialTelemetry.getCurrentUsage(), arena2.getTelemetry().getCurrentUsage());
        EXPECT_EQ(initialTelemetry.getPeakUsage(), arena2.getTelemetry().getPeakUsage());
        EXPECT_EQ(initialTelemetry.getArenaSize(), arena2.getTelemetry().getArenaSize());
        EXPECT_EQ(initialTelemetry.getMinUsage(), arena2.getTelemetry().getMinUsage());
    }


    /**
     * @brief Verify that move constructor moves the telemetry object(ptr).
     */
    TEST(ArenaMoveConstructor, MovesTelemetry)
    {
        constexpr auto size = 512;
        ArenaTelemetry telemetry{ size };
        telemetry.updateAllocationUsage(512);
        telemetry.updateAllocationUsage(256);
        Arena arena(size, &telemetry);

        const Arena arena2 = std::move(arena);

        // Update telemetry usage from outside
        // Since the arena class only holds a ptr
        // Updating the telemetry should reflect the change in the telemetry held by arena2
        telemetry.updateAllocationUsage(128);

        // Checking for telemetry equality
        EXPECT_EQ(telemetry.getCurrentUsage(), arena2.getTelemetry().getCurrentUsage());
        EXPECT_EQ(telemetry.getPeakUsage(), arena2.getTelemetry().getPeakUsage());
        EXPECT_EQ(telemetry.getArenaSize(), arena2.getTelemetry().getArenaSize());
        EXPECT_EQ(telemetry.getMinUsage(), arena2.getTelemetry().getMinUsage());
    }


    /**
     * @brief Verify that move constructor moves all data members of an aligned arena, including buffer into new object.
     */
    TEST(ArenaMoveConstructor, AlignedArena_MovesBufferIntoNewObject)
    {
        constexpr auto size      = 512;
        constexpr auto alignment = 128;
        Arena arena(size, alignment);
        const auto initialPointer            = arena._buffer;
        const auto initialOffset             = arena._offset;
        const auto initialPrevOffset         = arena._prevOffset;
        const auto initialTelemetryOwnership = arena._ownedTelemetry;


        const Arena arena2 = std::move(arena);
        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(initialOffset, arena2._offset);
        EXPECT_EQ(initialPrevOffset, arena2._prevOffset);
        EXPECT_EQ(alignment, arena2._defaultAlignment);
        EXPECT_EQ(size, arena2._sizeInBytes);
        EXPECT_EQ(initialTelemetryOwnership, arena2._ownedTelemetry);
    }


    /**
     * @brief Verify that move constructor moves the telemetry object(ptr) of aligned arena.
     */
    TEST(ArenaMoveConstructor, AlignedArena_MovesTelemetry)
    {
        constexpr auto size      = 512;
        constexpr auto alignment = 128;
        ArenaTelemetry telemetry{ size };
        telemetry.updateAllocationUsage(512);
        telemetry.updateAllocationUsage(256);
        Arena arena(size, alignment, &telemetry);

        const Arena arena2 = std::move(arena);

        // Update telemetry usage from outside
        // Since the arena class only holds a ptr
        // Updating the telemetry should reflect the change in the telemetry held by arena2
        telemetry.updateAllocationUsage(128);

        // Checking for telemetry equality
        EXPECT_EQ(telemetry.getCurrentUsage(), arena2.getTelemetry().getCurrentUsage());
        EXPECT_EQ(telemetry.getPeakUsage(), arena2.getTelemetry().getPeakUsage());
        EXPECT_EQ(telemetry.getArenaSize(), arena2.getTelemetry().getArenaSize());
        EXPECT_EQ(telemetry.getMinUsage(), arena2.getTelemetry().getMinUsage());
    }

} // namespace pmm


/**
 * @brief Verify that move assignment operator copies data members to new object.
 */
TEST(ArenaMoveAssignment, CopiesAttributesToNewObject)
{
    constexpr auto sampleAllocation = 50;
    constexpr auto size             = 512;
    pmm::Arena arena(size);
    static_cast<void>(arena.allocBytes(sampleAllocation));
    pmm::Arena arena2(256);

    arena2 = std::move(arena);
    EXPECT_EQ(size - sampleAllocation, arena2.freeSize());
    EXPECT_EQ(size, arena2.size());
    EXPECT_EQ(sampleAllocation, arena2.usedSize());

    EXPECT_EQ(size, arena2.getTelemetry().getArenaSize());
    EXPECT_EQ(sampleAllocation, arena2.getTelemetry().getCurrentUsage());
    EXPECT_EQ(sampleAllocation, arena2.getTelemetry().getPeakUsage());
    EXPECT_EQ(sampleAllocation, arena2.getTelemetry().getMinUsage());
}


// Namespacing is required for testing internal state
namespace pmm
{

    /**************************************
     *                                    *
     *          MOVE ASSIGNMENT           *
     *                                    *
     **************************************/

    /**
     * @brief Verify that move assignment operator nulls out current object internal buffer and related data members.
     */
    TEST(ArenaMoveAssignment, NullsOutInternalBuffer)
    {
        constexpr auto size = 512;
        Arena arena(size);
        [[maybe_unused]] Arena arena2(256);

        static_cast<void>(arena2 = std::move(arena));
        EXPECT_EQ(nullptr, arena._buffer);
        EXPECT_EQ(0, arena._offset);
        EXPECT_EQ(0, arena._prevOffset);
        EXPECT_EQ(0, arena._sizeInBytes);
    }


    /**
     * @brief Verify that move assignment operator moves all data members, including buffer into new object.
     */
    TEST(ArenaMoveAssignment, MovesBufferIntoNewObject)
    {
        constexpr auto size = 512;
        Arena arena(size);
        const auto initialPointer = arena._buffer;
        Arena arena2(256);

        arena2 = std::move(arena);

        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(0, arena2._offset);
        EXPECT_EQ(size, arena2._sizeInBytes);
        EXPECT_EQ(0, arena2._prevOffset);
    }


    /**
     * @brief Verify that move assignment within self returns the same arena. */
    TEST(ArenaMoveAssignment, SelfAssignmentReturnsTheSameArena)
    {
        constexpr auto size = 512;
        Arena arena(size);
        const auto initialAddress    = reinterpret_cast<uintptr_t>(arena._buffer);
        const auto initialOffset     = arena._offset;
        const auto initialPrevOffset = arena._prevOffset;
        const auto initialAlignment  = arena._defaultAlignment;

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
        EXPECT_EQ(initialAlignment, arena._defaultAlignment);
    }


    /**
     * @brief Verify that move assignment operator moves all data members, including buffer into new object.
     */
    TEST(ArenaMoveAssignment, DeletingOriginalArenaDoNotDeleteTheNewArenasMemory)
    {
        Arena arena2(256);
        constexpr auto size = 512;

        {
            Arena arena(size);
            arena2 = std::move(arena);
        }
        EXPECT_NE(nullptr, arena2._buffer);

        // Write arbitrary data into the buffer
        // NOTE: i % 128 ensures that uint8_t does not overflow
        for (uint32_t i = 0; i < size; ++i)
        {
            arena2._buffer[i] = i % 255;
        }

        // Read the value from buffer
        for (uint32_t i = 0; i < size / 4; i += 4)
        {
            EXPECT_EQ(i % 255, arena2._buffer[i]);
        }
    }


    /**************************************
     *                                    *
     *            ALLOC BYTES             *
     *                                    *
     **************************************/

    /**
     * @brief Verify that allocBytes returns an 8-byte address by default on 64-bit machines.
     */
    TEST(ArenaAllocBytes, Returns8ByteAlignedAddress)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Misaligned bytes to 2
        [[maybe_unused]] void* misalignedBytes = arena.allocBytes(2, 2);

        void* bytes = arena.allocBytes(8);

        const auto address = reinterpret_cast<uintptr_t>(bytes);
        EXPECT_EQ(0, address % sizeof(void*));
    }


    /**
     * @brief Verify that allocBytes returns byte aligned address specified by the parameter.
     */
    TEST(ArenaAllocBytes, ReturnsProvidedByteAlignedAddress)
    {
        constexpr auto size          = 512;
        constexpr auto byteAlignment = 32;
        Arena arena(size);

        void* bytes = arena.allocBytes(128, byteAlignment);

        const auto address = reinterpret_cast<uintptr_t>(bytes);
        EXPECT_EQ(0, address % byteAlignment);
    }

    /**
     * @brief Verify that allocBytes return non-null address when allocate size less than the total size of the arena.
     */
    TEST(ArenaAllocBytes, ReturnsNonNullPtrWhenAllocatingMemoryLessThanArenaSize)
    {
        constexpr auto size = 512;
        Arena arena(size);

        void* bytes = arena.allocBytes(256);

        EXPECT_NE(nullptr, bytes);
    }


    /**
     * @brief Arena allocBytes return non-null address when allocating size equal to total size of the arena.
     */
    TEST(ArenaAllocBytes, ReturnsNonNullPtrWhenAllocatingMemoryEqualArenaSize)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // To ensure test integrity we need to subtract max alignment of 7, which
        // the maximum offset will be moved when aligning.
        void* bytes = arena.allocBytes(size - 7);

        EXPECT_NE(nullptr, bytes);
    }

    /**
     * @brief Arena allocBytes return `nullptr` when allocating size greater than total size of the arena.
     */
    TEST(ArenaAllocBytes, ReturnsNullPtrWhenAllocatingMemoryGreaterThanArenaSize)
    {
        constexpr auto size = 512;
        Arena arena(size);

        void* bytes = arena.allocBytes(size + 1);

        EXPECT_EQ(nullptr, bytes);
    }

    /**
     * @brief Verify that allocByte writing to two contiguous block of memory do not overlap.
     */
    TEST(ArenaAllocBytes, ReadWritesDoNotOverlap)
    {
        constexpr auto size = 512;
        Arena arena(size);

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

    /**
     * @brief Verify that after allocation using @ref pmm::Arena::alloc, the difference between
     *        prevOffset and offset is the size of the object.
     */
    TEST(ArenaAllocBytes, OffsetMinusPrevOffsetGivesObjectSize)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(2, 2));

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        constexpr auto alignment           = 128;
        constexpr auto bufferSize          = 64;
        [[maybe_unused]] const auto buffer = arena.allocBytes(bufferSize, alignment);

        EXPECT_EQ(bufferSize, arena._offset - arena._prevOffset);
    }



    /**
     * @brief Verify that allocation using @ref pmm::Arena::allocBytes, updates the telemetry.
     */
    TEST(ArenaAllocBytes, UpdatesTelemetry)
    {
        constexpr auto size         = 512;
        constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;
        Arena arena(size);

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
     *                                    *
     *              ALLOC                 *
     *                                    *
     **************************************/

    /** @brief Verify that @ref pmm::Arena::alloc allocates an object in the arena with correct RAII. */
    TEST(ArenaAlloc, AllocatesAnObjectInTheArena)
    {
        constexpr auto size = 512;
        Arena arena(size);
        const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_FLOAT_EQ(1.0f, vec->x);
        EXPECT_FLOAT_EQ(2.0f, vec->y);
        EXPECT_FLOAT_EQ(3.0f, vec->z);
        EXPECT_FLOAT_EQ(4.0f, vec->w);
    }


    /** @brief Verify that @ref pmm::Arena::alloc returns a nullptr when the arena is full. */
    TEST(ArenaAlloc, ReturnsNullPtrWhenAllocatingInAFullArena)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Use the full capacity
        [[maybe_unused]] const auto fullSize = arena.allocBytes(size);

        const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_EQ(nullptr, vec);
    }


    /** @brief Verify that Arena's object allocation default to the alignment of object's T. */
    TEST(ArenaAlloc, AlignsToTargetAlignment)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(2, 2));

        constexpr auto expectedAlignment = alignof(Vec4);
        [[maybe_unused]] const auto vec  = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

        // Arena is aligned to alignment of vec4 which is 16 bytes.
        // Checking previous offset is required since current offset is will not be aligned to the boundary
        // but the base address.
        EXPECT_TRUE(((reinterpret_cast<uintptr_t>(arena._buffer) + arena._prevOffset) & (expectedAlignment - 1)) == 0);
    }

    /**
     * @brief Verify that after allocation using @ref pmm::Arena::alloc, the difference between
     *        prevOffset and offset is the size of the object.
     */
    TEST(ArenaAlloc, OffsetMinusPrevOffsetGivesObjectSize)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(2, 2));

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        [[maybe_unused]] const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_EQ(sizeof(Vec4), arena._offset - arena._prevOffset);
    }


    /**
     * @brief Verify that allocation using @ref pmm::Arena::alloc, updates the telemetry.
     */
    TEST(ArenaAlloc, UpdatesTelemetry)
    {
        constexpr auto size = 512;
        Arena arena(size);

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
     *                                    *
     *             ALLOC AS               *
     *                                    *
     **************************************/

    /** @brief Verify that @ref pmm::Arena::allocAs allocates an object in the arena with correct RAII. */
    TEST(ArenaAllocAs, AllocatesAnObjectInTheArena)
    {
        constexpr auto size = 512;
        Arena arena(size);
        const auto vec = arena.allocAs<Vec4>(alignof(Vec4), 1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_FLOAT_EQ(1.0f, vec->x);
        EXPECT_FLOAT_EQ(2.0f, vec->y);
        EXPECT_FLOAT_EQ(3.0f, vec->z);
        EXPECT_FLOAT_EQ(4.0f, vec->w);
    }


    /** @brief Verify that @ref pmm::Arena::allocAs returns a nullptr when the arena is full. */
    TEST(ArenaAllocAs, ReturnsNullPtrWhenAllocatingInAFullArena)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Use the full capacity
        [[maybe_unused]] const auto fullSize = arena.allocBytes(size);

        const auto vec = arena.allocAs<Vec4>(alignof(Vec4), 1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_EQ(nullptr, vec);
    }


    /** @brief Verify that Arena's object allocation aligns to passed-in alignment. */
    TEST(ArenaAllocAs, AlignsToGivenAlignment)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(2, 2));

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        constexpr auto expectedAlignment = 128;
        [[maybe_unused]] const auto vec  = arena.allocAs<Vec4>(expectedAlignment, 1.0f, 2.0f, 3.0f, 4.0f);

        // Arena is aligned to alignment of 128 bytes.
        // Checking previous offset is required since current offset is will not be aligned to the boundary
        // but the base address.
        EXPECT_TRUE(((reinterpret_cast<uintptr_t>(arena._buffer) + arena._prevOffset) & (expectedAlignment - 1)) == 0);
    }

    /**
     * @brief Verify that after allocation using @ref pmm::Arena::allocAs, the difference between
     *        prevOffset and offset is the size of the object.
     */
    TEST(ArenaAllocAs, OffsetMinusPrevOffsetGivesObjectSize)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(2, 2));

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        constexpr auto expectedAlignment = 128;
        [[maybe_unused]] const auto vec  = arena.allocAs<Vec4>(expectedAlignment, 1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_EQ(sizeof(Vec4), arena._offset - arena._prevOffset);
    }

    /**
     * @brief Verify that allocation using @ref pmm::Arena::alloc, updates the telemetry.
     */
    TEST(ArenaAllocAs, UpdatesTelemetry)
    {
        constexpr auto size = 512;
        Arena arena(size);

        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocAs<Vec4>(4, 1.0f, 2.0f, 3.0f, 4.0f));
        static_cast<void>(arena.allocAs<Vec4>(4, 1.0f, 2.0f, 3.0f, 4.0f));
        static_cast<void>(arena.allocAs<Vec4>(4, 1.0f, 2.0f, 3.0f, 4.0f));
        static_cast<void>(arena.allocAs<int>(4, 1));

        constexpr std::size_t expectedMinUsage  = sizeof(int);
        constexpr std::size_t expectedPeakUsage = sizeof(Vec4);
        constexpr std::size_t expectedUsage     = sizeof(Vec4) * 3 + sizeof(int);

        EXPECT_EQ(expectedMinUsage, arena.getTelemetry().getMinUsage());
        EXPECT_EQ(expectedPeakUsage, arena.getTelemetry().getPeakUsage());
        EXPECT_EQ(expectedUsage, arena.getTelemetry().getCurrentUsage());
    }


    /**************************************
     *                                    *
     *             FREE ALL               *
     *                                    *
     **************************************/

    /** @brief Verify that freeing an unaligned arena resets the offsets to zero. */
    TEST(ArenaFreeAll, ResetsOffsetToZero)
    {
        constexpr auto size = 512;
        Arena arena(size);

        [[maybe_unused]] const auto chunkOne = arena.allocBytes(128);
        [[maybe_unused]] const auto chunkTwo = arena.allocBytes(128);

        // Initially expect offset and prevOffset are not zero
        EXPECT_NE(0, arena._offset);
        EXPECT_NE(0, arena._prevOffset);

        // After freeing, offsets are reset
        arena.freeAll();

        // Offsets are reset to zero
        EXPECT_EQ(0, arena._offset);
        EXPECT_EQ(0, arena._prevOffset);
    }


    /** @brief Verify that freeing an aligned arena resets the offsets to an aligned address. */
    TEST(ArenaFreeAll, AlignedArena_ResetsOffsetToAlignedAddress)
    {
        constexpr auto size      = 512;
        constexpr auto alignment = 128;
        Arena arena(size, alignment);

        // 64 byte allocation proves to use in the latter phases that allocation was successful
        // since 128 aligned address + 64 bytes will not result in a 128 byte alignment.
        [[maybe_unused]] const auto bytes = arena.allocBytes(64);

        // Initially, expect offset to not equal previous offset after allocation
        EXPECT_NE(arena._prevOffset, arena._offset);

        // After freeing, offsets are reset
        arena.freeAll();

        // Offsets are reset to zero
        // Modulo arithmetic.
        const auto offsetBuffer     = reinterpret_cast<uintptr_t>(arena._buffer) + arena._offset;
        const auto prevOffsetBuffer = reinterpret_cast<uintptr_t>(arena._buffer) + arena._prevOffset;
        EXPECT_EQ(0, offsetBuffer & (alignment - 1));
        EXPECT_EQ(0, prevOffsetBuffer & (alignment - 1));
        // Besides that equality, both offset and previous offset must be equal
        EXPECT_EQ(offsetBuffer, prevOffsetBuffer);
    }

    /**
     * @brief Verify that freeing an unaligned arena resets ONLY the current usage.
     */
    TEST(ArenaFreeAll, OnlyResetsCurrentTelemetryUsage)
    {
        constexpr auto size         = 512;
        constexpr std::size_t byte1 = 20, byte2 = 56, byte3 = 128;
        Arena arena(size);

        // Allocate a 2 byte alignment forcing a misalignment to 2 bytes
        static_cast<void>(arena.allocBytes(byte1));
        static_cast<void>(arena.allocBytes(byte2));
        static_cast<void>(arena.allocBytes(byte3));

        constexpr std::size_t expectedMinUsage  = byte1;
        constexpr std::size_t expectedPeakUsage = byte3;

        arena.freeAll();

        EXPECT_EQ(expectedMinUsage, arena.getTelemetry().getMinUsage());
        EXPECT_EQ(expectedPeakUsage, arena.getTelemetry().getPeakUsage());
        EXPECT_EQ(0, arena.getTelemetry().getCurrentUsage());
    }


    /**************************************
     *                                    *
     *       RESIZE (INTERNAL STATE)      *
     *                                    *
     **************************************/

    /** @brief Verify that arena resize with last allocated buffer only resizes the arena. */
    TEST(ArenaResize, LatestAllocationResizeBuffer)
    {
        constexpr auto arenaSize = 1024;
        Arena arena(arenaSize);
        constexpr auto byteSize    = 128;
        constexpr auto newByteSize = byteSize * 2;

        [[maybe_unused]] const auto firstByteChunk = arena.allocBytes(byteSize);
        auto secondByteChunk                       = arena.allocBytes(byteSize);
        const auto offsetBeforeResize              = arena._offset;

        [[maybe_unused]] const auto data = arena.resize(secondByteChunk, byteSize, newByteSize, alignof(void*));

        EXPECT_GT(arena._offset, offsetBeforeResize);
        EXPECT_EQ(reinterpret_cast<uintptr_t>(secondByteChunk), reinterpret_cast<uintptr_t>(data));
    }


    /** @brief Verify that arena resize with last allocated buffer resizes only by the size difference. */
    TEST(ArenaResize, LatestAllocationOnlyResizeByOffsetDifference)
    {
        constexpr auto arenaSize = 1024;
        Arena arena(arenaSize);
        constexpr auto byteSize    = 128;
        constexpr auto newByteSize = byteSize * 2;

        [[maybe_unused]] const auto firstByteChunk = arena.allocBytes(byteSize);
        const auto secondByteChunk                 = arena.allocBytes(byteSize);
        const auto offsetBeforeResize              = arena._offset;
        const auto expectedOffset                  = offsetBeforeResize + (newByteSize - byteSize);

        [[maybe_unused]] const auto data = arena.resize(secondByteChunk, byteSize, newByteSize, alignof(void*));

        EXPECT_EQ(expectedOffset, arena._offset) << "Offset Mismatch";
    }


    /** @brief Verify that arena resize with nullptr updates the telemetry with freshly allocated byte usage. */
    TEST(ArenaResize, NoOldMemory_UpdatesTelemetry)
    {
        constexpr auto arenaSize = 1024;
        Arena arena(arenaSize);
        constexpr auto byteSize = 128;


        [[maybe_unused]] const auto data = arena.resize(nullptr, byteSize, byteSize, alignof(void*));

        EXPECT_EQ(byteSize, arena.getTelemetry().getCurrentUsage());
        EXPECT_EQ(byteSize, arena.getTelemetry().getMinUsage());
        EXPECT_EQ(byteSize, arena.getTelemetry().getPeakUsage());
    }

    /** @brief Verify that arena resize with same size, does not update telemetry. */
    TEST(ArenaResize, SameMemorySize_DoesNotUpdateTelemetry)
    {
        constexpr auto arenaSize = 1024;
        Arena arena(arenaSize);
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


    /** @brief Verify that arena resize with same size, does not update telemetry. */
    TEST(ArenaResize, SmallerMemorySize_DoesNotUpdateTelemetry)
    {
        constexpr auto arenaSize = 1024;
        Arena arena(arenaSize);
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

    /** @brief Verify that arena resize with larger size of final allocation, update telemetry by size difference. */
    TEST(ArenaResize, LatestAllocationResize_UpdatesTelemetry)
    {
        constexpr auto arenaSize = 1024;
        Arena arena(arenaSize);
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

    /** @brief Verify that arena resize with larger size of in-between allocation, update telemetry by size difference.
     */
    TEST(ArenaResize, InBetweenAllocationResize_UpdatesTelemetry)
    {
        constexpr auto arenaSize = 1024;
        Arena arena(arenaSize);
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

} // namespace pmm


/**************************************
 *                                    *
 *             RESIZE                 *
 *                                    *
 **************************************/

/** @brief Verify that arena resize on a nullptr returns a new allocation. */
TEST(ArenaResize, NullptrReturnsNewLocation)
{
    constexpr auto arenaSize = 1024;
    pmm::Arena arena(arenaSize);

    constexpr auto byteSize = 128;
    const auto data         = arena.resize(nullptr, 0, byteSize, alignof(void*));

    EXPECT_NE(nullptr, data);
}


/** @brief Verify that arena resize on a nullptr allocation beyond size of arena returns a nullptr. */
TEST(ArenaResize, NullptrResizeBeyondArenaSizeReturnsNullptr)
{
    constexpr auto arenaSize = 1024;
    pmm::Arena arena(arenaSize);

    constexpr auto byteSize = arenaSize + 1;
    const auto data         = arena.resize(nullptr, 0, byteSize, alignof(void*));

    EXPECT_EQ(nullptr, data);
}


/** @brief Verify that arena resize on a 0 size allocation returns a new allocation. */
TEST(ArenaResize, ZeroSizeReturnsNewLocation)
{
    constexpr auto arenaSize = 1024;
    pmm::Arena arena(arenaSize);

    constexpr auto byteSize = 128;
    const auto data         = arena.resize(nullptr, 0, byteSize, alignof(void*));

    EXPECT_NE(nullptr, data);
}


/** @brief Verify that arena resize on a nullptr returns a new location with read-write access. */
TEST(ArenaResize, NullptrAllocatesMemoryWithReadWrite)
{
    constexpr auto arenaSize = 1024;
    pmm::Arena arena(arenaSize);

    constexpr auto byteSize  = 128;
    constexpr auto arraySize = 128 / sizeof(int);
    const auto data          = static_cast<int*>(arena.resize(nullptr, 0, byteSize, alignof(int)));

    for (std::size_t i = 0; i < arraySize; ++i)
    {
        data[i] = static_cast<int>(i + 100);
    }
    for (std::size_t i = 0; i < arraySize; ++i)
    {
        EXPECT_EQ(i + 100, data[i]);
    }
}


/** @brief Verify that arena resize with new size less than old size returns the same address. */
TEST(ArenaResize, NewSizeLowerThanOldSizeReturnsSameLocation)
{
    constexpr auto arenaSize = 1024;
    pmm::Arena arena(arenaSize);
    constexpr auto byteSize = 128;

    const auto firstByteChunk = arena.allocBytes(byteSize);
    // Additional allocation
    [[maybe_unused]] const auto secondByteChunk = arena.allocBytes(byteSize);

    const auto data = arena.resize(firstByteChunk, byteSize, byteSize / 2, alignof(void*));

    EXPECT_EQ(reinterpret_cast<uintptr_t>(firstByteChunk), reinterpret_cast<uintptr_t>(data));
}


/** @brief Verify that arena resize with last allocated returns writable memory after resize. */
TEST(ArenaResize, LatestAllocationOnlyResizeByOffsetDifference)
{
    constexpr auto arenaSize = 1024;
    pmm::Arena arena(arenaSize);
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


/** @brief Verify that arena resize with allocation in the middle returns new memory. */
TEST(ArenaResize, AllocationBeforePriorAllocationReturnNewBuffer)
{
    constexpr auto arenaSize = 1024;
    pmm::Arena arena(arenaSize);
    constexpr auto byteSize    = 128;
    constexpr auto newByteSize = byteSize * 2;


    const auto firstByteChunk                   = arena.allocBytes(byteSize);
    [[maybe_unused]] const auto secondByteChunk = arena.allocBytes(byteSize);

    [[maybe_unused]] const auto data = arena.resize(firstByteChunk, byteSize, newByteSize, alignof(void*));

    EXPECT_NE(reinterpret_cast<uintptr_t>(firstByteChunk), reinterpret_cast<uintptr_t>(data));
}


/** @brief Verify that arena resize with allocation in the middle copies old data. */
TEST(ArenaResize, AllocationBeforePriorAllocationCopiesOldData)
{
    constexpr auto arenaSize = 1024;
    pmm::Arena arena(arenaSize);
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



/** @} */
