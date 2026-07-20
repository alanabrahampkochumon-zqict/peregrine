/**
 * @file StrictStackTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Verify @ref pmm::Stack<pmm::stack::Strict> allocation, deallocation, and other related memory management logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"
#include "Utils.h"

#include <array>
#include <gtest/gtest.h>
#include <peregrine/allocators/Stack.h>
#include <peregrine/utils/Constants.h>


/**************************************
 *                                    *
 *             TEST SETUP             *
 *                                    *
 **************************************/

using namespace pmm::constants;
constexpr static auto STACK_SIZE = 5_KB;
class StrictStackTests: public ::testing::Test
{

public:
    std::size_t stackSize{ STACK_SIZE };
    pmm::Stack<pmm::stack::Strict> stack{ stackSize };
};


class StrictStackAllocationAlignment: public ::testing::TestWithParam<std::size_t>
{};
INSTANTIATE_TEST_SUITE_P(
    StackAlignmentTests, StrictStackAllocationAlignment,
    ::testing::Values(4, 8, 16, 32, 64, 512, 4096));

// #ifndef NDEBUG
// class StrictStackAllocationAlignmentNonBinaryPowers: public ::testing::TestWithParam<std::size_t>
// {};
// INSTANTIATE_TEST_SUITE_P(NonPowersOfTwo, StrictStackAllocationAlignmentNonBinaryPowers, ::testing::Values(0, 1, 3, 5, 111));
// #endif
//
//
//
// struct StrictStackResizeLastParams
// {
//     std::size_t oldSize, newSize;
// };
// class StrictStackResizeLast: public ::testing::TestWithParam<StrictStackResizeLastParams>
// {};
// INSTANTIATE_TEST_SUITE_P(StackResize, StrictStackResizeLast,
//                          ::testing::Values(StrictStackResizeLastParams{ .oldSize = 256, .newSize = 128 },
//                                            StrictStackResizeLastParams{ .oldSize = 128, .newSize = 512 },
//                                            StrictStackResizeLastParams{ .oldSize = 2048, .newSize = 4096 },
//                                            StrictStackResizeLastParams{ .oldSize = 4096, .newSize = 2048 }));


/**
 * @addtogroup T_PMM_STACK
 * @{
 */


/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/**
 * @brief Verify that size() returns the total size of the stack.
 */
TEST_F(StrictStackTests, Size_ReturnsTheSizeOfTheStack) { EXPECT_EQ(stackSize, stack.size()); }


/**
 * @brief Verify that freeSize() returns the total size of the stack, when no allocation is made.
 */
TEST_F(StrictStackTests, FreeSize_NoAllocations_ReturnsTheSizeOfTheStack) { EXPECT_EQ(stackSize, stack.freeSize()); }


/**
 * @brief Verify that freeSize() returns the size - (allocated size + padding),
 *        after an allocation is made.
 */
TEST_F(StrictStackTests, FreeSize_ReturnsStackSizeMinusPaddingAndAllocationSize)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    const auto header       = reinterpret_cast<pmm::StrictStackHeader*>(memory - sizeof(pmm::StrictStackHeader));
    const auto expectedSize = stackSize - (allocatedSize + header->padding);

    EXPECT_EQ(expectedSize, stack.freeSize());
}


/**
 * @brief Verify that freeSize() returns stack size, after an allocation is freed.
 */
// TEST_F(StrictStackTests, FreeSize_AfterFreeingAllocation_ReturnsStackSize)
// {
//     constexpr std::size_t allocatedSize = 512;
//     const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
//     stack.freeBytes(memory);
//     EXPECT_EQ(stackSize, stack.freeSize());
// }


/**
 * @brief Verify that freeSize() returns stack size, after all allocations are freed.
 */
TEST_F(StrictStackTests, FreeSize_AfterFreeingAllAllocations_ReturnsStackSize)
{
    constexpr std::size_t allocatedSize = 512;
    static_cast<void>(stack.allocBytes(allocatedSize));
    stack.freeAll();
    EXPECT_EQ(stackSize, stack.freeSize());
}


/**
 * @brief Verify that usedSize() returns 0, when no allocation is made.
 */
TEST_F(StrictStackTests, UsedSize_NoAllocations_ReturnsZero) { EXPECT_EQ(0, stack.usedSize()); }


/**
 * @brief Verify that usedSize() returns (allocated size + padding), after an allocation is made.
 */
TEST_F(StrictStackTests, UsedSize_ReturnsStackSizeMinusPaddingAndAllocationSize)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    const auto header       = reinterpret_cast<pmm::StrictStackHeader*>(memory - sizeof(pmm::StrictStackHeader));
    const auto expectedSize = allocatedSize + header->padding;

    EXPECT_EQ(expectedSize, stack.usedSize());
}


/**
 * @brief Verify that usedSize() returns zero, after an allocation is freed.
 */
// TEST_F(StrictStackTests, UsedSize_AfterFreeingAllocation_ReturnsZero)
// {
//     constexpr std::size_t allocatedSize = 512;
//     const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
//     stack.freeBytes(memory);
//     EXPECT_EQ(0, stack.usedSize());
// }


/**
 * @brief Verify that usedSize() returns zero, after all allocations are freed.
 */
TEST_F(StrictStackTests, UsedSize_FreeAllAllocation_ReturnsStackSize)
{
    constexpr std::size_t allocatedSize = 512;
    static_cast<void>(stack.allocBytes(allocatedSize));
    stack.freeAll();
    EXPECT_EQ(0, stack.usedSize());
}


/**************************************
 *                                    *
 *            ALLOC BYTES             *
 *                                    *
 **************************************/

/** @brief Verify that stack allocation using allocBytes returns a valid pointer, given an empty stack. */
TEST_F(StrictStackTests, AllocBytes_ReturnsNonNullPtrOnEmptyStack)
{
    const auto dataPtr = stack.allocBytes(120);
    EXPECT_NE(nullptr, dataPtr);
}


/**
 * @brief Verify that stack allocation using allocBytes returns a valid pointer,
 *        given a non-empty stack with memory to spare.
 */
TEST_F(StrictStackTests, AllocBytes_ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.allocBytes(500);
    const auto dataPtr2 = stack.allocBytes(500);

    EXPECT_NE(nullptr, dataPtr1);
    EXPECT_NE(nullptr, dataPtr2);
    EXPECT_NE(dataPtr1, dataPtr2);
}



/** @brief Verify that memory allocated using allocBytes maintains data integrity. */
TEST_F(StrictStackTests, AllocBytes_RepeatedAllocationAndWritesDoNotCorruptData)
{
    constexpr std::size_t array1Len = 50;
    constexpr std::size_t array2Len = 25;
    const auto array1               = static_cast<int*>(stack.allocBytes(array1Len * sizeof(int)));

    // Write into first array
    for (std::size_t i = 0; i < array1Len; ++i)
    {
        array1[i] = static_cast<int>(i + 212);
    }

    // Write into second array
    const auto array2 = static_cast<int*>(stack.allocBytes(array2Len * sizeof(int)));
    for (std::size_t i = 0; i < array2Len; ++i)
    {
        array2[i] = static_cast<int>(i + 515);
    }


    // Verify no overlap / data corruption has occurred
    for (std::size_t i = 0; i < array1Len; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 212), array1[i]);
    }

    for (std::size_t i = 0; i < array2Len; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 515), array2[i]);
    }
}


/** @brief Verify that allocation using allocBytes stores header before returned address. */
TEST_F(StrictStackTests, AllocBytes_HeaderIsStoredBehindReturnedAddress)
{
    constexpr auto alignment = 8;
    const auto memoryStart   = static_cast<char*>(stack.allocBytes(500, alignment));

    const auto header =
        reinterpret_cast<pmm::StrictStackHeader*>(reinterpret_cast<char*>(memoryStart) - sizeof(pmm::StrictStackHeader));
    EXPECT_GE(header->padding, alignment);
}


/**
 * @brief Verify that allocation using allocBytes always return an address aligned
 *        to the specified boundary.
 */
TEST_P(StrictStackAllocationAlignment, AllocBytes_AlwaysReturnAnAlignedMemoryAddress)
{
    const auto alignment = this->GetParam();
    const auto blockSize = 5 * alignment;
    const auto stackSize = 10 * alignment;
    pmm::Stack<pmm::stack::Strict> stack{ stackSize };
    const void* dataAddress = stack.allocBytes(blockSize, alignment);

    // Verify returned address is 0 by using 2^n modulo trick
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(dataAddress) & (alignment - 1));
}


/** @} */

