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
TEST(PMMMemoryTests, MemAlloc_ReturnsReadWriteableMemory)
{
    constexpr std::size_t size  = 2_MB; // Greater than granularity and page size.
    constexpr std::size_t count = size / sizeof(int);
    const auto data             = static_cast<int*>(pmm::memAlloc(size));
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


TEST(PMMMemoryTests, MemAlloc_ReturnsNullptrWhenAllocationFails)
{
    constexpr std::size_t size = std::numeric_limits<size_t>::max();
    const auto memory          = pmm::memAlloc(size);
    EXPECT_EQ(nullptr, memory);
}


TEST(PMMMemoryTests, MemAlloc_ReturnsZeroedOutMemory)
{
    constexpr size_t size = 2_MB;
    const auto memory     = static_cast<size_t*>(pmm::memAlloc(size));
    for (size_t i = 0; i < size / sizeof(size_t); ++i)
    {
        EXPECT_EQ(0, memory[i]);
    }
}



TEST(PMMMemoryTests, MemFree_ReturnsTrueOnSuccessfulFree)
{
    constexpr std::size_t size = 2_MB; // Greater than granularity and page size.
    const auto data            = static_cast<int*>(pmm::memAlloc(size));
    if (data != nullptr)
    {
        bool result = pmm::memFree(data, size);
        EXPECT_TRUE(result);
    }
    else
    {
        EXPECT_TRUE(false) << "There was an error allocating memory for testing!";
    }
}


TEST(PMMMemoryTests, MemFree_NullptrReturnsFalse) { EXPECT_FALSE(pmm::memFree(nullptr, 0)); }


/**************************************
 *         LINUX & MACOS TESTS        *
 **************************************/

#if defined(PMM_PLATFORM_LINUX) || defined(PMM_PLATFORM_MACOS)
/**
 * @test Verify that freeing a zero size ptr on non-Windows platforms return false.
 */
TEST(PMMMemoryTests, MemFree_ZeroSizeReturnsFalse)
{
    const auto address = pmm::memAlloc(24);
    EXPECT_FALSE(pmm::memFree(address, 0));
}

#endif


TEST(PMMMemoryTests, QueryMemoryDetailsReturnsAValidStructure)
{
    // NOTE: We cannot hardcode the values that the function will return
    // but we can ensure that they are not equal to zero.
    const auto [pageSize, granularity] = pmm::queryMemoryDetails();
    EXPECT_GT(pageSize, 0);
    EXPECT_GT(granularity, 0);
}

/** @} */
