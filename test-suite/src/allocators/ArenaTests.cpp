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


TEST(ArenaInitialization, InitializesArenaWithTheGivenBytes)
{
    pmm::Arena arena(512);
    EXPECT_EQ(512, arena.size());
}


TEST(ArenaInitialization, ArenaHasZeroUsedSize)
{
    const pmm::Arena arena(512);
    EXPECT_EQ(0, arena.usedSize());
}


TEST(ArenaInitialization, ArenaHasFreeSpaceEqualToSize)
{
    const pmm::Arena arena(512);
    EXPECT_EQ(512, arena.freeSize());
}