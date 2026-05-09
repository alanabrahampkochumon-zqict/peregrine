/**
 * @file ArenaTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Verify Arena allocator logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <allocators/Arena.h>
#include <gtest/gtest.h>
#include <utility>
#include "Utils.h"


/**
 * @addtogroup T_PMM_Arena
 * @{
 */

/**
 * @brief Verify that Arena gets intialized with the correct size.
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
        EXPECT_EQ(0, arena._sizeInBytes);
    }


    /**
     * @brief Verify that move constructor moves all data members, including buffer into new object.
     */
    TEST(ArenaMoveConstructor, MovesBufferIntoNewObject)
    {
        constexpr auto size = 512;
        Arena arena(size);
        const auto initialPointer = arena._buffer;

        const Arena arena2 = std::move(arena);
        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(0, arena2._offset);
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

    /**
     * @brief Verify that allocBytes returns an 8-byte address by default on 64-bit machines.
     */
    TEST(ArenaAllocBytes, Returns8ByteAlignedAddress)
    {
        constexpr auto size = 512;
        Arena arena(size);

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



    TEST(ArenaAlloc, AllocatesAnObjectInTheArena)
    {
        constexpr auto size = 512;
        Arena arena(size);
        const auto vec = arena.alloc<Vec4>(1, 2, 3, 4);

        EXPECT_EQ(vec->x, 1);
        EXPECT_EQ(vec->y, 2);
        EXPECT_EQ(vec->z, 3);
        EXPECT_EQ(vec->w, 4);
    }

} // namespace pmm

// TODO: Add ArenaAlloc tests

/** @} */
