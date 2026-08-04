/**
 * @file WinMemoryTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 4, 2026
 *
 * @brief Verifies memory management logic specific to Windows.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

// PMM_WINDOWS is a custom macro
#include <gtest/gtest.h>
#include <peregrine/memory/Memory.h>
#include <peregrine/utils/Constants.h>

#ifdef _WIN32

using namespace pmm::constants;
TEST(WindowsMemory, Alloc_ReturnsReadWriteableMemory)
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
        EXPECT_TRUE(false) << "There was an error allocation memory in windows";
    }
}


TEST(WindowsMemory, Free_ReturnsTrueOnSuccessfulFree)
{
    constexpr std::size_t size  = 2_MB; // Greater than granularity and page size.
    const auto data             = static_cast<int*>(pmm::malloc(size));
    if (data != nullptr)
    {
        bool result = pmm::mfree(data, size);
        EXPECT_TRUE(result);
    }
    else
    {
        EXPECT_TRUE(false) << "There was an error allocation memory in windows";
    }
}

#endif
