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

#endif
