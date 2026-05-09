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


/**
 * @addtogroup T_PMM_Arena
 * @{
 */

/**
 * @brief Verify that Arena gets intialized with the correct size.
 */
TEST(ArenaInitialization, InitializesArenaWithTheGivenBytes) {
    const pmm::Arena arena(512);
    EXPECT_EQ(512, arena.size());
}


/**
 * @brief Verify that usedSize is equal to zero size before any allocations.
 */
TEST(ArenaInitialization, ArenaHasZeroUsedSize) {
    const pmm::Arena arena(512);
    EXPECT_EQ(0, arena.usedSize());
}


/**
 * @brief Verify that freeSize is equal to Arena size before any allocations.
 */
TEST(ArenaInitialization, ArenaHasFreeSpaceEqualToSize) {
    const pmm::Arena arena(512);
    EXPECT_EQ(512, arena.freeSize());
}


/**
 * @brief Move constructor copies data members to new object.
 */
TEST(ArenaMoveConstructor, CopiesAttributesToNewObject) {
    constexpr auto size = 512;
    pmm::Arena arena(size);

    const pmm::Arena arena2 = std::move(arena);
    EXPECT_EQ(size, arena2.freeSize());
    EXPECT_EQ(size, arena2.size());
    EXPECT_EQ(0, arena2.usedSize());
}


// Namespacing is required for testing internal state
namespace pmm {

    /**
     * @brief Move constructor nulls out current object internal buffer and related data members.
     */
    TEST(ArenaMoveConstructor, NullsOutInternalBuffer) {
        constexpr auto size = 512;
        Arena arena(size);

        const Arena arena2 = std::move(arena);
        EXPECT_EQ(nullptr, arena._buffer);
        EXPECT_EQ(0, arena._offset);
        EXPECT_EQ(0, arena._sizeInBytes);
    }


    /**
     * @brief Move constructor moves all data members, including buffer into new object.
     */
    TEST(ArenaMoveConstructor, MovesBufferIntoNewObject) {
        constexpr auto size = 512;
        Arena arena(size);
        const auto initialPointer = arena._buffer;

        const Arena arena2 = std::move(arena);
        EXPECT_EQ(initialPointer, arena2._buffer);
        EXPECT_EQ(0, arena2._offset);
        EXPECT_EQ(size, arena2._sizeInBytes);
    }
}


/** @} */
