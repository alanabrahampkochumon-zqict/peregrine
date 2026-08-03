/**
 * @file DeathTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Verifies pool allocation's assertions trigger correctly.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Utils.h"

#include <gtest/gtest.h>
#include <peregrine/allocators/Pool.h>
#include <peregrine/utils/Constants.h>



/**
 * TODO: Remove
 * @test Test dummy to ensure that the c++ program runs since
 *       applications require and entry point and death tests are
 *       triggered in debug mode only.
 */
// TEST(PeregrineDeathTestDummy, RunsTests) { EXPECT_EQ(1, 1); }

// NOTE: These tests only run with assertions on, which is only possible in debug mode.

#ifndef NDEBUG

namespace
{
    /**************************************
     *                                    *
     *             TEST SETUP             *
     *                                    *
     **************************************/
    using namespace pmm::constants;

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

    /// @brief Test fixture for @ref pmm::Pool<pmm::ManagedMemory> death tests.
    struct ManagedPoolAllocator: public testing::Test
    {
        size_t poolSize{ 2_KB }, chunkSize{ 8 }, alignment{ 8 };
        pmm::Pool<pmm::ManagedMemory> pool{ poolSize, chunkSize, alignment };

        [[maybe_unused]] friend void PrintTo(const ManagedPoolAllocator& param, std::ostream* os)
        {
            *os << "Managed Pool Allocator (Pool Size: " << param.poolSize << ", Alignment: " << param.alignment
                << ", Chunk Size: " << param.chunkSize << ")";
        }
    };


    /// @brief Test fixture for @ref pmm::Pool<pmm::UnmanagedMemory> death tests.
    struct UnmanagedPoolAllocator: public testing::Test
    {
        size_t bufferSize{ 2_KB }, chunkSize{ 8 }, alignment{ 8 };
        uint8_t* buffer = new uint8_t[bufferSize];
        pmm::Pool<pmm::UnmanagedMemory> pool{ buffer, bufferSize, chunkSize, alignment };

        [[maybe_unused]] friend void PrintTo(const UnmanagedPoolAllocator& param, std::ostream* os)
        {
            *os << "Managed Pool Allocator (Pool Size: " << param.bufferSize << ", Alignment: " << param.alignment
                << ", Chunk Size: " << param.chunkSize << ", Buffer: " << reinterpret_cast<uintptr_t>(param.buffer)
                << ")";
        }

        void TearDown() override { delete[] buffer; }
    };


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
                                     .poolSize  = 8,
                                     .chunkSize = 9,
                                     .alignment = 8,
                                 },
                                 PoolAllocatorAlignmentParams{
                                     .poolSize  = 20,
                                     .chunkSize = 19,
                                     .alignment = 8,
                                 }));

} // namespace


/**
 * @test Verify that managed pool allocator triggers assertions when memory allocator cannot fit
 *       at least one chunk in the pool.
 */
TEST_P(PoolAllocatorCtorAssertions, Managed_InadequateChunkSize_TriggersAssertionInDebugMode)
{
    const auto [poolSize, chunkSize, alignment] = GetParam();
    EXPECT_DEBUG_DEATH(static_cast<void>(pmm::Pool<>{ poolSize, chunkSize, alignment }), "");
}


TEST_F(ManagedPoolAllocator, Alloc_AllocatingObjectWithSizeGreaterThanChunkSizeTriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f)), ""); }


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


TEST_F(UnmanagedPoolAllocator, Alloc_AllocatingObjectWithSizeGreaterThanChunkSizeTriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f)), ""); }

#endif
