/**
 * @file SafeArenaTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 11, 2026
 *
 * @brief Verify managed and unmanaged safe arena operation for edge cases like freeing a nullptr in release mode.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#ifndef ENABLE_PMM_DEATH_TESTS

    #include "Utils.h"

    #include <gtest/gtest.h>
    #include <peregrine/allocators/Arena.h>
    #include <peregrine/utils/Constants.h>


namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/
    using namespace pmm::constants;

    /**
     * @brief Test fixture for managed safe @ref pmm::Arena.
     */
    class ManagedSafeArenaTests: public testing::Test
    {
    public:
        size_t arenaSize{ 2_KB };
        pmm::Arena<pmm::ManagedMemory, pmm::telemetry::Enabled, true> arena{ arenaSize };
    };


    /**
     * @brief Test fixture for managed safe @ref pmm::Arena.
     */
    class UnmanagedSafeArenaTests: public testing::Test
    {
    public:
        size_t arenaSize{ 2_KB };
        uint8_t* buffer = new uint8_t[arenaSize];
        pmm::Arena<pmm::UnmanagedMemory, pmm::telemetry::Enabled, true> arena{ buffer, arenaSize };

    protected:
        void TearDown() override { delete[] buffer; }
    };


} // namespace



/**************************************
 *           MANAGED ARENA            *
 **************************************/

TEST_F(ManagedSafeArenaTests, ReturnsNullPtrWhenAllocatingMemoryGreaterThanArenaSize)
{
    void* bytes = arena.allocBytes(arenaSize + 1);
    EXPECT_EQ(nullptr, bytes);
}


TEST_F(ManagedSafeArenaTests, AllocBytes_ReturnsNullPtrWhenAllocatingInAFullArena)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto data = arena.allocBytes(24);
    EXPECT_EQ(nullptr, data);
}


TEST_F(ManagedSafeArenaTests, Alloc_ReturnsNullPtrWhenAllocatingInAFullArena)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));
    std::cout << "Full Size Addr: " << fullSize << '\n';

    const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(ManagedSafeArenaTests, AllocV_ReturnsEmptySpan)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto vec = arena.allocV<Vec4>(2);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}



/**************************************
 *          UNMANAGED ARENA           *
 **************************************/

TEST_F(UnmanagedSafeArenaTests, ReturnsNullPtrWhenAllocatingMemoryGreaterThanArenaSize)
{
    void* bytes = arena.allocBytes(arenaSize + 1);
    EXPECT_EQ(nullptr, bytes);
}


TEST_F(UnmanagedSafeArenaTests, AllocBytes_ReturnsNullPtrWhenAllocatingInAFullArena)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto data = arena.allocBytes(24);
    EXPECT_EQ(nullptr, data);
}


TEST_F(UnmanagedSafeArenaTests, Alloc_ReturnsNullPtrWhenAllocatingInAFullArena)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));
    std::cout << "Full Size Addr: " << fullSize << '\n';

    const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(UnmanagedSafeArenaTests, AllocV_ReturnsEmptySpan)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto vec = arena.allocV<Vec4>(2);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}

#endif
