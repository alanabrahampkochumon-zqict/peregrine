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

    /// @brief Test fixture for @ref pmm::Pool<pmm::ManagedMemory> tests.
    struct ManagedPoolAllocator: public testing::Test
    {
        size_t poolSize{ 2_MB }, chunkSize{ 1_KB }, alignment{ 16 };
        pmm::Pool<pmm::ManagedMemory> pool{ poolSize, chunkSize, alignment };
    };

    /// @brief Test fixture for @ref pmm::Pool<pmm::UnmanagedMemory> tests.
    struct UnmanagedPoolAllocator: public testing::Test
    {
        size_t bufferSize{ 2_MB }, chunkSize{ 1_KB }, alignment{ 16 };
        uint8_t* buffer = new uint8_t[bufferSize];
        pmm::Pool<pmm::UnmanagedMemory> pool{ buffer, bufferSize, chunkSize, alignment };
    };


    /// @brief Parameters for Pool allocator's alignment testing.
    struct PoolAllocatorAlignmentParams
    {
        size_t poolSize, chunkSize, alignment;
    };
    /// @brief Parameterized test fixture for @ref pmm::Pool<> base address alignment.
    class PoolAllocatorAlignment: public testing::TestWithParam<PoolAllocatorAlignmentParams>
    {};
    INSTANTIATE_TEST_SUITE_P(
        PoolAllocatorTests, PoolAllocatorAlignment,
        testing::Values(PoolAllocatorAlignmentParams{ .poolSize = 4_KB, .chunkSize = 24, .alignment = 8 },
                        PoolAllocatorAlignmentParams{ .poolSize = 5123, .chunkSize = 32, .alignment = 32 },
                        PoolAllocatorAlignmentParams{ .poolSize = 9582, .chunkSize = 127, .alignment = 64 },
                        PoolAllocatorAlignmentParams{ .poolSize = 20_MB, .chunkSize = 2_KB, .alignment = 4_KB }));


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

    /**************************************
     *                                    *
     *           MANAGED STACK            *
     *                                    *
     **************************************/

    TEST_F(ManagedPoolAllocator, Ctor_InitializesMemberVariables)
    {
        EXPECT_EQ(poolSize, pool._poolSize);
        EXPECT_EQ(chunkSize, pool._chunkSize);
        EXPECT_EQ(alignment, pool._chunkAlignment);
        EXPECT_NE(nullptr, pool._buffer);
    }

    // TODO: Add debug death tests

    TEST_P(PoolAllocatorAlignment, Managed_Ctor_AlignsBaseAddress)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const Pool<> pool{ poolSize, chunkSize, alignment };
        const auto startAddress = reinterpret_cast<uintptr_t>(pool._buffer) + pool._initialAlignmentPadding;

        EXPECT_EQ(0, startAddress % alignment);
    }


    TEST_P(PoolAllocatorAlignment, Managed_Ctor_AlignsChunksize)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const Pool<> pool{ poolSize, chunkSize, alignment };

        EXPECT_EQ(0, pool._chunkSize % alignment);
    }



    /**************************************
     *                                    *
     *          UNMANAGED STACK           *
     *                                    *
     **************************************/

    TEST_F(UnmanagedPoolAllocator, Ctor_InitializesMemberVariables)
    {
        EXPECT_EQ(bufferSize, pool._poolSize);
        EXPECT_EQ(chunkSize, pool._chunkSize);
        EXPECT_EQ(alignment, pool._chunkAlignment);
        EXPECT_EQ(buffer, pool._buffer);
    }


    TEST_P(PoolAllocatorAlignment, Unmanaged_Ctor_AlignsBaseAddress)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const auto buffer = new uint8_t[poolSize];
        const Pool<UnmanagedMemory> pool{ buffer, poolSize, chunkSize, alignment };
        const auto startAddress = reinterpret_cast<uintptr_t>(pool._buffer) + pool._initialAlignmentPadding;

        EXPECT_EQ(0, startAddress % alignment);
    }


    TEST_P(PoolAllocatorAlignment, Unmanaged_Ctor_AlignsChunksize)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const auto buffer = new uint8_t[poolSize];
        const Pool<UnmanagedMemory> pool{ buffer, poolSize, chunkSize, alignment };

        EXPECT_EQ(0, pool._chunkSize % alignment);
    }
} // namespace pmm
