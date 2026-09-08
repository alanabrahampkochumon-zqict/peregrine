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
        static constexpr size_t offset = 10;

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

    private:
        uint8_t* buffer;
        size_t memorySize;
        Bitmask_t flBitmask;                                /// First level bitmap
        std::array<Bitmask_t, sizeof(flBitmask)> slBitmask; /// Second Level Bitmaps
    };

} // namespace pmm

#include "TLSF.tpp"
