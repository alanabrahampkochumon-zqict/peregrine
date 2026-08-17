/**
 * @file SafePoolTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 11, 2026
 *
 * @brief Verify managed and unmanaged safe pool operation for edge cases like freeing a nullptr in release mode.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#ifndef ENABLE_PMM_DEATH_TESTS

    #include "Mocks.h"
    #include "Utils.h"

    #include <gtest/gtest.h>
    #include <peregrine/allocators/Pool.h>
    #include <peregrine/utils/Constants.h>


namespace
{
    /**************************************
     *            TEST SETUP              *
     **************************************/
    using namespace pmm::constants;

    /**
     * @brief Test fixture for managed safe @ref pmm::Pool.
     */
    class ManagedSafePoolTests: public testing::Test
    {
    public:
        size_t poolSize{ 2_KB };
        static constexpr size_t chunkSize{ 1_KB };
        pmm::Pool<pmm::ManagedMemory, pmm::telemetry::Enabled, true> pool{ poolSize, chunkSize, 8 };
    };


    /**
     * @brief Test fixture for managed safe @ref pmm::Pool.
     */
    class UnmanagedSafePoolTests: public testing::Test
    {
    public:
        size_t poolSize{ 2_KB };
        uint8_t* buffer = new uint8_t[poolSize];
        static constexpr size_t chunkSize{ 1_KB };
        pmm::Pool<pmm::UnmanagedMemory, pmm::telemetry::Enabled, true> pool{ buffer, poolSize, chunkSize, 8 };

    protected:
        void TearDown() override { delete[] buffer; }
    };


} // namespace



/**************************************
 *           MANAGED POOL             *
 **************************************/

TEST_F(ManagedSafePoolTests, AllocBytes_ReturnsNullPtrWhenAllocatingInAFullPool)
{
    // Use the full capacity
    // Since each chunk is 1_KB, we can allocate 2 of those utmost
    // and any further allocation must return a nullptr.
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());

    EXPECT_EQ(nullptr, pool.allocChunk());
}


TEST_F(ManagedSafePoolTests, Alloc_ReturnsNullPtrWhenAllocatingInAFullPool)
{
    // Use the full capacity
    // Since each chunk is 1_KB, we can allocate 2 of those utmost
    // and any further allocation must return a nullptr.
    static_cast<void>(pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>());
    static_cast<void>(pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>());

    const auto vec = pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(ManagedSafePoolTests, FreeChunk_AddressLessThanBaseAddressReturnsFalse)
{
    const auto allocation = static_cast<uint8_t*>(pool.allocChunk());
    // NOTE: Since we are allocating backwards we cannot offset by 1 chunkSize,
    // since it can return a valid offset as out allocatedAddress - chunkSize can point
    // to the base address
    // BASE_ADDRESS <- ALLOCATION
    const auto outOfBoundsLocation = allocation - 2 * chunkSize;
    EXPECT_FALSE(pool.freeChunk(outOfBoundsLocation));
}

TEST_F(ManagedSafePoolTests, FreeChunk_AddressGreaterThanMaxAddressReturnsFalse)
{
    const auto allocation          = static_cast<uint8_t*>(pool.allocChunk());
    const auto outOfBoundsLocation = allocation + poolSize + 24;

    EXPECT_FALSE(pool.freeChunk(outOfBoundsLocation));
}


TEST_F(ManagedSafePoolTests, FreeChunk_AddressesInBetweenChunkSizeReturnsFalse)
{
    const auto allocation        = static_cast<uint8_t*>(pool.allocChunk());
    const auto inBetweenLocation = allocation + 24;

    EXPECT_FALSE(pool.freeChunk(inBetweenLocation));
}

TEST_F(ManagedSafePoolTests, FreeChunk_NullptrReturnsFalse) { EXPECT_FALSE(pool.freeChunk(nullptr)); }


TEST_F(ManagedSafePoolTests, Free_AddressLessThanBaseAddressReturnsFalse)
{
    const auto allocation          = pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>();
    const auto outOfBoundsLocation = allocation - chunkSize;

    EXPECT_FALSE(pool.freeChunk(outOfBoundsLocation));
}

TEST_F(ManagedSafePoolTests, Free_AddressGreaterThanMaxAddressReturnsFalse)
{
    const auto allocation          = pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>();
    const auto outOfBoundsLocation = allocation + poolSize + 24;

    EXPECT_FALSE(pool.freeChunk(outOfBoundsLocation));
}


TEST_F(ManagedSafePoolTests, Free_AddressesInBetweenChunkSizeReturnsFalse)
{
    const auto allocation        = pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>();
    const auto inBetweenLocation = allocation + 24;

    EXPECT_FALSE(pool.freeChunk(inBetweenLocation));
}

/// Note: We can't directly pass in a nullptr so we are casting it
TEST_F(ManagedSafePoolTests, Free_NullptrReturnsFalse) { EXPECT_FALSE(pool.free(static_cast<uint8_t*>(nullptr))); }

/// Free tests


/**************************************
 *          UNMANAGED POOL            *
 **************************************/
TEST_F(UnmanagedSafePoolTests, AllocBytes_ReturnsNullPtrWhenAllocatingInAFullPool)
{
    // Use the full capacity
    // Since each chunk is 1_KB, we can allocate 2 of those utmost
    // and any further allocation must return a nullptr.
    static_cast<void>(pool.allocChunk());
    static_cast<void>(pool.allocChunk());

    EXPECT_EQ(nullptr, pool.allocChunk());
}


TEST_F(UnmanagedSafePoolTests, Alloc_ReturnsNullPtrWhenAllocatingInAFullPool)
{
    // Use the full capacity
    // Since each chunk is 1_KB, we can allocate 2 of those utmost
    // and any further allocation must return a nullptr.
    static_cast<void>(pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>());
    static_cast<void>(pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>());

    const auto vec = pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(nullptr, vec);
}


TEST_F(UnmanagedSafePoolTests, FreeChunk_AddressLessThanBaseAddressReturnsFalse)
{
    const auto allocation = static_cast<uint8_t*>(pool.allocChunk());
    // NOTE: Since we are allocating backwards we cannot offset by 1 chunkSize,
    // since it can return a valid offset as out allocatedAddress - chunkSize can point
    // to the base address
    // BASE_ADDRESS <- ALLOCATION
    const auto outOfBoundsLocation = allocation - 2 * chunkSize;
    EXPECT_FALSE(pool.freeChunk(outOfBoundsLocation));
}


TEST_F(UnmanagedSafePoolTests, FreeChunk_AddressGreaterThanMaxAddressReturnsFalse)
{
    const auto allocation          = static_cast<uint8_t*>(pool.allocChunk());
    const auto outOfBoundsLocation = allocation + poolSize + 24;

    EXPECT_FALSE(pool.freeChunk(outOfBoundsLocation));
}


TEST_F(UnmanagedSafePoolTests, FreeChunk_AddressesInBetweenChunkSizeReturnsFalse)
{
    const auto allocation        = static_cast<uint8_t*>(pool.allocChunk());
    const auto inBetweenLocation = allocation + 24;

    EXPECT_FALSE(pool.freeChunk(inBetweenLocation));
}


TEST_F(UnmanagedSafePoolTests, FreeChunk_NullptrReturnsFalse) { EXPECT_FALSE(pool.freeChunk(nullptr)); }


TEST_F(UnmanagedSafePoolTests, Free_AddressLessThanBaseAddressReturnsFalse)
{
    const auto allocation          = pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>();
    const auto outOfBoundsLocation = allocation - chunkSize;

    EXPECT_FALSE(pool.freeChunk(outOfBoundsLocation));
}

TEST_F(UnmanagedSafePoolTests, Free_AddressGreaterThanMaxAddressReturnsFalse)
{
    const auto allocation          = pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>();
    const auto outOfBoundsLocation = allocation + poolSize + 24;

    EXPECT_FALSE(pool.freeChunk(outOfBoundsLocation));
}


TEST_F(UnmanagedSafePoolTests, Free_AddressesInBetweenChunkSizeReturnsFalse)
{
    const auto allocation        = pool.alloc<LargeData<chunkSize - sizeof(LargeData<chunkSize>)>>();
    const auto inBetweenLocation = allocation + 24;

    EXPECT_FALSE(pool.freeChunk(inBetweenLocation));
}

/// Note: We can't directly pass in a nullptr so we are casting it
TEST_F(UnmanagedSafePoolTests, Free_NullptrReturnsFalse) { EXPECT_FALSE(pool.free(static_cast<uint8_t*>(nullptr))); }

#endif
