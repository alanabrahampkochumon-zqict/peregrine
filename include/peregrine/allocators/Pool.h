#pragma once
/**
 * @file Pool.h
 * @author Alan Abraham P Kochumon
 * @date Created on: July 27, 2026
 *
 * @brief Pool allocator for managing buffers of similar size.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Policy.h"

#include <cstdint>


namespace pmm
{

    /**
     * @brief Free list node for pool allocator.
     */
    struct PoolFreeNode
    {
        PoolFreeNode* next;
    };


    template <MemoryStrategy MemStrategy = ManagedMemory, telemetry::TelemetryPolicy TelPolicy = telemetry::Enabled>
    class Pool
    {
    public:
        /**
         * @brief Create a pool allocator with internal managed buffer.
         *
         * @param[in] poolSize       The total capacity of the pool allocator in bytes.
         * @param[in] chuckSize      The per fragment/chunk size of the pool in bytes.
         * @param[in] chunkAlignment The base alignment for each fragment/chunk.
         *
         * @remarks API specialized for @ref pmm::ManagedMemory.
         *
         * @warning This allocator is NOT thread-safe by default.
         */
        [[nodiscard]] explicit constexpr Pool(size_t poolSize, size_t chuckSize, size_t chunkAlignment) noexcept
            requires std::same_as<MemStrategy, ManagedMemory>;


        /**
         * @brief Create a pool allocator with unmanaged memory.
         *
         * @param[in,out] backingBuffer  The memory buffer to be used by the allocator.
         * @param[in] bufferSize         The size of the backing buffer in bytes.
         * @param[in] chuckSize          The per fragment/chunk size of the pool in bytes.
         * @param[in] chunkAlignment     The base alignment for each fragment/chunk.
         *
         * @remarks API specialized for @ref pmm::UnmanagedMemory.
         *
         * @warning This allocator is NOT thread-safe by default.
         */
        [[nodiscard]] explicit constexpr Pool(uint8_t* backingBuffer, size_t bufferSize, size_t chuckSize,
                                              size_t chunkAlignment) noexcept
            requires std::same_as<MemStrategy, UnmanagedMemory>;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the stack.
         */
        constexpr Pool(const Pool&) = delete;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the stack.
         */
        constexpr Pool& operator=(const Pool&) = delete;


        /**
         * @brief Free the entire pool, resetting to a fresh state.
         *
         * @relatedalso  free
         * @relatedalso  freeBytes
         */
        void clear();


        /**
         * @brief Stack Destructor. Frees the internal buffer.
         *
         * @note For clearing the Stack, use @ref clear, or to move free individual frames use @ref free.
         *
         * @remarks API specialized for @ref pmm::ManagedMemory.
         */
        ~Pool() noexcept
            requires std::same_as<MemStrategy, ManagedMemory>;


        /**
         * @brief Stack Destructor.
         *
         * @note For clearing the Stack, use @ref clear, or to move free individual frames use @ref free.
         *
         * @warning Will not clear free the backing buffer since its managed by the user.
         *
         * @remarks API specialized for @ref pmm::UnmanagedMemory.
         */
        ~Pool() noexcept
            requires std::same_as<MemStrategy, UnmanagedMemory>
        = default;



    private:
        uint8_t* _buffer;
        size_t _poolSize, _chunkSize, _chunkAlignment, _initialAlignmentPadding, _chunkCount;
        PoolFreeNode* _head;



#ifdef ENABLE_PMM_TESTS
    // FRIEND TEST macros for verifying internal states
    #include <gtest/gtest_prod.h>



        FRIEND_TEST(ManagedPoolAllocator, Ctor_InitializesMemberVariables);
        FRIEND_TEST(ManagedPoolAllocator, Ctor_ClearsThePool);
        FRIEND_TEST(ManagedPoolAllocator, Clear_FillsTheMemoryWithChunkCountPoolFreeNodes);

        FRIEND_TEST(PoolAllocatorAlignment, Managed_Ctor_AlignsBaseAddress);
        FRIEND_TEST(PoolAllocatorAlignment, Managed_Ctor_AlignsChunksize);


        FRIEND_TEST(UnmanagedPoolAllocator, Ctor_InitializesMemberVariables);
        FRIEND_TEST(UnmanagedPoolAllocator, Ctor_ClearsThePool);
        FRIEND_TEST(UnmanagedPoolAllocator, Clear_FillsTheMemoryWithChunkCountPoolFreeNodes);

        FRIEND_TEST(PoolAllocatorAlignment, Unmanaged_Ctor_AlignsBaseAddress);
        FRIEND_TEST(PoolAllocatorAlignment, Unmanaged_Ctor_AlignsChunksize);

#endif
    };
}; // namespace pmm



#include "Pool.tpp"
