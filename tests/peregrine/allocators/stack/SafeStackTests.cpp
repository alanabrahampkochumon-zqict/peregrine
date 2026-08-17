/**
 * @file SafeStackTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 11, 2026
 *
 * @brief Verify managed and unmanaged safe stack operation for edge cases like freeing a nullptr in release mode.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#ifndef ENABLE_PMM_DEATH_TESTS

    #include "Utils.h"

    #include <gtest/gtest.h>
    #include <peregrine/allocators/Stack.h>
    #include <peregrine/utils/Constants.h>


namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/
    using namespace pmm::constants;

    /**
     * @brief Test fixture for managed loose safe @ref pmm::Stack.
     */
    class ManagedLooseSafeStackTests: public testing::Test
    {
    public:
        size_t stackSize{ 2_KB };
        pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Enabled, true> stack{ stackSize };
    };


    /**
     * @brief Test fixture for managed loose safe @ref pmm::Stack.
     */
    class UnmanagedLooseSafeStackTests: public testing::Test
    {
    public:
        size_t stackSize{ 2_KB };
        uint8_t* buffer = new uint8_t[stackSize];
        pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory, pmm::telemetry::Enabled, true> stack{ buffer, stackSize };

    protected:
        void TearDown() override { delete[] buffer; }
    };


    /**
     * @brief Test fixture for managed strict safe @ref pmm::Stack.
     */
    class ManagedStrictSafeStackTests: public testing::Test
    {
    public:
        size_t stackSize{ 2_KB };
        pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Enabled, true> stack{ stackSize };
    };


    /**
     * @brief Test fixture for managed strict safe @ref pmm::Stack.
     */
    class UnmanagedStrictSafeStackTests: public testing::Test
    {
    public:
        size_t stackSize{ 2_KB };
        uint8_t* buffer = new uint8_t[stackSize];
        pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory, pmm::telemetry::Enabled, true> stack{ buffer, stackSize };

    protected:
        void TearDown() override { delete[] buffer; }
    };


} // namespace



/**************************************
 *       MANAGED LOOSE STACK          *
 **************************************/

TEST_F(ManagedLooseSafeStackTests, AllocBytes_AllocatingMemoryGreaterThanStackSizeReturnsNullPtr)
{
    void* bytes = stack.allocBytes(stackSize + 1);
    EXPECT_EQ(nullptr, bytes);
}


TEST_F(ManagedLooseSafeStackTests, AllocBytes_AllocatingInAFullStackReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::LooseStackHeader) + (alignof(void*) + 1)));

    const auto data = stack.allocBytes(24);
    EXPECT_EQ(nullptr, data);
}


TEST_F(ManagedLooseSafeStackTests, Alloc_AllocatingInAFullStackReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::LooseStackHeader) + (alignof(void*) + 1)));

    const auto vec = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(ManagedLooseSafeStackTests, AllocV_FullStackReturnsEmptySpan)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::LooseStackHeader) + (alignof(void*) + 1)));

    const auto vec = stack.allocV<Vec4>(2);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(ManagedLooseSafeStackTests, AllocV_ZeroCountReturnsEmptySpan)
{
    const auto vec = stack.allocV<Vec4>(0);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(ManagedLooseSafeStackTests, Resize_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, Resize_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, Resize_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, Resize_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resize(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, Resize_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto resize = stack.resize(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeFast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeFast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeFast_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeFast_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resizeFast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeFast_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto resize = stack.resizeFast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeLast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeLast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeLast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeLast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeLast_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resizeLast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, ResizeLast_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto resize = stack.resizeLast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedLooseSafeStackTests, FreeBytes_Nullptr_ReturnsFalse) { EXPECT_FALSE(stack.freeBytes(nullptr)); }


TEST_F(ManagedLooseSafeStackTests, FreeBytes_AddressGreaterThanStackSize_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation + stackSize));
}


TEST_F(ManagedLooseSafeStackTests, FreeBytes_AddressBelowFirstAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation - 1));
}

TEST_F(ManagedLooseSafeStackTests, Free_Nullptr_ReturnsFalse)
{ EXPECT_FALSE(stack.free(static_cast<uint8_t*>(nullptr))); }


TEST_F(ManagedLooseSafeStackTests, Free_AddressGreaterThanStackSize_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.free(firstAllocation + stackSize));
}


TEST_F(ManagedLooseSafeStackTests, Free_AddressBelowFirstAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.free(firstAllocation - 1));
}


TEST_F(ManagedLooseSafeStackTests, FreeV_EmptySpan_ReturnsFalse) { EXPECT_FALSE(stack.freeV(std::span<int>{})); }



/**************************************
 *      UNMANAGED LOOSE STACK         *
 **************************************/

TEST_F(UnmanagedLooseSafeStackTests, AllocBytes_AllocatingMemoryGreaterThanStackSizeReturnsNullPtr)
{
    void* bytes = stack.allocBytes(stackSize + 1);
    EXPECT_EQ(nullptr, bytes);
}


TEST_F(UnmanagedLooseSafeStackTests, AllocBytes_AllocatingInAFullStackReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::LooseStackHeader) + (alignof(void*) + 1)));

    const auto data = stack.allocBytes(24);
    EXPECT_EQ(nullptr, data);
}


TEST_F(UnmanagedLooseSafeStackTests, Alloc_AllocatingInAFullStackReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::LooseStackHeader) + (alignof(void*) + 1)));

    const auto vec = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(UnmanagedLooseSafeStackTests, AllocV_FullStackReturnsEmptySpan)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::LooseStackHeader) + (alignof(void*) + 1)));

    const auto vec = stack.allocV<Vec4>(2);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(UnmanagedLooseSafeStackTests, AllocV_ZeroCountReturnsEmptySpan)
{
    const auto vec = stack.allocV<Vec4>(0);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(UnmanagedLooseSafeStackTests, Resize_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, Resize_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, Resize_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, Resize_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resize(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, Resize_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto resize = stack.resize(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeFast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeFast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeFast_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeFast_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resizeFast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeFast_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto resize = stack.resizeFast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeLast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeLast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeLast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeLast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeLast_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resizeLast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, ResizeLast_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::LooseStackHeader)));
    [[maybe_unused]] const auto resize = stack.resizeLast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedLooseSafeStackTests, FreeBytes_Nullptr_ReturnsFalse) { EXPECT_FALSE(stack.freeBytes(nullptr)); }


TEST_F(UnmanagedLooseSafeStackTests, FreeBytes_AddressGreaterThanStackSize_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation + stackSize));
}


TEST_F(UnmanagedLooseSafeStackTests, FreeBytes_AddressBelowFirstAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation - 1));
}

TEST_F(UnmanagedLooseSafeStackTests, Free_Nullptr_ReturnsFalse)
{ EXPECT_FALSE(stack.free(static_cast<uint8_t*>(nullptr))); }


TEST_F(UnmanagedLooseSafeStackTests, Free_AddressGreaterThanStackSize_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.free(firstAllocation + stackSize));
}


TEST_F(UnmanagedLooseSafeStackTests, Free_AddressBelowFirstAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.free(firstAllocation - 1));
}


TEST_F(UnmanagedLooseSafeStackTests, FreeV_EmptySpan_ReturnsFalse) { EXPECT_FALSE(stack.freeV(std::span<int>{})); }



/**************************************
 *       MANAGED STRICT STACK          *
 **************************************/

TEST_F(ManagedStrictSafeStackTests, AllocBytes_AllocatingMemoryGreaterThanStackSizeReturnsNullPtr)
{
    void* bytes = stack.allocBytes(stackSize + 1);
    EXPECT_EQ(nullptr, bytes);
}


TEST_F(ManagedStrictSafeStackTests, AllocBytes_AllocatingInAFullStackReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::StrictStackHeader) + (alignof(void*) + 1)));

    const auto data = stack.allocBytes(24);
    EXPECT_EQ(nullptr, data);
}


TEST_F(ManagedStrictSafeStackTests, Alloc_AllocatingInAFullStackReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::StrictStackHeader) + (alignof(void*) + 1)));

    const auto vec = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(ManagedStrictSafeStackTests, AllocV_FullStackReturnsEmptySpan)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::StrictStackHeader) + (alignof(void*) + 1)));

    const auto vec = stack.allocV<Vec4>(2);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(ManagedStrictSafeStackTests, AllocV_ZeroCountReturnsEmptySpan)
{
    const auto vec = stack.allocV<Vec4>(0);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(ManagedStrictSafeStackTests, Resize_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, Resize_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, Resize_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, Resize_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resize(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, Resize_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 2 * sizeof(pmm::StrictStackHeader)));
    std::cout << "First: " << firstAllocation << " Near Full Size: " << nearFullSize << "\n";
    [[maybe_unused]] const auto resize = stack.resize(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeFast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeFast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeFast_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeFast_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resizeFast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeFast_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto resize = stack.resizeFast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeLast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeLast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeLast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeLast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeLast_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resizeLast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, ResizeLast_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + 2 * sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto resize = stack.resizeLast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


/// @test Verify that even with a non-empty stack, resizing any allocations using resizeLast prior to last allocation
///       returns a nullptr.
TEST_F(ManagedStrictSafeStackTests, ResizeLast_PriorToLatestsAllocation_ReturnsNullptr)
{
    // Use the full capacity
    constexpr auto allocationSize                = 32;
    [[maybe_unused]] const auto firstAllocation  = stack.allocBytes(allocationSize);
    [[maybe_unused]] const auto secondAllocation = stack.allocBytes(allocationSize);
    [[maybe_unused]] const auto resize = stack.resizeLast(firstAllocation, allocationSize, allocationSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(ManagedStrictSafeStackTests, FreeBytes_Nullptr_ReturnsFalse) { EXPECT_FALSE(stack.freeBytes(nullptr)); }


TEST_F(ManagedStrictSafeStackTests, FreeBytes_AddressGreaterThanStackSize_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation + stackSize));
}


TEST_F(ManagedStrictSafeStackTests, NonLatestAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation  = static_cast<uint8_t*>(stack.allocBytes(512));
    [[maybe_unused]] const auto secondAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation));
}


TEST_F(ManagedStrictSafeStackTests, FreeBytes_AddressBelowFirstAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation - 1));
}

TEST_F(ManagedStrictSafeStackTests, Free_Nullptr_ReturnsFalse)
{ EXPECT_FALSE(stack.free(static_cast<uint8_t*>(nullptr))); }


TEST_F(ManagedStrictSafeStackTests, Free_AddressGreaterThanStackSize_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.free(firstAllocation + stackSize));
}


TEST_F(ManagedStrictSafeStackTests, Free_AddressBelowFirstAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.free(firstAllocation - 1));
}


TEST_F(ManagedStrictSafeStackTests, FreeV_EmptySpan_ReturnsFalse) { EXPECT_FALSE(stack.freeV(std::span<int>{})); }



/**************************************
 *      UNMANAGED STRICT STACK        *
 **************************************/

TEST_F(UnmanagedStrictSafeStackTests, AllocBytes_AllocatingMemoryGreaterThanStackSizeReturnsNullPtr)
{
    void* bytes = stack.allocBytes(stackSize + 1);
    EXPECT_EQ(nullptr, bytes);
}


TEST_F(UnmanagedStrictSafeStackTests, AllocBytes_AllocatingInAFullStackReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::StrictStackHeader) + (alignof(void*) + 1)));

    const auto data = stack.allocBytes(24);
    EXPECT_EQ(nullptr, data);
}


TEST_F(UnmanagedStrictSafeStackTests, Alloc_AllocatingInAFullStackReturnsNullPtr)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::StrictStackHeader) + (alignof(void*) + 1)));

    const auto vec = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(UnmanagedStrictSafeStackTests, AllocV_FullStackReturnsEmptySpan)
{
    // Use the full capacity
    [[maybe_unused]] const auto fullSize =
        stack.allocBytes(stackSize - (sizeof(pmm::StrictStackHeader) + (alignof(void*) + 1)));

    const auto vec = stack.allocV<Vec4>(2);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(UnmanagedStrictSafeStackTests, AllocV_ZeroCountReturnsEmptySpan)
{
    const auto vec = stack.allocV<Vec4>(0);

    EXPECT_EQ(nullptr, vec.data());
    EXPECT_EQ(0, vec.size());
}


TEST_F(UnmanagedStrictSafeStackTests, Resize_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, Resize_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, Resize_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resize(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, Resize_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resize(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, Resize_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 2 * sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto resize = stack.resize(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeFast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeFast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeFast_ZeroAlignmentReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeFast(firstAllocation, 12, 24, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeFast_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resizeFast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeFast_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto resize = stack.resizeFast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeLast_ZeroNewSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeLast(firstAllocation, 12, 0);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeLast_ZeroOldSizeReturnsNullptr)
{
    // Use the full capacity
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(12);
    [[maybe_unused]] const auto resize          = stack.resizeLast(firstAllocation, 0, 12);

    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeLast_LatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize = 32;
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 1 + sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto latestAllocation = stack.allocBytes(firstAllocSize);

    [[maybe_unused]] const auto resize = stack.resizeLast(latestAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, ResizeLast_PriorToLatestsAllocation_FullStackReturnsNullptr)
{
    // Use the full capacity
    constexpr auto firstAllocSize               = 32;
    [[maybe_unused]] const auto firstAllocation = stack.allocBytes(firstAllocSize);
    [[maybe_unused]] const auto nearFullSize =
        stack.allocBytes(stackSize - (firstAllocSize + alignof(void*) + 2 * sizeof(pmm::StrictStackHeader)));
    [[maybe_unused]] const auto resize = stack.resizeLast(firstAllocation, firstAllocSize, firstAllocSize + 120);
    EXPECT_EQ(nullptr, resize);
}


/// @test Verify that even with a non-empty stack, resizing any allocations using resizeLast prior to last allocation
///       returns a nullptr.
TEST_F(UnmanagedStrictSafeStackTests, ResizeLast_PriorToLatestsAllocation_ReturnsNullptr)
{
    // Use the full capacity
    constexpr auto allocationSize                = 32;
    [[maybe_unused]] const auto firstAllocation  = stack.allocBytes(allocationSize);
    [[maybe_unused]] const auto secondAllocation = stack.allocBytes(allocationSize);
    [[maybe_unused]] const auto resize = stack.resizeLast(firstAllocation, allocationSize, allocationSize + 120);
    EXPECT_EQ(nullptr, resize);
}


TEST_F(UnmanagedStrictSafeStackTests, FreeBytes_Nullptr_ReturnsFalse) { EXPECT_FALSE(stack.freeBytes(nullptr)); }


TEST_F(UnmanagedStrictSafeStackTests, FreeBytes_AddressGreaterThanStackSize_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation + stackSize));
}


TEST_F(UnmanagedStrictSafeStackTests, NonLatestAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation  = static_cast<uint8_t*>(stack.allocBytes(512));
    [[maybe_unused]] const auto secondAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation));
}


TEST_F(UnmanagedStrictSafeStackTests, FreeBytes_AddressBelowFirstAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.freeBytes(firstAllocation - 1));
}

TEST_F(UnmanagedStrictSafeStackTests, Free_Nullptr_ReturnsFalse)
{ EXPECT_FALSE(stack.free(static_cast<uint8_t*>(nullptr))); }


TEST_F(UnmanagedStrictSafeStackTests, Free_AddressGreaterThanStackSize_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.free(firstAllocation + stackSize));
}


TEST_F(UnmanagedStrictSafeStackTests, Free_AddressBelowFirstAllocation_ReturnsFalse)
{
    [[maybe_unused]] const auto firstAllocation = static_cast<uint8_t*>(stack.allocBytes(512));
    EXPECT_FALSE(stack.free(firstAllocation - 1));
}


TEST_F(UnmanagedStrictSafeStackTests, FreeV_EmptySpan_ReturnsFalse) { EXPECT_FALSE(stack.freeV(std::span<int>{})); }



#endif
