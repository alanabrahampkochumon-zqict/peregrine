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

TEST_F(ManagedSafeArenaTests, AllocBytes_AllocatingMemoryGreaterThanArenaSizeReturnsNullPtr)
{
    void* bytes = arena.allocBytes(arenaSize + 1);
    EXPECT_EQ(nullptr, bytes);
}


TEST_F(ManagedSafeArenaTests, AllocBytes_AllocatingInAFullArenaReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto data = arena.allocBytes(24);
    EXPECT_EQ(nullptr, data);
}


TEST_F(ManagedSafeArenaTests, Alloc_AllocatingInAFullArenaReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(ManagedSafeArenaTests, AllocV_FullArenaReturnsEmptySpan)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto vec = arena.allocV<Vec4>(2);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(ManagedSafeArenaTests, AllocV_ZeroCountReturnsEmptySpan)
{
    const auto vec = arena.allocV<Vec4>(0);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}



TEST_F(ManagedSafeArenaTests, Resize_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resize(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, Resize_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resize(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, Resize_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resize(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, Resize_LatestsAllocation_FullArenaReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize                = 32;
    [[maybe_unused]] const auto nearFullSize     = arena.allocBytes(arenaSize - (firstAllocSize + alignof(void*) + 1));
    [[maybe_unused]] const auto latestAllocation = arena.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = arena.resize(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, Resize_PriorToLatestsAllocation_FullArenaReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize    = arena.allocBytes(arenaSize - (firstAllocSize + alignof(void*) + 1));
    [[maybe_unused]] const auto resize          = arena.resize(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, ResizeFast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resizeFast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, ResizeFast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resizeFast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, ResizeFast_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resizeFast(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, ResizeFast_LatestsAllocation_FullArenaReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize                = 32;
    [[maybe_unused]] const auto nearFullSize     = arena.allocBytes(arenaSize - (firstAllocSize + alignof(void*) + 1));
    [[maybe_unused]] const auto latestAllocation = arena.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = arena.resizeFast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedSafeArenaTests, ResizeFast_PriorToLatestsAllocation_FullArenaReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize    = arena.allocBytes(arenaSize - (firstAllocSize + alignof(void*) + 1));
    [[maybe_unused]] const auto resize = arena.resizeFast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}



/**************************************
 *          UNMANAGED ARENA           *
 **************************************/

TEST_F(UnmanagedSafeArenaTests, AllocBytes_AllocatingMemoryGreaterThanArenaSizeReturnsNullPtr)
{
    void* bytes = arena.allocBytes(arenaSize + 1);
    EXPECT_EQ(nullptr, bytes);
}


TEST_F(UnmanagedSafeArenaTests, AllocBytes_AllocatingInAFullArenaReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto data = arena.allocBytes(24);
    EXPECT_EQ(nullptr, data);
}


TEST_F(UnmanagedSafeArenaTests, Alloc_AllocatingInAFullArenaReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto vec = arena.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(UnmanagedSafeArenaTests, AllocV_FullArenaReturnsEmptySpan)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize = arena.allocBytes(arenaSize - (alignof(void*) - 1));

    const auto vec = arena.allocV<Vec4>(2);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(UnmanagedSafeArenaTests, AllocV_ZeroCountReturnsEmptySpan)
{
    const auto vec = arena.allocV<Vec4>(0);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(UnmanagedSafeArenaTests, Resize_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resize(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, Resize_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resize(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, Resize_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resize(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, Resize_LatestsAllocation_FullArenaReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize                = 32;
    [[maybe_unused]] const auto nearFullSize     = arena.allocBytes(arenaSize - (firstAllocSize + alignof(void*) + 1));
    [[maybe_unused]] const auto latestAllocation = arena.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = arena.resize(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, Resize_PriorToLatestsAllocation_FullArenaReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize    = arena.allocBytes(arenaSize - (firstAllocSize + alignof(void*) + 1));
    [[maybe_unused]] const auto resize          = arena.resize(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, ResizeFast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resizeFast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, ResizeFast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resizeFast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, ResizeFast_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(12);
    [[maybe_unused]] const auto resize          = arena.resizeFast(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, ResizeFast_LatestsAllocation_FullArenaReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize                = 32;
    [[maybe_unused]] const auto nearFullSize     = arena.allocBytes(arenaSize - (firstAllocSize + alignof(void*) + 1));
    [[maybe_unused]] const auto latestAllocation = arena.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = arena.resizeFast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedSafeArenaTests, ResizeFast_PriorToLatestsAllocation_FullArenaReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = arena.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize    = arena.allocBytes(arenaSize - (firstAllocSize + alignof(void*) + 1));
    [[maybe_unused]] const auto resize = arena.resizeFast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}

#endif
