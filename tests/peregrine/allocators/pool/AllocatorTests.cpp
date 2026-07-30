/**
 * @file AllocatorTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 27, 2026
 *
 * @brief Verifies pool allocator logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <gtest/gtest.h>
#include <peregrine/allocators/Pool.h>
#include <peregrine/utils/Constants.h>

namespace
{
    /**************************************
     *                                    *
     *             TEST SETUP             *
     *                                    *
     **************************************/
    using namespace pmm::constants;

    /// @test Test fixture for @ref pmm::Pool<pmm::ManagedMemory> tests.
    struct ManagedPoolAllocator: public testing::Test
    {
        size_t poolSize{ 2_MB }, chunkSize{ 1_KB }, alignment{ 16 };
        pmm::Pool<pmm::ManagedMemory> pool{ poolSize, chunkSize, alignment };
    };

    /// @test Test fixture for @ref pmm::Pool<pmm::UnmanagedMemory> tests.
    struct UnmanagedPoolAllocator: public testing::Test
    {
        size_t bufferSize{ 2_MB }, chunkSize{ 1_KB }, alignment{ 16 };
        uint8_t* buffer = new uint8_t[bufferSize];
        pmm::Pool<pmm::UnmanagedMemory> pool{ buffer, bufferSize, chunkSize, alignment };
    };
    //
    // struct PoolAllocatorAlignment{};



} // namespace


/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/




/**************************************
 *                                    *
 *           FRIEND TESTS             *
 *                                    *
 **************************************/

namespace pmm
{

    TEST_F(ManagedPoolAllocator, Ctor_InitializesMemberVariables)
    {
        EXPECT_EQ(poolSize, pool._poolSize);
        EXPECT_EQ(chunkSize, pool._chunkSize);
        EXPECT_EQ(alignment, pool._chunkAlignment);
        EXPECT_NE(nullptr, pool._buffer);
    }


    TEST_F(UnmanagedPoolAllocator, Ctor_InitializesMemberVariables)
    {
        EXPECT_EQ(bufferSize, pool._poolSize);
        EXPECT_EQ(chunkSize, pool._chunkSize);
        EXPECT_EQ(alignment, pool._chunkAlignment);
        EXPECT_EQ(buffer, pool._buffer);
    }
} // namespace pmm
