/**
 * @file TempArenaTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: 2026
 *
 * @brief Verify @ref pmm::TempArena logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Utils.h"

#include <allocators/Arena.h>
#include <allocators/TempArena.h>
#include <gtest/gtest.h>


class TempArenaTest: public testing::Test
{
protected:
    std::size_t _arenaSize = 2048;
    pmm::Arena* _arena = nullptr;

    void SetUp() override
    {
        _arena = new pmm::Arena(_arenaSize);
    }
};


/**
 * @addtogroup T_PMM_TArena
 * @{
 */

/**
 * @brief Verify that temporary arena stores the internal state of the arena.
 */
TEST_F(TempArenaTest, SnapshotsArenaStateOnConstructor)
{
    constexpr auto firstAllocSize = 256, secondAllocSize = 128;

    // Allocate some memory in arena
    static_cast<void>(this->_arena->allocBytes(firstAllocSize));
    static_cast<void>(this->_arena->allocBytes(secondAllocSize));

    auto tArena = pmm::TempArena(this->_arena);
    EXPECT_EQ(firstAllocSize + secondAllocSize, tArena.currentOffset);
    EXPECT_EQ(firstAllocSize, tArena.prevOffset);
}


/**
 * @brief Verify that when a temporary arena goes out of scope, it "releases" any memory held.
 */
TEST_F(TempArenaTest, ArenaStateIsRestoredAfterTempArenaDeallocation)
{
    constexpr auto tArenaAllocSize = 256, alignment = 8;

    // Since we cannot access arena's internal pointer, we only way to ensure that the memory is freed
    // is to allocate some bytes close to the arena capacity and ensuring it doesn't return a nullptr
    {
        auto tArena = pmm::TempArena(this->_arena);
        static_cast<void>(tArena.arena->allocBytes(tArenaAllocSize));
    }

    // Subtracting 8 bytes of alignment just in case alignment results out-of-memory for arena
    void* memory = this->_arena->allocBytes(this->_arenaSize - alignment);

    EXPECT_NE(nullptr, memory);
}


/**
 * @brief Verify that freeing one temporary arena among multiple one restored the arena state.
 */
TEST_F(TempArenaTest, RewindsStateFreeingMemoryHeldByTempArena)
{
    constexpr auto firstTArenaAllocSize = 256, secondTArenaAllocSize = 512, alignment = 8;

    // Since we cannot access arena's internal pointer, we only way to ensure that the memory is freed
    // is to allocate some bytes close to the arena capacity and ensuring it doesn't return a nullptr
    {
        auto tArena1 = pmm::TempArena(this->_arena);
        static_cast<void>(tArena1.arena->allocBytes(firstTArenaAllocSize));
        {
            auto tArena2 = pmm::TempArena(this->_arena);
            static_cast<void>(tArena2.arena->allocBytes(secondTArenaAllocSize));
        }

        // Second arena should be free by now
        // So we could allocate arenaSize - firstArenaAllocSize bytes with a buffer of alignment
        // which shouldn't return a nullptr
        void* memory = tArena1.arena->allocBytes(this->_arenaSize - firstTArenaAllocSize - alignment);
        EXPECT_NE(nullptr, memory);
    }
}


/**
 * @brief Verify that @ref TempArena::allocBytes returns a non-nullptr in an arena with free space.
 */
TEST_F(TempArenaTest, AllocBytes_ReturnsNonNullValue)
{
    auto tArena1 = pmm::TempArena(this->_arena);
    const auto memory = tArena1.allocBytes(512);

    EXPECT_NE(nullptr, memory);
}


/**
 * @brief Verify that @ref TempArena::alloc returns a non-nullptr in an arena with free space.
 */
TEST_F(TempArenaTest, Alloc_ReturnsNonNullValue)
{
    auto tArena1 = pmm::TempArena(this->_arena);
    const auto memory = tArena1.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_NE(nullptr, memory);
    EXPECT_EQ(1.0f, memory->x);
    EXPECT_EQ(2.0f, memory->y);
    EXPECT_EQ(3.0f, memory->z);
    EXPECT_EQ(4.0f, memory->w);
}


/**
 * @brief Verify that @ref TempArena::allocAs returns a non-nullptr in an arena with free space.
 */
TEST_F(TempArenaTest, AllocAs_ReturnsNonNullValue)
{
    auto tArena1 = pmm::TempArena(this->_arena);
    const auto memory = tArena1.allocAs<Vec4>(32, 1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_NE(nullptr, memory);
    EXPECT_EQ(1.0f, memory->x);
    EXPECT_EQ(2.0f, memory->y);
    EXPECT_EQ(3.0f, memory->z);
    EXPECT_EQ(4.0f, memory->w);
}

/**
 * @brief Verify that @ref TempArena::allocV returns a non-empty span in an arena with free space.
 */
TEST_F(TempArenaTest, AllocV_ReturnsNonNullValue)
{
    auto tArena1 = pmm::TempArena(this->_arena);
    const auto memory = tArena1.allocV<Vec4>(10);

    EXPECT_EQ(10, memory.size());
}


/** @} */