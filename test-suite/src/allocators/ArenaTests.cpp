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
 * @brief Move constructor moves the Arena to new variable and sets the old location to null.
 */
TEST(ArenaMove, MovesToNewVariable) {
    pmm::Arena arena(512);

    const pmm::Arena arena2 = std::move(arena);
    // arena.
}

/** @} */