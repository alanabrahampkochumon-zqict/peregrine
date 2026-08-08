/**
 * @file DeathTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 7, 2026
 *
 * @brief Verify assertion for fgm::Arena allocation in debug mode.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/allocators/Arena.h>

#ifdef ENABLE_PMM_DEATH_TESTS
namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/
    /// @brief Test Fixture for @ref pmm::Arena<Managed> debug assertions.
    class ManagedArenaDeathTests: public testing::Test
    {
    public:
        size_t arenaSize{ 4096 };
        pmm::Arena<> arena{ arenaSize };
    };


} // namespace



/**************************************
 *           MANAGED ARENA            *
 **************************************/

TEST_F(ManagedArenaDeathTests, AllocBytes_ZeroSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(0)), ""); }


TEST_F(ManagedArenaDeathTests, AllocBytes_SizeGreaterThanArenaSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(arenaSize + 1)), ""); }


// TEST_F(ManagedArenaDeathTests, Alloc_ZeroSize_TriggersAssertionInDebugMode)
// { EXPECT_DEBUG_DEATH(static_cast<void>(arena.alloc<nullptr>()), ""); }
//
//
// TEST_F(ManagedArenaDeathTests, Alloc_SizeGreaterThanArenaSize_TriggersAssertionInDebugMode)
// { EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(arenaSize + 1)), ""); }

// TODO: Alignment ne to multiple of 1

#endif
// 1. Nullptr to alloc > size
// 1. Nullptr to alloc
/** @brief Verify that arena resize on a nullptr returns a new allocation. */
// TODO: To assertion
// TEST_F(ManagedArenaTests, ResizeNullptrReturnsNewLocation)
// {
//     constexpr auto arenaSize = 1024;
//     pmm::Arena arena(arenaSize);
//
//     constexpr auto byteSize = 128;
//     const auto data         = arena.resize(nullptr, 0, byteSize, alignof(void*));
//
//     EXPECT_NE(nullptr, data);
// }
// Assertion + add to safetype
/** @brief Verify that arena resize on a nullptr allocation beyond size of arena returns a nullptr. */
// TEST(ArenaResize, NullptrResizeBeyondArenaSizeReturnsNullptr)
// {
//     constexpr auto arenaSize = 1024;
//     pmm::Arena arena(arenaSize);
//
//     constexpr auto byteSize = arenaSize + 1;
//     const auto data         = arena.resize(nullptr, 0, byteSize, alignof(void*));
//
//     EXPECT_EQ(nullptr, data);
// }

// /** @brief Verify that arena resize on a 0 size allocation returns a new allocation. */
// TEST(ArenaResize, ZeroSizeReturnsNewLocation)
// {
//     constexpr auto arenaSize = 1024;
//     pmm::Arena arena(arenaSize);
//
//     constexpr auto byteSize = 128;
//     const auto data         = arena.resize(nullptr, 0, byteSize, alignof(void*));
//
//     EXPECT_NE(nullptr, data);
// }
// /** @brief Verify that arena resize on a nullptr returns a new location with read-write access. */
// TEST(ArenaResize, NullptrAllocatesMemoryWithReadWrite)
// {
//     constexpr auto arenaSize = 1024;
//     pmm::Arena arena(arenaSize);
//
//     constexpr auto byteSize  = 128;
//     constexpr auto arraySize = 128 / sizeof(int);
//     const auto data          = static_cast<int*>(arena.resize(nullptr, 0, byteSize, alignof(int)));
//
//     for (std::size_t i = 0; i < arraySize; ++i)
//     {
//         data[i] = static_cast<int>(i + 100);
//     }
//     for (std::size_t i = 0; i < arraySize; ++i)
//     {
//         EXPECT_EQ(i + 100, data[i]);
//     }
// }
