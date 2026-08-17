/**
 * @file DeathTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Verifies stack allocators's assertions trigger correctly in DEBUG MODE.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "StackTestSetup.h"


#ifdef ENABLE_PMM_DEATH_TESTS

namespace
{
    class StackAlignmentNonPowersOfTwo: public testing::TestWithParam<std::size_t>
    {};
    INSTANTIATE_TEST_SUITE_P(LooseStackAllocationAlignmentNonPowerOfTwo, StackAlignmentNonPowersOfTwo,
                             ::testing::Values(0, 1, 3, 5, 111));

} // namespace



/**************************************
 *            LOOSE STACK             *
 **************************************/

TEST(ManagedLooseStackCtorTests, ZeroSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory>(0)), ""); }


TEST(UnmanagedLooseStackCtorTests, ZeroSize_TriggersAssertionInDebugMode)
{
    const auto buffer = new uint8_t[512];
    EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory>(buffer, 0)), "");
    delete[] buffer;
}


TEST(UnmanagedLooseStackCtorTests, NullptrForBackingBuffer_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory>(nullptr, 512)), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*, when allocating memory greater
 *        than the stack capacity.
 */
TEST_F(LooseStackTests, Allocation_GreaterThanCapacity_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(stackSize + 10)), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given a stack nearing its capacity(allocation > free).
 */
TEST_F(LooseStackTests, Allocation_NearFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    static_cast<void>(stack.allocBytes(stackSize - 50));
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500)), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given an uneven alignment(non-powers of 2).
 */
TEST_P(StackAlignmentNonPowersOfTwo, LooseStackAllocation_InFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    pmm::Stack<pmm::stack::Loose> stack(5120);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500, GetParam())), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given alignment greater than 128.
 */
TEST_F(LooseStackTests, Allocation_GreaterThanSize_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500, 255)), ""); }


/**
 * @brief Verify that stack allocV triggers assertion in *DEBUG MODE*,
 *        when trying to allocate a zero size buffer.
 */
TEST_F(LooseStackTests, AllocV_SizeZero_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocV<int>(0)), ""); }


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing nullptr.
 */
TEST_F(LooseStackTests, FreeBytes_Nullptr_TriggersAssertion) { EXPECT_DEBUG_DEATH(stack.freeBytes(nullptr), ""); }


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing unallocated valid memory space.
 */
TEST_F(LooseStackTests, FreeBytes_UnallocatedMemoryAddress_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size));
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory + size + 1), "");
}


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing memory space below the base memory address.
 */
TEST_F(LooseStackTests, FreeBytes_BelowBufferMemoryAddress_TriggersAssertion)
{
    constexpr auto size      = 512;
    constexpr auto alignment = 8;
    // We are assuming worst case padding of 7 for alignment
    constexpr auto assumedHeaderSize = sizeof(pmm::LooseStackHeader) + alignment - 1;

    const auto memory = static_cast<char*>(stack.allocBytes(size, alignment));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory - assumedHeaderSize - 1), "");
}


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing memory above maximum memory address.
 */
TEST_F(LooseStackTests, FreeBytes_BeyondCapacity_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size, 8));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory + stackSize), "");
}


/**
 * @brief Verify that stack resize triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(LooseStackTests, Resize_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(LooseStackTests, Resize_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(address, 128, 0)), "");
}


TEST_F(LooseStackTests, Resize_FromZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(address, 0, 256)), "");
}


TEST_F(LooseStackTests, Resize_LatestAllocation_ToAnInsufficentlyLargeSizeTriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(address, 128, stackSize)), "");
}

/**
 * @brief Verify that stack resize using resizeFast triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(LooseStackTests, ResizeFast_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize using resizeFast triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(LooseStackTests, ResizeFast_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(address, 128, 0)), "");
}

TEST_F(LooseStackTests, ResizeFast_FromZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(address, 0, 256)), "");
}


/**
 * @brief Verify that stack resize using resizeLast triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(LooseStackTests, ResizeLast_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize using resizeLast triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(LooseStackTests, ResizeLast_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(address, 128, 0)), "");
}


TEST_F(LooseStackTests, ResizeLast_FromZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(address, 0, 256)), "");
}


/**************************************
 *            STRICT STACK            *
 **************************************/

TEST(ManagedStrictStackCtorTests, ZeroSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory>(0)), ""); }


TEST(UnmanagedStrictStackCtorTests, ZeroSize_TriggersAssertionInDebugMode)
{
    const auto buffer = new uint8_t[512];
    EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory>(buffer, 0)), "");
    delete[] buffer;
}


TEST(UnmanagedStrictStackCtorTests, NullptrForBackingBuffer_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory>(nullptr, 512)), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*, when allocating memory greater
 *        than the stack capacity.
 */
TEST_F(StrictStackTests, Allocation_GreaterThanCapacity_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(stackSize + 10)), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given a stack nearing its capacity(allocation > free).
 */
TEST_F(StrictStackTests, Allocation_NearFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    static_cast<void>(stack.allocBytes(stackSize - 50));
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500)), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given an uneven alignment(non-powers of 2).
 */
TEST_P(StackAlignmentNonPowersOfTwo, StrictStackAllocation_InFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    pmm::Stack<pmm::stack::Strict> stack(5120);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500, GetParam())), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given alignment greater than 128.
 */
TEST_F(StrictStackTests, Allocation_GreaterThanSize_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500, 255)), ""); }


/**
 * @brief Verify that stack allocV triggers assertion in *DEBUG MODE*,
 *        when trying to allocate a zero size buffer.
 */
TEST_F(StrictStackTests, AllocV_SizeZero_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocV<int>(0)), ""); }


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing nullptr.
 */
TEST_F(StrictStackTests, FreeBytes_Nullptr_TriggersAssertion) { EXPECT_DEBUG_DEATH(stack.freeBytes(nullptr), ""); }


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing unallocated valid memory space.
 */
TEST_F(StrictStackTests, FreeBytes_UnallocatedMemoryAddress_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size));
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory + size + 1), "");
}


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing memory space below the base memory address.
 */
TEST_F(StrictStackTests, FreeBytes_BelowBufferMemoryAddress_TriggersAssertion)
{
    constexpr auto size      = 512;
    constexpr auto alignment = 8;
    // We are assuming worst case padding of 7 for alignment
    constexpr auto assumedHeaderSize = sizeof(pmm::StrictStackHeader) + alignment - 1;

    const auto memory = static_cast<char*>(stack.allocBytes(size, alignment));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory - assumedHeaderSize - 1), "");
}


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing memory above maximum memory address.
 */
TEST_F(StrictStackTests, FreeBytes_BeyondCapacity_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size, 8));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory + stackSize), "");
}

/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when memory out of order of allocation.
 */
TEST_F(StrictStackTests, FreeBytes_OutOfOrder_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size, 8));

    // Second allocation
    static_cast<void>(stack.allocBytes(40));

    EXPECT_DEBUG_DEATH(stack.freeBytes(memory), "");
}

/**
 * @brief Verify that stack resize triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(StrictStackTests, Resize_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(StrictStackTests, Resize_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(address, 128, 0)), "");
}


TEST_F(StrictStackTests, Resize_FromZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(address, 0, 256)), "");
}


TEST_F(StrictStackTests, Resize_LatestAllocation_ToAnInsufficentlyLargeSizeTriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(address, 128, stackSize)), "");
}


/**
 * @brief Verify that stack resize using resizeFast triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(StrictStackTests, ResizeFast_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize using resizeFast triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(StrictStackTests, ResizeFast_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(address, 128, 0)), "");
}


TEST_F(StrictStackTests, ResizeFast_FromZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(address, 0, 256)), "");
}

/**
 * @brief Verify that stack resize using resizeLast triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(StrictStackTests, ResizeLast_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize using resizeLast triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(StrictStackTests, ResizeLast_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(address, 128, 0)), "");
}


/**
 * @brief Verify that stack resize using resizeLast triggers assertion in *DEBUG MODE*,
 *        when resizing allocation in out of order.
 */
TEST_F(StrictStackTests, ResizeLast_OutOfOrder_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    static_cast<void>(stack.allocBytes(20)); // Second allocation to trigger out of order
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(address, 128, 0)), "");
}


TEST_F(StrictStackTests, ResizeLast_FromZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(address, 0, 256)), "");
}

#endif
