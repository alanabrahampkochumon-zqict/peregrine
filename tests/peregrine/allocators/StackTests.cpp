/**
 * @file StackTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Verify @ref pmm::StackAllocator allocation, deallocation, and other related memory management logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


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
class StackTests: public ::testing::Test
{

public:
    std::size_t stackSize{ 5_KB };
    pmm::Stack<> stack{ stackSize };
};


class StackAllocationAlignment: public ::testing::TestWithParam<std::size_t>
{};
INSTANTIATE_TEST_SUITE_P(
    StackAlignmentTests, StackAllocationAlignment,
    ::testing::Values(4, 8, 16, 32, 64)); // TODO: Add more alignment and with different types of arena padding type

#ifndef NDEBUG
class StackAllocationAlignmentNonBinaryPowers: public ::testing::TestWithParam<std::size_t>
{};
INSTANTIATE_TEST_SUITE_P(NonPowersOfTwo, StackAllocationAlignmentNonBinaryPowers, ::testing::Values(0, 1, 3, 5, 111));
#endif

/**
 * @addtogroup T_PMM_Stack
 * @{
 */

/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/**
 * @brief Verify that size() of stack allocator returns to the total size of the stack.
 */
TEST(StackGetters, SizeReturnsTheSizeOfTheStack)
{
    constexpr std::size_t sizeInBytes = 512;
    const pmm::Stack stack{ sizeInBytes };

    EXPECT_EQ(sizeInBytes, stack.size());
}


/** @brief Verify that stack allocation returns a valid pointer, given an empty stack. */
TEST_F(StackTests, ReturnsNonNullPtrOnEmptyStack)
{
    const auto dataPtr = stack.alloc(120);
    ASSERT_NE(nullptr, dataPtr);
}


/** @brief Verify that stack allocation returns a valid pointer, given a non-empty stack with memory to spare. */
TEST_F(StackTests, ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.alloc(500);
    const auto dataPtr2 = stack.alloc(500);

    ASSERT_NE(nullptr, dataPtr1);
    ASSERT_NE(nullptr, dataPtr2);
    ASSERT_NE(dataPtr1, dataPtr2);
}



/** @brief Verify that allocated memory maintains data integrity. */
TEST_F(StackTests, RepeatedAllocationAndWritesDoNotCorruptData)
{
    const std::size_t array1Len = 50;
    const std::size_t array2Len = 25;
    const auto array1           = static_cast<int*>(stack.alloc(array1Len * sizeof(int)));

    // Write into first array
    for (std::size_t i = 0; i < array1Len; ++i)
    {
        array1[i] = static_cast<int>(i + 212);
    }

    // Write into second array
    const auto array2 = static_cast<int*>(stack.alloc(array2Len * sizeof(int)));
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


/** @brief Verify that allocate always return an address aligned to the specified boundary. */
TEST_F(StackTests, HeaderIsStoredBehindReturnedAddress)
{
    constexpr auto alignment = 8;
    const auto memoryStart   = static_cast<char*>(stack.alloc(500, alignment));

    const auto header = reinterpret_cast<pmm::LooseStackHeader*>(memoryStart - sizeof(pmm::LooseStackHeader));
    EXPECT_GE(alignment, header->padding);
}


/** @brief Verify that allocate always return an address aligned to the specified boundary. */
TEST_P(StackAllocationAlignment, AlwaysReturnAnAlignedMemoryAddress)
{
    const auto alignment = this->GetParam();
    const auto blockSize = 5 * alignment;

    pmm::Stack stack{ 8192 }; // 8KB Stack
    const void* dataAddress = stack.alloc(blockSize, alignment);

    // Verify returned address is 0 by using 2^n module trick
    ASSERT_EQ(0, reinterpret_cast<uintptr_t>(dataAddress) & (alignment - 1));
}


/** @brief Verify that stack free, frees up memory for subsequent allocations. */
TEST_F(StackTests, Free_FreesMemoryForSubsequentAllocations)
{
    constexpr std::size_t alignment = 8;
    const auto usableSize = stackSize - 128; // A big offset is used since we need to make room for header + alignment
    const auto freeMemory = static_cast<char*>(stack.alloc(usableSize, alignment));
    // Free the memory
    stack.free(freeMemory);

    // Allocation another buffer
    const auto elementCount  = (stackSize - alignment - 1) / sizeof(int);
    const auto newAllocation = static_cast<int*>(stack.alloc(elementCount * sizeof(int)));

    // Write to new allocation
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        newAllocation[i] = static_cast<int>(i + 3812);
    }

    // Verify the allocation is successful with data writes
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 3812), newAllocation[i]);
    }
}

/** @brief Verify that stack free, when called multiple times, free the allocated buffer. */
TEST_F(StackTests, Free_MultipleTimesMakesRoomInTheStack)
{
    constexpr std::size_t alignment = 8;
    // Last 128 is the offset used to make room for header + alignment
    const std::array<std::size_t, 4> allocationSizes{ 128, 256, 1024, stackSize - 128 - 256 - 1024 - 128 };
    std::array<void*, 4> memory;

    // Allocate Memory
    for (std::size_t i = 0; i < 4; ++i)
    {
        memory[i] = stack.alloc(allocationSizes[i], alignment);
    }

    // Free the memory
    // Due to rotation of unsigned types, we need to start from sizeof(array) and inside the loop use -1 to
    // properly index into the array
    // Data must be freed in the reverse order of allocation
    for (std::size_t i = 4; i > 0; --i)
    {
        stack.free(memory[i - 1]);
    }

    // Allocation another buffer with a large enough size that proper allocation will not happen without proper frees
    const auto elementCount  = (stackSize - alignment - 1) / sizeof(int);
    const auto newAllocation = static_cast<int*>(stack.alloc(elementCount * sizeof(int)));

    // Write to new allocation
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        newAllocation[i] = static_cast<int>(i + 3812);
    }

    // Verify the allocation is successful with data writes
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 3812), newAllocation[i]);
    }
}


/** @brief Verify that stack freeAll, frees the entire stack. */
TEST_F(StackTests, FreeAll_FreesTheEntireStack)
{
    constexpr std::size_t alignment = 8;
    // Last 128 is the offset used to make room for header + alignment
    const std::array<std::size_t, 4> allocationSizes{ 128, 256, 1024, stackSize - 128 - 256 - 1024 - 128 };

    // Allocate Memory
    for (std::size_t i = 0; i < 4; ++i)
    {
        static_cast<void>(stack.alloc(allocationSizes[i], alignment));
    }

    // Free the entire memory
    stack.freeAll();

    // Allocation another buffer with a large enough size that proper allocation will not happen without proper frees
    const auto elementCount  = (stackSize - alignment - 1) / sizeof(int);
    const auto newAllocation = static_cast<int*>(stack.alloc(elementCount * sizeof(int)));

    // Write to new allocation
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        newAllocation[i] = static_cast<int>(i + 3812);
    }

    // Verify the allocation is successful with data writes
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 3812), newAllocation[i]);
    }
}


#ifndef NDEBUG
/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*, when allocating memory greater
 *        than the stack capacity.
 */
TEST_F(StackTests, GreaterThanCapacity_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.alloc(stackSize + 10)), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given a stack nearing its capacity(allocation > free).
 */
TEST_F(StackTests, NearFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    static_cast<void>(stack.alloc(stackSize - 50));
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.alloc(500)), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given an uneven alignment(non-powers of 2).
 */
TEST_P(StackAllocationAlignmentNonBinaryPowers, AllocationInFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    pmm::Stack stack(5120);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.alloc(500, GetParam())), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given alignment greater than 128.
 */
TEST_F(StackTests, AllocationGreaterThanSize_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.alloc(500, 255)), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        when freeing nullptr.
 */
TEST_F(StackTests, FreeWithNullptr_TriggersAssertion) { EXPECT_DEBUG_DEATH(stack.free(nullptr), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        when freeing unallocated valid memory space.
 */
TEST_F(StackTests, FreeingUnallocatedMemory_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.alloc(size));
    EXPECT_DEBUG_DEATH(stack.free(memory + size + 1), "");
}


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        when freeing memory space below the base memory address.
 */
TEST_F(StackTests, FreeingBelowBufferMemory_TriggersAssertion)
{
    constexpr auto size      = 512;
    constexpr auto alignment = 8;
    // We are assuming worst case padding of 7 for alignment
    constexpr auto assumedHeaderSize = sizeof(pmm::LooseStackHeader) + alignment - 1;

    const auto memory = static_cast<char*>(stack.alloc(size, alignment));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.free(memory - assumedHeaderSize - 1), "");
}


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        when freeing memory above maximum memory address.
 */
TEST_F(StackTests, FreeingMemoryBeyondCapacity_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.alloc(size, 8));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.free(memory + stackSize), "");
}

#endif


/**************************************
 *                                    *
 *            FRIEND TESTS            *
 *                                    *
 **************************************/

namespace pmm
{

    /** @brief Verify that stack allocator is initialized with passed-in size. */
    TEST(StackInitialization, InitializesDefaultStateAndBuffer)
    {
        constexpr std::size_t sizeInBytes = 512;
        const Stack stack{ sizeInBytes };

        EXPECT_EQ(sizeInBytes, stack._size);
        EXPECT_EQ(0, stack._offset);
        EXPECT_NE(nullptr, stack._buffer); // TODO: Investigate leak-> Add Destructor
    }

    /** @brief Verify that allocation moves the stack offset by at least the request size. */
    TEST_F(StackTests, Initialization_MovesOffsetAtleastByAllocationSize)
    {
        // Note: Due to alignment and header storage we cannot guarantee
        // that the allocation will be exactly the size
        constexpr std::size_t allocationSize = 120;

        static_cast<void>(stack.alloc(allocationSize));
        EXPECT_GE(stack._offset, allocationSize);
    }

    /** @brief Verify that freeAll moves the offset back to zero. */
    TEST_F(StackTests, FreeAll_MovesOffsetToZero)
    {
        constexpr auto size = 512;
        // Initially allocate some memory
        static_cast<void>(stack.alloc(size));
        // Assert initial offset state is greater than size
        EXPECT_GT(stack._offset, size);

        // Free the entire stack
        stack.freeAll();

        // Offset is reset to 0
        EXPECT_EQ(0, stack._offset);
    }

} // namespace pmm
/** @} */
