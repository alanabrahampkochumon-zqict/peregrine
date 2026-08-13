/**
 * @file DeathTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 7, 2026
 *
 * @brief Verify assertion for fgm::Arena allocation in debug mode.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"

#include <gtest/gtest.h>
#include <peregrine/allocators/Arena.h>

#ifdef ENABLE_PMM_DEATH_TESTS
namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/
    /// @brief Test Fixture for @ref pmm::Arena<UnmanagedMemory> debug assertions.
    class ManagedArenaDeathTests: public testing::Test
    {
    public:
        static constexpr size_t arenaSize{ 4096 };
        pmm::Arena<> arena{ arenaSize };
    };


    /// @brief Test Fixture for @ref pmm::Arena<UnmanagedMemory> debug assertions.
    class UnmanagedArenaDeathTests: public testing::Test
    {
    public:
        static constexpr size_t arenaSize{ 4096 };
        pmm::Arena<> arena{ arenaSize };
    };


    /// @brief Test fixture for @ref pmm::Arena debug assertions for alignments.
    class ArenaAllocAlignmentTests: public testing::TestWithParam<size_t>
    {};
    INSTANTIATE_TEST_SUITE_P(ArenaNonPowersOfTwoAlignment, ArenaAllocAlignmentTests, testing::Values(0, 1, 3, 6, 12));


} // namespace



/**************************************
 *           MANAGED ARENA            *
 **************************************/

TEST(ManagedArenaCtorTests, ZeroArenaSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Arena<pmm::ManagedMemory>{ 0 }), ""); }

TEST_F(ManagedArenaDeathTests, AllocBytes_ZeroSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(0)), ""); }


TEST_F(ManagedArenaDeathTests, AllocBytes_SizeGreaterThanArenaSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(arenaSize + 1)), ""); }


TEST_P(ArenaAllocAlignmentTests, AllocBytes_UnevenAlignment_TriggersAssertionInDebugMode)
{
    pmm::Arena arena(512);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(12, 12)), "");
}


/// HERE 8 bit alignment needs to preserved when increment LargeData size beyond the arena size
/// else it will give a warning on unpadded-byte which will not compile in strict mode(See CML).
TEST_F(ManagedArenaDeathTests, Alloc_SizeGreaterThanArenaSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.alloc<LargeData<arenaSize + 8>>()), ""); }


TEST_F(ManagedArenaDeathTests, AllocV_SizeGreaterThanArenaSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocV<int>(arenaSize / sizeof(int) + 1)), ""); }


TEST_F(ManagedArenaDeathTests, Resize_Nullptr_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.resize(nullptr, 20, 24)), ""); }


TEST_F(ManagedArenaDeathTests, Resize_ZeroOldSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.resize(nullptr, 0, 24)), ""); }


TEST_F(ManagedArenaDeathTests, Resize_ZeroNewSize_TriggersAssertionInDebugMode)
{
    const auto memory = arena.allocBytes(16);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.resize(memory, 16, 0)), "");
}


TEST_F(ManagedArenaDeathTests, Resize_LargerThanArenaSize_TriggersAssertionInDebugMode)
{
    const auto memory = arena.allocBytes(16);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.resize(memory, 16, arenaSize + 1)), "");
}


TEST_F(ManagedArenaDeathTests, ResizeFast_Nullptr_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.resizeFast(nullptr, 20, 24)), ""); }


TEST_F(ManagedArenaDeathTests, ResizeFast_ZeroOldSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.resizeFast(nullptr, 0, 24)), ""); }


TEST_F(ManagedArenaDeathTests, ResizeFast_ZeroNewSize_TriggersAssertionInDebugMode)
{
    const auto memory = arena.allocBytes(16);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.resizeFast(memory, 16, 0)), "");
}


TEST_F(ManagedArenaDeathTests, ResizeFast_LargerThanArenaSize_TriggersAssertionInDebugMode)
{
    const auto memory = arena.allocBytes(16);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.resizeFast(memory, 16, arenaSize + 1)), "");
}



/**************************************
 *          UNMANAGED ARENA           *
 **************************************/

TEST(UnmanagedArenaCtorTests, ZeroArenaSize_TriggersAssertionInDebugMode)
{
    const auto buffer = new uint8_t[512];
    EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Arena<pmm::UnmanagedMemory>(buffer, 0)), "");
    delete[] buffer;
}


TEST(UnmanagedArenaCtorTests, NullptrForBackingBuffer_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Arena<pmm::UnmanagedMemory>(nullptr, 512)), ""); }


TEST_F(UnmanagedArenaDeathTests, AllocBytes_ZeroSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(0)), ""); }


TEST_F(UnmanagedArenaDeathTests, AllocBytes_SizeGreaterThanArenaSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(arenaSize + 1)), ""); }


TEST_P(ArenaAllocAlignmentTests, UnmanagedArena_AllocBytes_UnevenAlignment_TriggersAssertionInDebugMode)
{
    const auto buffer = new uint8_t[512];
    pmm::Arena<pmm::UnmanagedMemory> arena(buffer, 512);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocBytes(12, 12)), "");
    delete[] buffer;
}


/// HERE 8 bit alignment needs to preserved when increment LargeData size beyond the arena size
/// else it will give a warning on unpadded-byte which will not compile in strict mode(See CML).
TEST_F(UnmanagedArenaDeathTests, Alloc_SizeGreaterThanArenaSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.alloc<LargeData<arenaSize + 8>>()), ""); }


TEST_F(UnmanagedArenaDeathTests, AllocV_SizeGreaterThanArenaSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.allocV<int>(arenaSize / sizeof(int) + 1)), ""); }


TEST_F(UnmanagedArenaDeathTests, Resize_Nullptr_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.resize(nullptr, 20, 24)), ""); }


TEST_F(UnmanagedArenaDeathTests, Resize_ZeroOldSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.resize(nullptr, 0, 24)), ""); }


TEST_F(UnmanagedArenaDeathTests, Resize_ZeroNewSize_TriggersAssertionInDebugMode)
{
    const auto memory = arena.allocBytes(16);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.resize(memory, 16, 0)), "");
}


TEST_F(UnmanagedArenaDeathTests, Resize_LargerThanArenaSize_TriggersAssertionInDebugMode)
{
    const auto memory = arena.allocBytes(16);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.resize(memory, 16, arenaSize + 1)), "");
}


TEST_F(UnmanagedArenaDeathTests, ResizeFast_Nullptr_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.resizeFast(nullptr, 20, 24)), ""); }


TEST_F(UnmanagedArenaDeathTests, ResizeFast_ZeroOldSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(arena.resizeFast(nullptr, 0, 24)), ""); }


TEST_F(UnmanagedArenaDeathTests, ResizeFast_ZeroNewSize_TriggersAssertionInDebugMode)
{
    const auto memory = arena.allocBytes(16);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.resizeFast(memory, 16, 0)), "");
}


TEST_F(UnmanagedArenaDeathTests, ResizeFast_LargerThanArenaSize_TriggersAssertionInDebugMode)
{
    const auto memory = arena.allocBytes(16);
    EXPECT_DEBUG_DEATH(static_cast<void>(arena.resizeFast(memory, 16, arenaSize + 1)), "");
}

#endif
