/**
 * @file DeathTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Verifies pool allocator's assertions trigger correctly in DEBUG MODE.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "PoolTestSetup.h"


/**
 * @addtogroup T_PMM_Pool
 * @{
 */

// NOTE: These tests only run with assertions on, which is only possible in debug mode.
#ifdef ENABLE_PMM_DEATH_TESTS

namespace
{
    /**************************************
     *                                    *
     *             TEST SETUP             *
     *                                    *
     **************************************/
    using namespace pmm::constants;

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


TEST_F(ManagedPoolAllocator, FreeChunk_NullptrTriggersAssertion) { EXPECT_DEBUG_DEATH(pool.freeChunk(nullptr), ""); }


TEST_F(ManagedPoolAllocator, Free_NullptrTriggersAssertion) { EXPECT_DEBUG_DEATH(pool.freeChunk(nullptr), ""); }


TEST_F(ManagedPoolAllocator, FreeChunk_GreaterThanMaxMemoryAddressTriggersAssertion)
{
    // Since we arranging memory from back to front
    // the first allocation holds the largest allocation memory address
    // so we can try to free 1 + last address which should trigger assertion
    const auto lastAddressChunk = reinterpret_cast<uintptr_t>(pool.allocChunk());
    EXPECT_DEBUG_DEATH(pool.freeChunk(reinterpret_cast<void*>(lastAddressChunk + 1)), "");
}


TEST_F(ManagedPoolAllocator, Free_SmallerThanMinimumMemoryAddressTriggersAssertion)
{
    // Due to back to front arrangement, we need to allocate full buffer and try to free
    // 1 byte beyond the last buffer's address
    uintptr_t firstAddressChunk = 0;
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        firstAddressChunk = reinterpret_cast<uintptr_t>(pool.allocChunk());
    }
    EXPECT_NE(0, firstAddressChunk);
    EXPECT_DEBUG_DEATH(pool.freeChunk(reinterpret_cast<void*>(firstAddressChunk - 1)), "");
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


TEST_F(UnmanagedPoolAllocator, Alloc_AllocatingObjectWithSizeGreaterThanChunkSizeTriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f)), ""); }


TEST_F(UnmanagedPoolAllocator, FreeChunk_NullptrTriggersAssertion) { EXPECT_DEBUG_DEATH(pool.freeChunk(nullptr), ""); }


TEST_F(UnmanagedPoolAllocator, Free_NullptrTriggersAssertion) { EXPECT_DEBUG_DEATH(pool.freeChunk(nullptr), ""); }


TEST_F(UnmanagedPoolAllocator, FreeChunk_GreaterThanMaxMemoryAddressTriggersAssertion)
{
    // Since we arranging memory from back to front
    // the first allocation holds the largest allocation memory address
    // so we can try to free 1 + last address which should trigger assertion
    const auto lastAddressChunk = reinterpret_cast<uintptr_t>(pool.allocChunk());
    EXPECT_DEBUG_DEATH(pool.freeChunk(reinterpret_cast<void*>(lastAddressChunk + 1)), "");
}


TEST_F(UnmanagedPoolAllocator, Free_SmallerThanMinimumMemoryAddressTriggersAssertion)
{
    // Due to back to front arrangement, we need to allocate full buffer and try to free
    // 1 byte beyond the last buffer's address
    uintptr_t firstAddressChunk = 0;
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        firstAddressChunk = reinterpret_cast<uintptr_t>(pool.allocChunk());
    }
    EXPECT_NE(0, firstAddressChunk);
    EXPECT_DEBUG_DEATH(pool.freeChunk(reinterpret_cast<void*>(firstAddressChunk - 1)), "");
}

#endif

/** @} */
