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
        EXPECT_NE(nullptr, stack._buffer);
    }

} // namespace pmm
/** @} */
