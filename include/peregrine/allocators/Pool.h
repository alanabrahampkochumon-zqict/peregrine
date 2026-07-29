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

    private:
        uint8_t* _buffer;
        size_t _poolSize, _chunkSize, _chunkAlignment;



#ifdef ENABLE_PMM_TESTS
    // FRIEND TEST macros for verifying internal states
    #include <gtest/gtest_prod.h>



        FRIEND_TEST(ManagedPoolAllocator, Ctor_InitializesMemberVariables);


        FRIEND_TEST(UnmanagedPoolAllocator, Ctor_InitializesMemberVariables);

#endif
    };
}; // namespace pmm



#include "Pool.tpp"
