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

#include <allocators/Arena.h>
#include <gtest/gtest.h>
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
    const pmm::Arena arena(512);
    EXPECT_EQ(512, arena.size());
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
}


// Namespacing is required for testing internal state
namespace pmm
{

    /** @brief Verify that the initializer with default alignment creates an offset for alignment. */
    TEST_P(AlignedArenaInitialization, InternalState_AlignBaseOffset)
    {
        constexpr auto size = 512;
        const auto alignment = GetParam();
        Arena arena(size, alignment);

        const auto baseAddress = reinterpret_cast<uintptr_t>(arena._buffer);
        const auto absoluteOffset = baseAddress + arena._offset;
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
        EXPECT_EQ(nullptr, arena._buffer);
        EXPECT_EQ(0, arena._offset);
        EXPECT_EQ(0, arena._prevOffset);
        EXPECT_EQ(0, arena._sizeInBytes);
        EXPECT_EQ(0, arena._defaultAlignment);
    }


    /**
     * @brief Verify that move constructor moves all data members, including buffer into new object.
     */
    TEST(ArenaMoveConstructor, MovesBufferIntoNewObject)
    {
        constexpr auto size = 512;
        Arena arena(size);
        const auto initialPointer = arena._buffer;
        const auto initialOffset = arena._offset;
        const auto initialPrevOffset = arena._prevOffset;
        const auto initialAlignment = arena._defaultAlignment;


        const Arena arena2 = std::move(arena);
        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(initialOffset, arena2._offset);
        EXPECT_EQ(initialPrevOffset, arena2._prevOffset);
        EXPECT_EQ(initialAlignment, arena2._defaultAlignment);
        EXPECT_EQ(size, arena2._sizeInBytes);
    }


    /**
     * @brief Verify that move constructor moves all data members, including buffer into new object.
     */
    TEST(ArenaMoveConstructor, AlignedArena_MovesBufferIntoNewObject)
    {
        constexpr auto size = 512;
        constexpr auto alignment = 128;
        Arena arena(size, alignment);
        const auto initialPointer = arena._buffer;
        const auto initialOffset = arena._offset;
        const auto initialPrevOffset = arena._prevOffset;


        const Arena arena2 = std::move(arena);
        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(initialOffset, arena2._offset);
        EXPECT_EQ(initialPrevOffset, arena2._prevOffset);
        EXPECT_EQ(alignment, arena2._defaultAlignment);
        EXPECT_EQ(size, arena2._sizeInBytes);
    }

} // namespace pmm


/**
 * @brief Verify that move assignment operator copies data members to new object.
 */
TEST(ArenaMoveAssignment, CopiesAttributesToNewObject)
{
    constexpr auto size = 512;
    pmm::Arena arena(size);
    pmm::Arena arena2(256);

    arena2 = std::move(arena);
    EXPECT_EQ(size, arena2.freeSize());
    EXPECT_EQ(size, arena2.size());
    EXPECT_EQ(0, arena2.usedSize());
}


// Namespacing is required for testing internal state
namespace pmm
{
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
        constexpr auto size = 512;
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
            firstAlloc[i] = static_cast<int>(i + 5);

        const auto secondAlloc = static_cast<int*>(arena.allocBytes(bufferLength * sizeof(int)));
        for (std::size_t i = 0; i < bufferLength; ++i)
            secondAlloc[i] = static_cast<int>(i + 7);

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
        arena.allocBytes(2, 2);

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        constexpr auto alignment = 128;
        constexpr auto bufferSize = 64;
        [[maybe_unused]] const auto buffer = arena.allocBytes(bufferSize, alignment);

        EXPECT_EQ(bufferSize, arena._offset - arena._prevOffset);
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
        arena.allocBytes(2, 2);

        constexpr auto expectedAlignment = alignof(Vec4);
        [[maybe_unused]] const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

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
        arena.allocBytes(2, 2);

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        [[maybe_unused]] const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_EQ(sizeof(Vec4), arena._offset - arena._prevOffset);
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
        arena.allocBytes(2, 2);

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        constexpr auto expectedAlignment = 128;
        [[maybe_unused]] const auto vec = arena.allocAs<Vec4>(expectedAlignment, 1.0f, 2.0f, 3.0f, 4.0f);

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
        arena.allocBytes(2, 2);

        // For testing using 128 byte alignment instead of the object's 16-byte natural alignment
        constexpr auto expectedAlignment = 128;
        [[maybe_unused]] const auto vec = arena.allocAs<Vec4>(expectedAlignment, 1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_EQ(sizeof(Vec4), arena._offset - arena._prevOffset);
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
        constexpr auto size = 512;
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
        const auto offsetBuffer = reinterpret_cast<uintptr_t>(arena._buffer) + arena._offset;
        const auto prevOffsetBuffer = reinterpret_cast<uintptr_t>(arena._buffer) + arena._prevOffset;
        EXPECT_EQ(0, offsetBuffer & (alignment - 1));
        EXPECT_EQ(0, prevOffsetBuffer & (alignment - 1));
        // Besides that equality, both offset and previous offset must be equal
        EXPECT_EQ(offsetBuffer, prevOffsetBuffer);
    }

} // namespace pmm


/** @} */
