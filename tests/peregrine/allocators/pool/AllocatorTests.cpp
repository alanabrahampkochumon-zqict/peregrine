/**
 * @file AllocatorTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 27, 2026
 *
 * @brief Verifies pool allocator logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "PoolTestSetup.h"



/**
 * @addtogroup T_PMM_Pool
 * @{
 */

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


    /// @brief Parameterized test fixture for @ref pmm::Pool<> base address and chunk size alignment.
    class PoolAllocatorAlignment: public testing::TestWithParam<PoolAllocatorAlignmentParams>
    {};
    INSTANTIATE_TEST_SUITE_P(
        PoolAllocatorTests, PoolAllocatorAlignment,
        testing::Values(PoolAllocatorAlignmentParams{ .poolSize = 4_KB, .chunkSize = 24, .alignment = 8 },
                        PoolAllocatorAlignmentParams{ .poolSize = 5123, .chunkSize = 32, .alignment = 32 },
                        PoolAllocatorAlignmentParams{ .poolSize = 9582, .chunkSize = 127, .alignment = 64 },
                        PoolAllocatorAlignmentParams{ .poolSize = 20_MB, .chunkSize = 2_KB, .alignment = 4_KB }));

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


TEST_F(ManagedPoolAllocator, FreeChunk_FreesBufferForNewerAllocations)
{
    std::vector<size_t*> data;
    // Allocate some buffer and fill it with data.
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        data.push_back(pool.alloc<size_t>(i * 11 + 37));
    }

    // Free in the elements
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        // Data[i] stores the address so free the current chunk
        // free order shouldn't matter as we are using an internal freelist
        // rather than a strict structure like LIFO or FIFO
        pool.freeChunk(data[i]);
    }

    // clear the vector
    data.clear();
    // Allocate the same count of buffer, and fill with data(different pattern)
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        data.push_back(pool.alloc<size_t>(i * 13 + 101));
    }

    // Verify the data
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 13 + 101, *data[i]);
    }
}


TEST_F(ManagedPoolAllocator, Free_FreesBufferForNewerAllocations)
{
    std::vector<size_t*> data;
    // Allocate some buffer and fill it with data.
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        data.push_back(pool.alloc<size_t>(i * 11 + 37));
    }

    // Free in the elements
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        // Data[i] stores the address so free the current chunk
        // free order shouldn't matter as we are using an internal freelist
        // rather than a strict structure like LIFO or FIFO
        pool.free(data[i]);
    }

    // clear the vector
    data.clear();
    // Allocate the same count of buffer, and fill with data(different pattern)
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        data.push_back(pool.alloc<size_t>(i * 13 + 101));
    }

    // Verify the data
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 13 + 101, *data[i]);
    }
}


TEST_F(ManagedPoolAllocator, Free_OnNonTrivialTypesCallsDtor)
{
    int dtorInvocationCount = 0;
    const auto typedMemory  = pool.alloc<DestructionTracker>(&dtorInvocationCount);
    pool.free(typedMemory);
    EXPECT_EQ(1, dtorInvocationCount);
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
        const auto chunk = static_cast<size_t*>(pool.allocChunk());
        *chunk           = i * 11 + 37;
        data.push_back(chunk);
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
        data.push_back(pool.alloc<size_t>(i * 11 + 37));
    }

    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 11 + 37, *data[i]);
    }
}


TEST_F(UnmanagedPoolAllocator, FreeChunk_FreesBufferForNewerAllocations)
{
    std::vector<size_t*> data;
    // Allocate some buffer and fill it with data.
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        data.push_back(pool.alloc<size_t>(i * 11 + 37));
    }

    // Free in the elements
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        // Data[i] stores the address so free the current chunk
        // free order shouldn't matter as we are using an internal freelist
        // rather than a strict structure like LIFO or FIFO
        pool.freeChunk(data[i]);
    }

    // clear the vector
    data.clear();
    // Allocate the same count of buffer, and fill with data(different pattern)
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        data.push_back(pool.alloc<size_t>(i * 13 + 101));
    }

    // Verify the data
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 13 + 101, *data[i]);
    }
}



TEST_F(UnmanagedPoolAllocator, Free_FreesBufferForNewerAllocations)
{
    std::vector<size_t*> data;
    // Allocate some buffer and fill it with data.
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        data.push_back(pool.alloc<size_t>(i * 11 + 37));
    }

    // Free in the elements
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        // Data[i] stores the address so free the current chunk
        // free order shouldn't matter as we are using an internal freelist
        // rather than a strict structure like LIFO or FIFO
        pool.free(data[i]);
    }

    // clear the vector
    data.clear();
    // Allocate the same count of buffer, and fill with data(different pattern)
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        data.push_back(pool.alloc<size_t>(i * 13 + 101));
    }

    // Verify the data
    for (size_t i = 0; i < pool.getMaxAllocationCount(); ++i)
    {
        EXPECT_EQ(i * 13 + 101, *data[i]);
    }
}


TEST_F(UnmanagedPoolAllocator, Free_OnNonTrivialTypesCallsDtor)
{
    int dtorInvocationCount = 0;
    const auto typedMemory  = pool.alloc<DestructionTracker>(&dtorInvocationCount);
    pool.free(typedMemory);
    EXPECT_EQ(1, dtorInvocationCount);
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

/** @} */
