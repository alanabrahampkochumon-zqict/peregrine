/**
 * @file StackTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Verify @ref pmm::StackAllocator allocation, deallocation, and other related memory management logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/allocators/Stack.h>


/**************************************
 *                                    *
 *             TEST SETUP             *
 *                                    *
 **************************************/

class StackAllocation: public ::testing::Test
{
public:
    std::size_t stackSize{};
    pmm::Stack stack{ 1 };

protected:
    void SetUp() override
    {
        stackSize = static_cast<std::size_t>(1024 * 5); // 5KB
        stack     = pmm::Stack(stackSize);
    }
};


class StackAllocationAlignment: public ::testing::TestWithParam<std::size_t>
{};
INSTANTIATE_TEST_SUITE_P(
    StackTests, StackAllocationAlignment,
    ::testing::Values(4, 8, 16, 32, 64)); // TODO: Add more alignment and with different types of arena padding type



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
TEST_F(StackAllocation, ReturnsNonNullPtrOnEmptyStack)
{
    const auto dataPtr = stack.alloc(120);
    ASSERT_NE(nullptr, dataPtr);
}


/**
 * @brief Verify that stack allocation returns a null pointer, when allocating memory greater
 *        than the stack capacity.
 */
TEST_F(StackAllocation, GreaterThanCapacity_ReturnsNullPtr)
{
    const auto dataPtr = stack.alloc(stackSize + 10);
    ASSERT_EQ(nullptr, dataPtr);
}


/** @brief Verify that stack allocation returns a valid pointer, given a non-empty stack with memory to spare. */
TEST_F(StackAllocation, ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.alloc(500);
    const auto dataPtr2 = stack.alloc(500);

    ASSERT_NE(nullptr, dataPtr1);
    ASSERT_NE(nullptr, dataPtr2);
    ASSERT_NE(dataPtr1, dataPtr2);
}


/**
 * @brief Verify that stack allocation returns a null pointer, given a stack nearing its capacity(allocation > free).
 */
TEST_F(StackAllocation, ReturnNullPtrFromNearFullStack)
{
    // Allocate a big chunk to fill the stack near capacity
    static_cast<void>(stack.alloc(stackSize - 50));
    const auto dataPtr = stack.alloc(500);
    ASSERT_EQ(nullptr, dataPtr);
}


/** @brief Verify that allocated memory maintains data integrity. */
TEST_F(StackAllocation, RepeatedAllocationAndWritesDoNotCorruptData)
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
TEST_P(StackAllocationAlignment, AlwaysReturnAnAlignedMemoryAddress)
{
    const auto alignment = this->GetParam();
    const auto blockSize = 5 * alignment;

    pmm::Stack stack{8192}; // 8KB Stack
    const void* dataAddress = stack.alloc(blockSize, alignment);

    // Verify returned address is 0 by using 2^n module trick
    ASSERT_EQ(0, reinterpret_cast<uintptr_t>(dataAddress) & alignment - 1);
}



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
    TEST_F(StackAllocation, MovesOffsetAtleastByAllocationSize)
    {
        // Note: Due to alignment and header storage we cannot guarantee
        // that the allocation will be exactly the size
        constexpr std::size_t allocationSize = 120;

        static_cast<void>(stack.alloc(allocationSize));
        EXPECT_GT(stack._offset, allocationSize);
    }

} // namespace pmm
/** @} */
