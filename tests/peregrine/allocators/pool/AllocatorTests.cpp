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

        friend void PrintTo(const ManagedPoolAllocator& param, std::ostream* os)
        {
            *os << "Managed Pool Allocator (Pool Size: " << param.poolSize << ", Alignment: " << param.alignment
                << ", Chunk Size: " << param.chunkSize << ")";
        }
    };

    /// @brief Test fixture for @ref pmm::Pool<pmm::UnmanagedMemory> tests.
    struct UnmanagedPoolAllocator: public testing::Test
    {
        size_t bufferSize{ 2_MB }, chunkSize{ 1_KB }, alignment{ 16 };
        uint8_t* buffer = new uint8_t[bufferSize];
        pmm::Pool<pmm::UnmanagedMemory> pool{ buffer, bufferSize, chunkSize, alignment };

        friend void PrintTo(const UnmanagedPoolAllocator& param, std::ostream* os)
        {
            *os << "Managed Pool Allocator (Pool Size: " << param.bufferSize << ", Alignment: " << param.alignment
                << ", Chunk Size: " << param.chunkSize << ", Buffer: " << reinterpret_cast<uintptr_t>(param.buffer)
                << ")";
        }
    };


    /// @brief Parameters for Pool allocator's alignment testing.
    struct PoolAllocatorAlignmentParams
    {
        size_t poolSize, chunkSize, alignment;

        friend void PrintTo(const PoolAllocatorAlignmentParams& param, std::ostream* os)
        {
            *os << "Pool Size: " << param.poolSize << ", Alignment: " << param.alignment
                << ", Chunk Size: " << param.chunkSize;
        }
    };

    /// @brief Parameterized test fixture for @ref pmm::Pool<> base address and chunk size alignment.
    class PoolAllocatorAlignment: public testing::TestWithParam<PoolAllocatorAlignmentParams>
    {};
    INSTANTIATE_TEST_SUITE_P(
        PoolAllocatorTests, PoolAllocatorAlignment,
        testing::Values(PoolAllocatorAlignmentParams{ .poolSize = 4_KB, .chunkSize = 24, .alignment = 8 },
                        PoolAllocatorAlignmentParams{ .poolSize = 5123, .chunkSize = 32, .alignment = 32 },
                        PoolAllocatorAlignmentParams{ .poolSize = 9582, .chunkSize = 127, .alignment = 64 },
                        PoolAllocatorAlignmentParams{ .poolSize = 20_MB, .chunkSize = 2_KB, .alignment = 4_KB }));



#ifndef NDEBUG
    /// @brief Parameterized test fixture for @ref pmm::Pool<> constructor pool size assertions.
    class PoolAllocatorCtorAssertions: public testing::TestWithParam<PoolAllocatorAlignmentParams>
    {};
    INSTANTIATE_TEST_SUITE_P(PoolAllocatorCtorDeathTests, PoolAllocatorCtorAssertions,
                             testing::Values(
                                 PoolAllocatorAlignmentParams{
                                     .poolSize  = 4_KB,
                                     .chunkSize = 5_KB,
                                     .alignment = 8,
                                 },
                                 PoolAllocatorAlignmentParams{
                                     .poolSize  = 4_KB,
                                     .chunkSize = 4_KB,
                                     .alignment = 2_KB,
                                 },
                                 PoolAllocatorAlignmentParams{
                                     .poolSize  = 8,
                                     .chunkSize = 9,
                                     .alignment = 8,
                                 },
                                 PoolAllocatorAlignmentParams{
                                     .poolSize  = 20,
                                     .chunkSize = 19,
                                     .alignment = 8,
                                 }));
#endif


} // namespace


/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

#ifndef NDEBUG

/**
 * @test Verify that managed pool allocator triggers assertions when memory allocator cannot fit
 *       at least one chunk in the pool.
 */
TEST_P(PoolAllocatorCtorAssertions, Managed_InadequateChunkSize_TriggersAssertionInDebugMode)
{
    const auto [poolSize, chunkSize, alignment] = GetParam();
    EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Pool<>{ poolSize, chunkSize, alignment }), "");
}


/**
 * @test Verify that unmanaged pool allocator triggers assertions when memory allocator cannot fit
 *       at least one chunk in the pool.
 */
TEST_P(PoolAllocatorCtorAssertions, Unmanaged_InadequateChunkSize_TriggersAssertionInDebugMode)
{
    const auto [poolSize, chunkSize, alignment] = GetParam();
    const auto buffer                           = new uint8_t[poolSize];
    EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Pool<pmm::UnmanagedMemory>{ buffer, poolSize, chunkSize, alignment }),
                       "");
    delete[] buffer;
}
#endif



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
        const auto buffer                           = new uint8_t[poolSize];
        const Pool<UnmanagedMemory> pool{ buffer, poolSize, chunkSize, alignment };
        const auto startAddress = reinterpret_cast<uintptr_t>(pool._buffer) + pool._initialAlignmentPadding;

        EXPECT_EQ(0, startAddress % alignment);
    }


    TEST_P(PoolAllocatorAlignment, Unmanaged_Ctor_AlignsChunksize)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const auto buffer                           = new uint8_t[poolSize];
        const Pool<UnmanagedMemory> pool{ buffer, poolSize, chunkSize, alignment };
        EXPECT_EQ(0, pool._chunkSize % alignment);

        delete[] buffer;
    }
} // namespace pmm
