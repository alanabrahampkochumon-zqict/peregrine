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
    class PoolAllocatorAlignmentTests: public testing::TestWithParam<PoolAllocatorAlignmentParams>
    {};
    INSTANTIATE_TEST_SUITE_P(
        PoolAllocatorTests, PoolAllocatorAlignmentTests,
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
 *            MANAGED POOL            *
 **************************************/

TEST_F(ManagedPoolAllocatorTests, AllocChunk_AllocatesDistinctBuffer)
{
    constexpr auto expectedValue = 1230480231401234812;
    const auto integer           = static_cast<size_t*>(pool.allocChunk());
    *integer                     = expectedValue;
    EXPECT_EQ(expectedValue, *integer);
}


TEST_F(ManagedPoolAllocatorTests, AllocChunk_CanAllocateMaximumPossibleChunkCountWithoutOverlap)
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


TEST_F(ManagedPoolAllocatorTests, Alloc_AllocatesBufferOfSizeSize)
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


TEST_F(ManagedPoolAllocatorTests, FreeChunk_FreesBufferForNewerAllocations)
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
        EXPECT_TRUE(pool.freeChunk(data[i]));
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


TEST_F(ManagedPoolAllocatorTests, Free_FreesBufferForNewerAllocations)
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
        EXPECT_TRUE(pool.free(data[i]));
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


TEST_F(ManagedPoolAllocatorTests, Free_OnNonTrivialTypesCallsDtor)
{
    int dtorInvocationCount = 0;
    const auto typedMemory  = pool.alloc<DestructionTracker>(&dtorInvocationCount);
    EXPECT_TRUE(pool.free(typedMemory));
    EXPECT_EQ(1, dtorInvocationCount);
}




TEST_F(ManagedPoolAllocatorTests, MoveCtor_CopiesAttributesToNewObject)
{
    const auto prevAllocationCount = pool.getMaxAllocationCount();
    const pmm::Pool<> pool2        = std::move(pool);
    EXPECT_EQ(prevAllocationCount, pool2.getMaxAllocationCount());
    EXPECT_EQ(poolSize, pool2.getTelemetry().getPoolSize());
}


TEST_F(ManagedPoolAllocatorTests, MoveCtor_MovesTelemetry)
{
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());
    // Get the telemetry to ensure that the value is preserved when moving
    // DON'T get by reference as it will change internally
    const auto telemetry = pool.getTelemetry();

    const pmm::Pool<> pool2 = std::move(pool);

    // Checking for telemetry equality
    EXPECT_EQ(telemetry.getUsedSize(), pool2.getTelemetry().getUsedSize());
    EXPECT_EQ(telemetry.getFreeSize(), pool2.getTelemetry().getFreeSize());
    EXPECT_EQ(telemetry.getUsedAllocationCount(), pool2.getTelemetry().getUsedAllocationCount());
    EXPECT_EQ(telemetry.getMaxAllocationCount(), pool2.getTelemetry().getMaxAllocationCount());
}


TEST_F(ManagedPoolAllocatorTests, MoveAssign_CopiesAttributesToNewObject)
{
    const auto prevAllocationCount = pool.getMaxAllocationCount();

    pmm::Pool<> pool2(256, 16, 16);

    pool2 = std::move(pool);

    EXPECT_EQ(prevAllocationCount, pool2.getMaxAllocationCount());
    EXPECT_EQ(poolSize, pool2.getTelemetry().getPoolSize());
}


TEST_F(ManagedPoolAllocatorTests, MoveAssign_MovesTelemetry)
{
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());
    // Get the telemetry to ensure that the value is preserved when moving
    // DON'T get by reference as it will change internally
    const auto telemetry = pool.getTelemetry();

    pmm::Pool<> pool2(256, 16, 16);
    pool2 = std::move(pool);

    // Checking for telemetry equality
    EXPECT_EQ(telemetry.getUsedSize(), pool2.getTelemetry().getUsedSize());
    EXPECT_EQ(telemetry.getFreeSize(), pool2.getTelemetry().getFreeSize());
    EXPECT_EQ(telemetry.getUsedAllocationCount(), pool2.getTelemetry().getUsedAllocationCount());
    EXPECT_EQ(telemetry.getMaxAllocationCount(), pool2.getTelemetry().getMaxAllocationCount());
}



/**************************************
 *           UNMANAGED POOL           *
 **************************************/

TEST_F(UnmanagedPoolAllocatorTests, AllocChunk_AllocatesDistinctBuffer)
{
    constexpr auto expectedValue = 1230480231401234812;
    const auto integer           = static_cast<size_t*>(pool.allocChunk());
    *integer                     = expectedValue;
    EXPECT_EQ(expectedValue, *integer);
}


TEST_F(UnmanagedPoolAllocatorTests, AllocChunk_CanAllocateMaximumPossibleChunkCountWithoutOverlap)
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


TEST_F(UnmanagedPoolAllocatorTests, Alloc_AllocatesBufferOfSizeSize)
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


TEST_F(UnmanagedPoolAllocatorTests, FreeChunk_FreesBufferForNewerAllocations)
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
        EXPECT_TRUE(pool.freeChunk(data[i]));
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



TEST_F(UnmanagedPoolAllocatorTests, Free_FreesBufferForNewerAllocations)
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
        EXPECT_TRUE(pool.free(data[i]));
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


TEST_F(UnmanagedPoolAllocatorTests, Free_OnNonTrivialTypesCallsDtor)
{
    int dtorInvocationCount = 0;
    const auto typedMemory  = pool.alloc<DestructionTracker>(&dtorInvocationCount);
    EXPECT_TRUE(pool.free(typedMemory));
    EXPECT_EQ(1, dtorInvocationCount);
}


TEST_F(UnmanagedPoolAllocatorTests, MoveCtor_CopiesAttributesToNewObject)
{
    const auto prevAllocationCount              = pool.getMaxAllocationCount();
    const pmm::Pool<pmm::UnmanagedMemory> pool2 = std::move(pool);

    EXPECT_EQ(prevAllocationCount, pool2.getMaxAllocationCount());
    EXPECT_EQ(bufferSize, pool2.getTelemetry().getPoolSize());
}


TEST_F(UnmanagedPoolAllocatorTests, MoveCtor_MovesTelemetry)
{
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());
    // Get the telemetry to ensure that the value is preserved when moving
    // DON'T get by reference as it will change internally
    const auto telemetry = pool.getTelemetry();

    const pmm::Pool<pmm::UnmanagedMemory> pool2 = std::move(pool);

    // Checking for telemetry equality
    EXPECT_EQ(telemetry.getUsedSize(), pool2.getTelemetry().getUsedSize());
    EXPECT_EQ(telemetry.getFreeSize(), pool2.getTelemetry().getFreeSize());
    EXPECT_EQ(telemetry.getUsedAllocationCount(), pool2.getTelemetry().getUsedAllocationCount());
    EXPECT_EQ(telemetry.getMaxAllocationCount(), pool2.getTelemetry().getMaxAllocationCount());
}


TEST_F(UnmanagedPoolAllocatorTests, MoveAssign_CopiesAttributesToNewObject)
{
    const auto prevAllocationCount = pool.getMaxAllocationCount();
    const auto backingBuffer       = new uint8_t[256];
    pmm::Pool<pmm::UnmanagedMemory> pool2(backingBuffer, 256, 16, 16);

    pool2 = std::move(pool);

    EXPECT_EQ(prevAllocationCount, pool2.getMaxAllocationCount());
    EXPECT_EQ(bufferSize, pool2.getTelemetry().getPoolSize());

    delete[] backingBuffer;
}


TEST_F(UnmanagedPoolAllocatorTests, MoveAssign_MovesTelemetry)
{
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());
    // Get the telemetry to ensure that the value is preserved when moving
    // DON'T get by reference as it will change internally
    const auto telemetry     = pool.getTelemetry();
    const auto backingBuffer = new uint8_t[256];
    pmm::Pool<pmm::UnmanagedMemory> pool2(backingBuffer, 256, 16, 16);

    pool2 = std::move(pool);

    // Checking for telemetry equality
    EXPECT_EQ(telemetry.getUsedSize(), pool2.getTelemetry().getUsedSize());
    EXPECT_EQ(telemetry.getFreeSize(), pool2.getTelemetry().getFreeSize());
    EXPECT_EQ(telemetry.getUsedAllocationCount(), pool2.getTelemetry().getUsedAllocationCount());
    EXPECT_EQ(telemetry.getMaxAllocationCount(), pool2.getTelemetry().getMaxAllocationCount());

    delete[] backingBuffer;
}


/**************************************
 *                                    *
 *           FRIEND TESTS             *
 *                                    *
 **************************************/

namespace pmm
{

    /**************************************
     *           MANAGED POOL             *
     **************************************/

    TEST_F(ManagedPoolAllocatorTests, Ctor_InitializesMemberVariables)
    {
        EXPECT_EQ(poolSize, pool._poolSize);
        EXPECT_EQ(chunkSize, pool._chunkSize);
        EXPECT_EQ(alignment, pool._chunkAlignment);
        EXPECT_NE(nullptr, pool._buffer);

        // Invariant: This expects the previous assertions to pass
        const auto expectedChunkCount = (poolSize - pool._initialAlignmentPadding) / pool._chunkSize;
        EXPECT_EQ(expectedChunkCount, pool._chunkCount);
    }


    TEST_F(ManagedPoolAllocatorTests, Ctor_ClearsThePool)
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


    TEST_P(PoolAllocatorAlignmentTests, Managed_Ctor_AlignsBaseAddress)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const Pool<> pool{ poolSize, chunkSize, alignment };
        const auto startAddress = reinterpret_cast<uintptr_t>(pool._buffer) + pool._initialAlignmentPadding;

        EXPECT_EQ(0, startAddress % alignment);
    }


    TEST_P(PoolAllocatorAlignmentTests, Managed_Ctor_AlignsChunksize)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const Pool<> pool{ poolSize, chunkSize, alignment };

        EXPECT_EQ(0, pool._chunkSize % alignment);
    }

    TEST_F(ManagedPoolAllocatorTests, MoveCtor_ClearsMovedPool)
    {
        const Pool<> pool2 = std::move(pool);
        // NOLINT(bugprone-use-after-move)
        EXPECT_EQ(nullptr, pool._buffer);
        EXPECT_EQ(0, pool._poolSize);
        EXPECT_EQ(0, pool._chunkSize);
        EXPECT_EQ(0, pool._chunkAlignment);
        EXPECT_EQ(0, pool._initialAlignmentPadding);
        EXPECT_EQ(0, pool._chunkCount);
        EXPECT_EQ(nullptr, pool._head);
        EXPECT_EQ(0, pool.getTelemetry().getUsedAllocationCount());
    }


    TEST_F(ManagedPoolAllocatorTests, MoveCtor_MovesBufferIntoNewObject)
    {
        const auto initialBufferPtr               = pool._buffer;
        const auto initialChunkSize               = pool._chunkSize;
        const auto initialChunkAlignment          = pool._chunkAlignment;
        const auto initialInitialAlignmentPadding = pool._initialAlignmentPadding;
        const auto initialChunkCount              = pool._chunkCount;
        const auto initialHeadPtr                 = pool._head;


        const Pool<> pool2 = std::move(pool);

        EXPECT_EQ(initialBufferPtr, pool2._buffer);
        EXPECT_EQ(poolSize, pool2._poolSize);
        EXPECT_EQ(initialChunkSize, pool2._chunkSize);
        EXPECT_EQ(initialChunkAlignment, pool2._chunkAlignment);
        EXPECT_EQ(initialInitialAlignmentPadding, pool2._initialAlignmentPadding);
        EXPECT_EQ(initialChunkCount, pool2._chunkCount);
        EXPECT_EQ(initialHeadPtr, pool2._head);
        EXPECT_EQ(0, pool2.getTelemetry().getUsedAllocationCount());
    }


    TEST_F(ManagedPoolAllocatorTests, MoveAssign_ClearsMovedPool)
    {
        [[maybe_unused]] Pool<> pool2(256, 16, 16);

        static_cast<void>(pool2 = std::move(pool));

        EXPECT_EQ(nullptr, pool._buffer);
        EXPECT_EQ(0, pool._poolSize);
        EXPECT_EQ(0, pool._chunkSize);
        EXPECT_EQ(0, pool._chunkAlignment);
        EXPECT_EQ(0, pool._initialAlignmentPadding);
        EXPECT_EQ(0, pool._chunkCount);
        EXPECT_EQ(nullptr, pool._head);
        EXPECT_EQ(0, pool.getTelemetry().getUsedAllocationCount());
    }


    TEST_F(ManagedPoolAllocatorTests, MoveAssign_MovesBufferIntoNewObject)
    {
        const auto initialBufferPtr               = pool._buffer;
        const auto initialChunkSize               = pool._chunkSize;
        const auto initialChunkAlignment          = pool._chunkAlignment;
        const auto initialInitialAlignmentPadding = pool._initialAlignmentPadding;
        const auto initialChunkCount              = pool._chunkCount;
        const auto initialHeadPtr                 = pool._head;

        Pool<> pool2(256, 16, 16);

        pool2 = std::move(pool);


        EXPECT_EQ(initialBufferPtr, pool2._buffer);
        EXPECT_EQ(poolSize, pool2._poolSize);
        EXPECT_EQ(initialChunkSize, pool2._chunkSize);
        EXPECT_EQ(initialChunkAlignment, pool2._chunkAlignment);
        EXPECT_EQ(initialInitialAlignmentPadding, pool2._initialAlignmentPadding);
        EXPECT_EQ(initialChunkCount, pool2._chunkCount);
        EXPECT_EQ(initialHeadPtr, pool2._head);
        EXPECT_EQ(0, pool2.getTelemetry().getUsedAllocationCount());
    }


    TEST_F(ManagedPoolAllocatorTests, MoveAssign_SelfAssignmentReturnsTheSamePool)
    {
        const auto initialBufferPtr               = pool._buffer;
        const auto initialChunkSize               = pool._chunkSize;
        const auto initialChunkAlignment          = pool._chunkAlignment;
        const auto initialInitialAlignmentPadding = pool._initialAlignmentPadding;
        const auto initialChunkCount              = pool._chunkCount;
        const auto initialHeadPtr                 = pool._head;
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wself-move"
#endif
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-move"
#endif
        pool = std::move(pool);
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

        EXPECT_EQ(initialBufferPtr, pool._buffer);
        EXPECT_EQ(poolSize, pool._poolSize);
        EXPECT_EQ(initialChunkSize, pool._chunkSize);
        EXPECT_EQ(initialChunkAlignment, pool._chunkAlignment);
        EXPECT_EQ(initialInitialAlignmentPadding, pool._initialAlignmentPadding);
        EXPECT_EQ(initialChunkCount, pool._chunkCount);
        EXPECT_EQ(initialHeadPtr, pool._head);
    }


    TEST_F(ManagedPoolAllocatorTests, MoveAssign_DeletingOriginalPoolDoNotDeleteTheNewPoolsMemory)
    {
        Pool<> pool2(256, 16, 16);
        constexpr auto scopedPoolSize = 512;

        // The pool being moved is scoped
        {
            Pool<> scopedPool(scopedPoolSize, 16, 16);
            pool2 = std::move(scopedPool);
        }
        EXPECT_NE(nullptr, pool2._buffer);

        // Write arbitrary data into the buffer
        // NOTE: i % 255 ensures that uint8_t does not overflow
        for (uint32_t i = 0; i < scopedPoolSize; ++i)
        {
            pool2._buffer[i] = i % 255;
        }

        // Read the value from buffer
        for (uint32_t i = 0; i < scopedPoolSize / 4; i += 4)
        {
            EXPECT_EQ(i % 255, pool2._buffer[i]);
        }
    }


    TEST_F(ManagedPoolAllocatorTests, GetMaxAllocationCount_ReturnsChunkCount)
    { EXPECT_EQ(pool._chunkCount, pool.getMaxAllocationCount()); }


    TEST_F(ManagedPoolAllocatorTests, Clear_FillsTheMemoryWithChunkCountPoolFreeNodes)
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
     *          UNMANAGED POOL            *
     **************************************/

    TEST_F(UnmanagedPoolAllocatorTests, Ctor_InitializesMemberVariables)
    {
        EXPECT_EQ(bufferSize, pool._poolSize);
        EXPECT_EQ(chunkSize, pool._chunkSize);
        EXPECT_EQ(alignment, pool._chunkAlignment);
        EXPECT_EQ(buffer, pool._buffer);

        // Invariant: This expects the previous assertions to pass
        const auto expectedChunkCount = (bufferSize - pool._initialAlignmentPadding) / pool._chunkSize;
        EXPECT_EQ(expectedChunkCount, pool._chunkCount);
    }


    TEST_F(UnmanagedPoolAllocatorTests, Ctor_ClearsThePool)
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


    TEST_P(PoolAllocatorAlignmentTests, Unmanaged_Ctor_AlignsBaseAddress)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const auto buffer                           = new uint8_t[poolSize];
        const Pool<UnmanagedMemory> pool{ buffer, poolSize, chunkSize, alignment };
        const auto startAddress = reinterpret_cast<uintptr_t>(pool._buffer) + pool._initialAlignmentPadding;

        EXPECT_EQ(0, startAddress % alignment);
    }


    TEST_P(PoolAllocatorAlignmentTests, Unmanaged_Ctor_AlignsChunksize)
    {
        const auto [poolSize, chunkSize, alignment] = GetParam();
        const auto buffer                           = new uint8_t[poolSize];
        const Pool<UnmanagedMemory> pool{ buffer, poolSize, chunkSize, alignment };
        EXPECT_EQ(0, pool._chunkSize % alignment);

        delete[] buffer;
    }


    TEST_F(UnmanagedPoolAllocatorTests, MoveCtor_ClearsMovedPool)
    {
        [[maybe_unused]] const Pool<pmm::UnmanagedMemory> pool2 = std::move(pool);
        // NOLINT(bugprone-use-after-move)
        EXPECT_EQ(nullptr, pool._buffer);
        EXPECT_EQ(0, pool._poolSize);
        EXPECT_EQ(0, pool._chunkSize);
        EXPECT_EQ(0, pool._chunkAlignment);
        EXPECT_EQ(0, pool._initialAlignmentPadding);
        EXPECT_EQ(0, pool._chunkCount);
        EXPECT_EQ(nullptr, pool._head);
        EXPECT_EQ(0, pool.getTelemetry().getUsedAllocationCount());
    }


    TEST_F(UnmanagedPoolAllocatorTests, MoveCtor_MovesBufferIntoNewObject)
    {
        const auto initialBufferPtr               = pool._buffer;
        const auto initialChunkSize               = pool._chunkSize;
        const auto initialChunkAlignment          = pool._chunkAlignment;
        const auto initialInitialAlignmentPadding = pool._initialAlignmentPadding;
        const auto initialChunkCount              = pool._chunkCount;
        const auto initialHeadPtr                 = pool._head;


        const Pool<pmm::UnmanagedMemory> pool2 = std::move(pool);

        EXPECT_EQ(initialBufferPtr, pool2._buffer);
        EXPECT_EQ(bufferSize, pool2._poolSize);
        EXPECT_EQ(initialChunkSize, pool2._chunkSize);
        EXPECT_EQ(initialChunkAlignment, pool2._chunkAlignment);
        EXPECT_EQ(initialInitialAlignmentPadding, pool2._initialAlignmentPadding);
        EXPECT_EQ(initialChunkCount, pool2._chunkCount);
        EXPECT_EQ(initialHeadPtr, pool2._head);
        EXPECT_EQ(0, pool2.getTelemetry().getUsedAllocationCount());
    }


    TEST_F(UnmanagedPoolAllocatorTests, MoveAssign_ClearsMovedPool)
    {
        const auto backingBuffer = new uint8_t[256];
        [[maybe_unused]] Pool<pmm::UnmanagedMemory> pool2(backingBuffer, 256, 16, 16);

        static_cast<void>(pool2 = std::move(pool));

        EXPECT_EQ(nullptr, pool._buffer);
        EXPECT_EQ(0, pool._poolSize);
        EXPECT_EQ(0, pool._chunkSize);
        EXPECT_EQ(0, pool._chunkAlignment);
        EXPECT_EQ(0, pool._initialAlignmentPadding);
        EXPECT_EQ(0, pool._chunkCount);
        EXPECT_EQ(nullptr, pool._head);
        EXPECT_EQ(0, pool.getTelemetry().getUsedAllocationCount());

        delete[] backingBuffer;
    }


    TEST_F(UnmanagedPoolAllocatorTests, MoveAssign_MovesBufferIntoNewObject)
    {
        const auto initialBufferPtr               = pool._buffer;
        const auto initialChunkSize               = pool._chunkSize;
        const auto initialChunkAlignment          = pool._chunkAlignment;
        const auto initialInitialAlignmentPadding = pool._initialAlignmentPadding;
        const auto initialChunkCount              = pool._chunkCount;
        const auto initialHeadPtr                 = pool._head;

        const auto backingBuffer = new uint8_t[256];
        Pool<pmm::UnmanagedMemory> pool2(backingBuffer, 256, 16, 16);

        pool2 = std::move(pool);


        EXPECT_EQ(initialBufferPtr, pool2._buffer);
        EXPECT_EQ(bufferSize, pool2._poolSize);
        EXPECT_EQ(initialChunkSize, pool2._chunkSize);
        EXPECT_EQ(initialChunkAlignment, pool2._chunkAlignment);
        EXPECT_EQ(initialInitialAlignmentPadding, pool2._initialAlignmentPadding);
        EXPECT_EQ(initialChunkCount, pool2._chunkCount);
        EXPECT_EQ(initialHeadPtr, pool2._head);
        EXPECT_EQ(0, pool2.getTelemetry().getUsedAllocationCount());
        delete[] backingBuffer;
    }


    TEST_F(UnmanagedPoolAllocatorTests, MoveAssign_SelfAssignmentReturnsTheSamePool)
    {
        const auto initialBufferPtr               = pool._buffer;
        const auto initialChunkSize               = pool._chunkSize;
        const auto initialChunkAlignment          = pool._chunkAlignment;
        const auto initialInitialAlignmentPadding = pool._initialAlignmentPadding;
        const auto initialChunkCount              = pool._chunkCount;
        const auto initialHeadPtr                 = pool._head;
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wself-move"
#endif
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-move"
#endif
        pool = std::move(pool);
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

        EXPECT_EQ(initialBufferPtr, pool._buffer);
        EXPECT_EQ(bufferSize, pool._poolSize);
        EXPECT_EQ(initialChunkSize, pool._chunkSize);
        EXPECT_EQ(initialChunkAlignment, pool._chunkAlignment);
        EXPECT_EQ(initialInitialAlignmentPadding, pool._initialAlignmentPadding);
        EXPECT_EQ(initialChunkCount, pool._chunkCount);
        EXPECT_EQ(initialHeadPtr, pool._head);
    }


    TEST_F(UnmanagedPoolAllocatorTests, MoveAssign_DeletingOriginalPoolDoNotDeleteTheNewPoolsMemory)
    {
        constexpr auto scopedPoolSize = 512;
        const auto backingBuffer      = new uint8_t[256];
        const auto backingBuffer2     = new uint8_t[scopedPoolSize];
        Pool<pmm::UnmanagedMemory> pool2(backingBuffer, 256, 16, 16);

        // The pool being moved is scoped
        {
            Pool<pmm::UnmanagedMemory> scopedPool(backingBuffer2, scopedPoolSize, 16, 16);
            pool2 = std::move(scopedPool);
        }
        EXPECT_NE(nullptr, pool2._buffer);

        // Write arbitrary data into the buffer
        // NOTE: i % 255 ensures that uint8_t does not overflow
        for (uint32_t i = 0; i < scopedPoolSize; ++i)
        {
            pool2._buffer[i] = i % 255;
        }

        // Read the value from buffer
        for (uint32_t i = 0; i < scopedPoolSize / 4; i += 4)
        {
            EXPECT_EQ(i % 255, pool2._buffer[i]);
        }
        delete[] backingBuffer;
        delete[] backingBuffer2;
    }



    TEST_F(UnmanagedPoolAllocatorTests, GetMaxAllocationCount_ReturnsChunkCount)
    { EXPECT_EQ(pool._chunkCount, pool.getMaxAllocationCount()); }


    TEST_F(UnmanagedPoolAllocatorTests, Clear_FillsTheMemoryWithChunkCountPoolFreeNodes)
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
