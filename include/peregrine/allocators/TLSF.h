#pragma once
/**
 * @file TLSF.h
 * @author Alan Abraham P Kochumon
 * @date Created on: September 07, 2026
 *
 * @brief Constant Time Dynamic Memory Allocator using TLSF(Two Level Segregated Fit).
 *
 * @details http://www.gii.upv.es/tlsf/
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Policy.h"
#include "peregrine/utils/Preprocessors.h"

#include <array>
#include <bit>

namespace pmm
{

    // TODO: Add a thread safe variant using mutexes
    /**
     * @brief Constant time dynamic memory allocator.
     *
     * @tparam MemStrategy Memory management type. See @ref pmm::MemoryStrategy.
     * @tparam TelPolicy   Flag indicating whether or not telemetry is enabled for this arena. See @ref pmm::telemetry.
     * @tparam Safe        Flags an arena as safe, implying certain operations like resizing a `nullptr` are handled
     *                     gracefully when assertions are disabled. `False` by default to prevent any performance
     *                     stalls incurred by conditional checks.
     * @tparam MTPolicy    Flag hinting the instance handle multithreading safety. Default: @ref mt::NonThreadSafe.
     */
    template <MemoryStrategy MemStrategy = ManagedMemory, telemetry::TelemetryPolicy TelPolicy = telemetry::Enabled,
              bool Safe = false, mt::MTPolicy MTPolicy = mt::NonThreadSafe>
    class TLSF
    {
    public:
        using Bitmask_t = uint64_t; /// Data type used for bitmasks
        /// The offset used for LSB(Least Significant Bit) of FL.
        /// Since we are using 64-bit integrals for SL, the difference between each
        /// This is the L value in the TLSF paper.
        static constexpr size_t LSB_OFFSET = 10;

        struct Header
        {
            size_t sizeWithFlags; /// The size of the memory block with 2 LSB used for flags.

            /// Mask for manipulating the free bit.
            static constexpr size_t MASK_FREE = 0b01;
            /// Mask for manipulating the free bit for previous memory address.
            static constexpr size_t MASK_PREV_FREE = 0b10;
            /// Mask for all flag bits
            static constexpr size_t MASK_FLAGS = 0b11;
            /// Mask for getting the true block size.
            static constexpr size_t MASK_SIZE = ~MASK_FLAGS;

            /// Bit manipulator for marking this block as free.
            static constexpr size_t FREE_MAN_BIT = 0b1;
            /// Bit manipulator for marking this block as used.
            static constexpr size_t USED_MAN_BIT = ~0 - 1; // 0b1111...1110
            /// Bit manipulator for marking previous block as free.
            static constexpr size_t PREV_FREE_MAN_BIT = 0b10; // 0b0000...000010
            /// Bit manipulator for marking previous block as used.
            static constexpr size_t PREV_USED_MAN_BIT = ~0 - 0b10; // 0b1111...111111 - 0b10 = 0b1111...111101


            /// Return whether the current block is free.
            [[nodiscard]] PMM_INLINE constexpr bool isFree() const noexcept
            {
                // Block is free if the LSB is set to 1.
                return (sizeWithFlags & MASK_FREE) == MASK_FREE;
            }
            /// Return whether the previous block is free.
            [[nodiscard]] PMM_INLINE constexpr bool isPrevFree() const noexcept
            {
                // Previous block is free if bit before LSB is 1
                return (sizeWithFlags & MASK_PREV_FREE) == MASK_PREV_FREE;
            }
            /// Return the true size of the block.
            [[nodiscard]] PMM_INLINE constexpr size_t getSize() const noexcept { return sizeWithFlags & MASK_SIZE; }

            /// Set the size of the block to @p size.
            PMM_INLINE constexpr void setSize(const size_t size) noexcept
            { sizeWithFlags = size | (sizeWithFlags & MASK_FLAGS); }

            /// Mark the block as free.
            PMM_INLINE constexpr void markFree() noexcept
            {
                // To mark a block as free we need to turn on the LSB
                // so we just or the sizeWithFlag with 1
                sizeWithFlags |= FREE_MAN_BIT;
            }
            /// Mark the block as used.
            PMM_INLINE constexpr void markUsed() noexcept
            {
                // To mark a block as used we need to turn off the LSB
                // so we and it the binary equivalent of 0b1111...1110
                sizeWithFlags &= USED_MAN_BIT;
            }

            /// Mark the block previous as free.
            /// @note This doesn't manipulate the previous memory address, but only updates
            ///       internal state of current block.
            PMM_INLINE constexpr void markPrevFree() noexcept
            {
                // We can't directly manipulate the free bits so we need to mask that bit and update it to
                // 1 such that 0bxxxx...xxxx0x | 0b0000...000010 -> 0bxxxx...xxxx1x
                sizeWithFlags |= PREV_FREE_MAN_BIT;
            }

            /// Mark the block as used.
            /// @note This doesn't manipulate the previous memory address, but only updates
            ///       internal state of current block.
            PMM_INLINE constexpr void markPrevUsed() noexcept
            {
                // We can't directly manipulate the free bits so we need to mask that bit and update it to
                // 1 such that 0bxxxx...xxxx1x & 0b1111...111101 -> 0bxxxx...xxxx0x
                sizeWithFlags &= PREV_USED_MAN_BIT;
            }

            /// Internal doubly linked list node used for storing memory blocks of similar size in the
            /// same FL_SL bitmask.
            struct FreeNode
            {
                FreeNode* prev;
                FreeNode* next;
            };
        };


        /**
         * @brief Create a TLSF allocator with user managed memory.
         *
         * @param buffer     The buffer to use for allocations.
         * @param memorySize The size of the buffer, which will also acts as the allocator's size.
         *
         * @remarks API specialized for @ref pmm::UnmanagedMemory.
         */
        constexpr TLSF(uint8_t* buffer, size_t memorySize) noexcept
            requires std::same_as<MemStrategy, UnmanagedMemory>;


        /**
         * @brief Create a TLSF allocator with internally managed memory.
         *
         * @param allocatorSize The size of the allocator's size.
         *                      Will not be the true usable size due internal paddings
         *                      for headers and alignment.
         *
         * @remarks API specialized for @ref pmm::ManagedMemory.
         */
        explicit constexpr TLSF(size_t allocatorSize) noexcept
            requires std::same_as<MemStrategy, ManagedMemory>;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the TLSF allocator.
         */
        constexpr TLSF(const TLSF&) = delete;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the TLSF allocator.
         */
        constexpr TLSF& operator=(const TLSF&) = delete;


        /**
         * @brief Transfer a TLSF allocator's memory to a this object.
         *
         * @param[in,out] tlsf The TLSF allocator to move into this object.
         */
        constexpr TLSF(TLSF&& tlsf) noexcept;


        /**
         * @brief Transfer a TLSF allocator's memory to this object.
         *
         * @warning This will delete any buffers held by the allocator on the left hand side of the assingment.
         *
         * @param[in,out] tlsf The TLSF allocator to move into this object.
         *
         * @return The current TLSF allocator instance.
         */
        constexpr TLSF& operator=(TLSF&& tlsf) noexcept;


        /**
         * @brief TLSF Destructor.
         *
         * @note TODO: For clearing the TLSF allocator use @ref clear or
         *       use @ref free to free individual allocations.
         *
         * @warning Will not free the backing buffer since it is managed by the user.
         *
         * @remarks API specialized for @ref pmm::UnmanagedMemory.
         */
        constexpr ~TLSF() noexcept
            requires std::same_as<MemStrategy, UnmanagedMemory>
        = default;


        /**
         * @brief TLSF Destructor. Frees the internal buffer.
         *
         * @note TODO: For clearing the TLSF allocator use @ref clear or
         *       use @ref free to free individual allocations.
         *
         * @remarks API specialized for @ref pmm::ManagedMemory.
         */
        constexpr ~TLSF() noexcept
            requires std::same_as<MemStrategy, ManagedMemory>;


        /// Get the size of the allocator in bytes.
        constexpr size_t size() const noexcept;

        /// Get the amount of memory in use in bytes.
        constexpr size_t usedSize() const noexcept;

        /// Get the amount of free memory in bytes.
        constexpr size_t freeSize() const noexcept;


    private:
        uint8_t* _buffer;
        size_t _size, _usedSize;
        Bitmask_t _flBitmask;                                 /// First level bitmap
        std::array<Bitmask_t, sizeof(_flBitmask)> _slBitmask; /// Second Level Bitmaps

        /// Structure used for exchanging bit mask indices internally.
        struct BitmapIndices
        {
            Bitmask_t flIndex{}, slIndex{};
        };


        constexpr Bitmask_t findFL(const size_t blockSize) const noexcept
        {
            // floor(log_2(x)) which is the first one in the set
            // i.ie for 13(1011) -> 4
            // + 1 is required since countr_zero is zero indexed
            std::countr_zero(blockSize) + 1;
        }

        constexpr BitmapIndices mappingInsert(const size_t blockSize) const noexcept
        {
            BitmapIndices indices;
            // TODO: One question what if look into the SL map with fl index and if the map is zero then
            // return the next free index? Test after implementation since that will prevent a
            // branching in allocBytes.
            indices.flIndex = findFL(blockSize);
            // sl := (r right_shift (fl-L)) - 2^L
            indices.slIndex = blockSize >> (indices.flIndex - LSB_OFFSET) - (2 << LSB_OFFSET);

            return indices;
        }



#ifdef ENABLE_PMM_TESTS
    // FRIEND TEST macros for verifying internal states
    #include <gtest/gtest_prod.h>




        FRIEND_TEST(ManagedTLSFTests, MoveCtor_ClearsMovedTLSFsInternalBuffer);
        FRIEND_TEST(ManagedTLSFTests, MoveCtor_MovesBufferIntoNewObject);
        FRIEND_TEST(ManagedTLSFTests, MoveAssign_ClearsMovedTLSF);
        FRIEND_TEST(ManagedTLSFTests, MoveAssign_MovesBufferIntoNewObject);
        FRIEND_TEST(ManagedTLSFTests, MoveAssign_SelfAssignmentReturnsTheSameTLSF);
        FRIEND_TEST(ManagedTLSFTests, MoveAssign_DeletingOriginalTLSFDoNotDeleteTheNewTLSFsMemory);
        FRIEND_TEST(ManagedTLSFTests, AllocBytes_MovesPrevOffset);
        FRIEND_TEST(ManagedTLSFTests, Alloc_MovesPrevOffset);
        FRIEND_TEST(ManagedTLSFTests, AllocBytes_UpdatesTelemetryPadding);
        FRIEND_TEST(ManagedTLSFTests, Alloc_UpdatesTelemetryPadding);
        FRIEND_TEST(ManagedTLSFTests, AllocV_UpdatesTelemetryPadding);
        FRIEND_TEST(ManagedTLSFTests, Resize_LatestAllocationResizeBuffer);
        FRIEND_TEST(ManagedTLSFTests, Resize_LatestAllocationOnlyResizeByOffsetDifference);
        FRIEND_TEST(ManagedTLSFTests, Clear_ResetsOffsetToZero);
        FRIEND_TEST(ManagedTLSFTests, ZeroOut_ZeroesOutTheInternalBuffer);

        FRIEND_TEST(UnmanagedTLSFTests, MoveCtor_ClearsMovedTLSFsInternalBuffer);
        FRIEND_TEST(UnmanagedTLSFTests, MoveCtor_MovesBufferIntoNewObject);
        FRIEND_TEST(UnmanagedTLSFTests, MoveAssign_ClearsMovedTLSF);
        FRIEND_TEST(UnmanagedTLSFTests, MoveAssign_MovesBufferIntoNewObject);
        FRIEND_TEST(UnmanagedTLSFTests, MoveAssign_SelfAssignmentReturnsTheSameTLSF);
        FRIEND_TEST(UnmanagedTLSFTests, MoveAssign_DeletingOriginalTLSFDoNotDeleteTheNewTLSFsMemory);
        FRIEND_TEST(UnmanagedTLSFTests, AllocBytes_MovesPrevOffset);
        FRIEND_TEST(UnmanagedTLSFTests, Alloc_MovesPrevOffset);
        FRIEND_TEST(UnmanagedTLSFTests, AllocBytes_UpdatesTelemetryPadding);
        FRIEND_TEST(UnmanagedTLSFTests, Alloc_UpdatesTelemetryPadding);
        FRIEND_TEST(UnmanagedTLSFTests, AllocV_UpdatesTelemetryPadding);
        FRIEND_TEST(UnmanagedTLSFTests, Resize_LatestAllocationResizeBuffer);
        FRIEND_TEST(UnmanagedTLSFTests, Resize_LatestAllocationOnlyResizeByOffsetDifference);
        FRIEND_TEST(UnmanagedTLSFTests, Clear_ResetsOffsetToZero);
#endif
    };


} // namespace pmm

#include "TLSF.tpp"
