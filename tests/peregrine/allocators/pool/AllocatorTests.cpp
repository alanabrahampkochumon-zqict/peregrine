/**
 * @file AllocatorTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 27, 2026
 *
 * @brief Verifies pool allocator logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Utils.h"

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
        size_t poolSize{ 2_KB }, chunkSize{ 8 }, alignment{ 8 };
        pmm::Pool<pmm::ManagedMemory> pool{ poolSize, chunkSize, alignment };

        [[maybe_unused]] friend void PrintTo(const ManagedPoolAllocator& param, std::ostream* os)
        {
            *os << "Managed Pool Allocator (Pool Size: " << param.poolSize << ", Alignment: " << param.alignment
                << ", Chunk Size: " << param.chunkSize << ")";
        }
    };


    /// @brief Test fixture for @ref pmm::Pool<pmm::UnmanagedMemory> tests.
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



#ifdef ENABLE_PMM_TESTS
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


    /**************************************
     *                                    *
     *           STATIC TESTS             *
     *                                    *
     **************************************/
    namespace static_tests
    {
        /** @test Verify that unmanaged pool does not free memory.
         *  @note Since we cant really confirm if a buffer is freed and we only delete[] buffer in the dtor of Pool,
         *        we can check if its trivially destructible to ensure memory is freed in the pool in unmanaged mode
         *        and opposite otherwise.
         */
        static_assert(std::is_trivially_destructible_v<pmm::Pool<pmm::UnmanagedMemory>> == true);
        static_assert(std::is_trivially_destructible_v<pmm::Pool<pmm::UnmanagedMemory>> == true);

        /// @test Verify that manged pool frees buffer it allocates.
        static_assert(std::is_trivially_destructible_v<pmm::Pool<pmm::ManagedMemory>> == false);
        static_assert(std::is_trivially_destructible_v<pmm::Pool<pmm::ManagedMemory>> == false);
    } // namespace static_tests

} // namespace



/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

#ifdef ENABLE_PMM_TESTS

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


/**************************************
 *                                    *
 *            MANAGED POOL            *
 *                                    *
 **************************************/

TEST_F(ManagedPoolAllocator, AllocChunk_AllocatesDistinctBuffer)
{
    constexpr auto expectedValue = 1230480231401234812;
    const auto integer           = static_cast<size_t*>(pool.allocChunk());
    *integer                     = expectedValue;
    EXPECT_EQ(expectedValue, *integer);
}


TEST_F(ManagedPoolAllocator, AllocChunk_CanAllocateMaximumPossibleChunkCountWithoutOverlap)
{
    std::vector<size_t*> data;

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        const auto buffer = static_cast<size_t*>(pool.allocChunk());
        *buffer           = i * 11 + 37;
        data.push_back(buffer);
    }

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 11 + 37, *data[i]);
    }
}


TEST_F(ManagedPoolAllocator, Alloc_AllocatesBufferOfSizeSize)
{
    std::vector<size_t*> data;

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        const auto buffer = pool.alloc<size_t>(i * 11 + 37);
        data.push_back(buffer);
    }

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 11 + 37, *data[i]);
    }
}



/**************************************
 *                                    *
 *           UNMANAGED POOL           *
 *                                    *
 **************************************/

TEST_F(UnmanagedPoolAllocator, AllocChunk_AllocatesDistinctBuffer)
{
    constexpr auto expectedValue = 1230480231401234812;
    const auto integer           = static_cast<size_t*>(pool.allocChunk());
    *integer                     = expectedValue;
    EXPECT_EQ(expectedValue, *integer);
}


TEST_F(UnmanagedPoolAllocator, AllocChunk_CanAllocateMaximumPossibleChunkCountWithoutOverlap)
{
    std::vector<size_t*> data;

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        const auto buffer = static_cast<size_t*>(pool.allocChunk());
        *buffer           = i * 11 + 37;
        data.push_back(buffer);
    }

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 11 + 37, *data[i]);
    }
}


TEST_F(UnmanagedPoolAllocator, Alloc_AllocatesBufferOfSizeSize)
{
    std::vector<size_t*> data;

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        const auto buffer = pool.alloc<size_t>(i * 11 + 37);
        data.push_back(buffer);
    }

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 11 + 37, *data[i]);
    }
}



/**************************************
 *                                    *
 *           FRIEND TESTS             *
 *                                    *
 **************************************/

namespace pmm
{

    /**************************************
     *                                    *
     *           MANAGED POOL             *
     *                                    *
     **************************************/

    TEST_F(ManagedPoolAllocator, Ctor_InitializesMemberVariables)
    {
        EXPECT_EQ(poolSize, pool._poolSize);
        EXPECT_EQ(chunkSize, pool._chunkSize);
        EXPECT_EQ(alignment, pool._chunkAlignment);
        EXPECT_NE(nullptr, pool._buffer);

        // Invariant: This expects the previous assertions to pass
        const auto expectedChunkCount = (poolSize - pool._initialAlignmentPadding) / pool._chunkSize;
        EXPECT_EQ(expectedChunkCount, pool._chunkCount);
    }


    TEST_F(ManagedPoolAllocator, Ctor_ClearsThePool)
    {
        // Theoretically, If the pool is cleared then the base address must have the header
        // with its next pointing to the base of next address
        // But by how our clear method works, each node stores the prior address not
        // the next address, Head -> Last Address -> Second Last Address -> etc..
        const auto baseAddress  = pool._buffer + pool._initialAlignmentPadding;
        const auto nextAddress  = baseAddress + chunkSize;
        const auto secondHeader = reinterpret_cast<PoolFreeNode*>(nextAddress);

        EXPECT_EQ(reinterpret_cast<uintptr_t>(baseAddress), reinterpret_cast<uintptr_t>(secondHeader->next));
    }




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


    TEST_F(ManagedPoolAllocator, GetMaxAllocationCount_ReturnsChunkCount)
    { EXPECT_EQ(pool._chunkCount, pool.getMaxAllocationCount()); }


    TEST_F(ManagedPoolAllocator, Clear_FillsTheMemoryWithChunkCountPoolFreeNodes)
    {
        const auto baseAddress = pool._buffer + pool._initialAlignmentPadding;

        // Since we store the pool nodes in a front to back manner,
        // we need to test whether the current header holds the previous address
        // starting from 1st index and comparing it to 0th mem address and so on.
        for (size_t i = 1; i < pool._chunkCount; ++i)
        {
            const auto currentHeader   = reinterpret_cast<PoolFreeNode*>(baseAddress + i * chunkSize);
            const auto previousAddress = reinterpret_cast<uintptr_t>(baseAddress + (i - 1) * chunkSize);

            EXPECT_EQ(previousAddress, reinterpret_cast<uintptr_t>(currentHeader->next));
        }
        // Finally checking if the node stored at base address points to nullptr
        const auto currentHeader = reinterpret_cast<PoolFreeNode*>(baseAddress);
        EXPECT_EQ(nullptr, currentHeader->next);
    }



    /**************************************
     *                                    *
     *          UNMANAGED POOL            *
     *                                    *
     **************************************/

    TEST_F(UnmanagedPoolAllocator, Ctor_InitializesMemberVariables)
    {
        EXPECT_EQ(bufferSize, pool._poolSize);
        EXPECT_EQ(chunkSize, pool._chunkSize);
        EXPECT_EQ(alignment, pool._chunkAlignment);
        EXPECT_EQ(buffer, pool._buffer);

        // Invariant: This expects the previous assertions to pass
        const auto expectedChunkCount = (bufferSize - pool._initialAlignmentPadding) / pool._chunkSize;
        EXPECT_EQ(expectedChunkCount, pool._chunkCount);
    }


    TEST_F(UnmanagedPoolAllocator, Ctor_ClearsThePool)
    {
        // If the pool is cleared then the base address must have the header
        // with its next pointing to the base of next address
        // But by how our clear method works, each node stores the prior address not
        // the next address, Head -> Last Address -> Second Last Address -> etc..
        const auto baseAddress  = pool._buffer + pool._initialAlignmentPadding;
        const auto nextAddress  = baseAddress + chunkSize;
        const auto secondHeader = reinterpret_cast<PoolFreeNode*>(nextAddress);

        EXPECT_EQ(reinterpret_cast<uintptr_t>(baseAddress), reinterpret_cast<uintptr_t>(secondHeader->next));
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


    TEST_F(UnmanagedPoolAllocator, GetMaxAllocationCount_ReturnsChunkCount)
    { EXPECT_EQ(pool._chunkCount, pool.getMaxAllocationCount()); }


    TEST_F(UnmanagedPoolAllocator, Clear_FillsTheMemoryWithChunkCountPoolFreeNodes)
    {
        const auto baseAddress = pool._buffer + pool._initialAlignmentPadding;

        // Since we store the pool nodes in a front to back manner,
        // we need to test whether the current header holds the previous address
        // starting from 1st index and comparing it to 0th mem address and so on.
        for (size_t i = 1; i < pool._chunkCount; ++i)
        {
            const auto currentHeader   = reinterpret_cast<PoolFreeNode*>(baseAddress + i * chunkSize);
            const auto previousAddress = reinterpret_cast<uintptr_t>(baseAddress + (i - 1) * chunkSize);

            EXPECT_EQ(previousAddress, reinterpret_cast<uintptr_t>(currentHeader->next));
        }
        // Finally checking if the node stored at base address points to nullptr
        const auto firstHeader = reinterpret_cast<PoolFreeNode*>(baseAddress);
        EXPECT_EQ(nullptr, firstHeader->next);
    }


} // namespace pmm
