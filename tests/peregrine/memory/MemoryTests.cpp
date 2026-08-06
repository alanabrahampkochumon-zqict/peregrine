/**
 * @file MemoryTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 4, 2026
 *
 * @brief Verifies memory management logic specific in a platform-agnostic manner.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <gtest/gtest.h>
#include <limits>
#include <peregrine/memory/Memory.h>
#include <peregrine/utils/Constants.h>


/**
 * @addtogroup T_PMM_Memory
 * @{
 */

using namespace pmm::constants;
TEST(PMMMemory, Alloc_ReturnsReadWriteableMemory)
{
    constexpr std::size_t size  = 2_MB; // Greater than granularity and page size.
    constexpr std::size_t count = size / sizeof(int);
    const auto data             = static_cast<int*>(pmm::malloc(size));
    if (data != nullptr)
    {
        for (size_t i = 0; i < count; ++i)
        {
            data[i] = static_cast<int>(131 * i + 11);
        }


        for (size_t i = 0; i < count; ++i)
        {
            EXPECT_EQ(static_cast<int>(131 * i + 11), data[i]);
        }
    }
    else
    {
        EXPECT_TRUE(false) << "There was an error allocating memory for testing!";
    }
}


TEST(PMMMemory, Alloc_ReturnsNullptrWhenAllocationFails)
{
    constexpr std::size_t size = std::numeric_limits<size_t>::max();
    const auto memory          = pmm::malloc(size);
    EXPECT_EQ(nullptr, memory);
}


TEST(PMMMemory, Free_ReturnsTrueOnSuccessfulFree)
{
    constexpr std::size_t size = 2_MB; // Greater than granularity and page size.
    const auto data            = static_cast<int*>(pmm::malloc(size));
    if (data != nullptr)
    {
        bool result = pmm::mfree(data, size);
        EXPECT_TRUE(result);
    }
    else
    {
        EXPECT_TRUE(false) << "There was an error allocating memory for testing!";
    }
}


TEST(PMMMemory, QueryMemoryDetailsReturnsAValidStructure)
{
    // NOTE: We cannot hardcode the values that the function will return
    // but we can ensure that they are not equal to zero.
    const auto [pageSize, granularity] = pmm::queryMemoryDetails();
    EXPECT_GT(pageSize, 0);
    EXPECT_GT(granularity, 0);
}

/** @} */
