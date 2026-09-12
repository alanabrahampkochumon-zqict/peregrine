#pragma once
/**
 * @file TLSF.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 08, 2026
 *
 * @brief Implmentation of member functions defined in TLSF.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "../memory/Memory.h"


namespace pmm
{
    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(uint8_t* buffer,
                                                                            const size_t memorySize) noexcept
        requires std::same_as<MemStrategy, UnmanagedMemory>
        : _buffer{ buffer }, _size{ memorySize }, _usedSize{ 0 }, _flBitmask{ 0 }, _slBitmask{}
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(const size_t allocatorSize) noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
        : _buffer{ static_cast<uint8_t*>(memAlloc(allocatorSize)) },
          _size{ allocatorSize },
          _usedSize{ 0 },
          _flBitmask{ 0 },
          _slBitmask{}
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(TLSF&& tlsf) noexcept
        : _buffer{ std::exchange(tlsf._buffer, nullptr) },
          _size{ tlsf._size },
          _usedSize{ tlsf._usedSize },
          _flBitmask{ tlsf._flBitmask }
    { std::move(tlsf._slBitmask.begin(), tlsf._slBitmask.end(), _slBitmask.begin()); }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>& TLSF<MemStrategy, TelPolicy, Safe,
                                                                            MTPolicy>::operator=(TLSF&& tlsf) noexcept
    {
        // If the memory is internally managed, we need to free it before moving over the other allocator's
        // internal buffer.
        if constexpr (std::is_same_v<MemStrategy, ManagedMemory>)
        {
            memFree(_buffer, _size);
        }
        // TODO: Update to assert if hampering performance
        if (this == &tlsf)
        {
            return *this;
        }

        _buffer    = std::exchange(tlsf._buffer, nullptr);
        _size      = tlsf._size;
        _usedSize  = tlsf._usedSize;
        _flBitmask = tlsf._flBitmask;
        std::move(tlsf._slBitmask.begin(), tlsf._slBitmask.end(), _slBitmask.begin());

        return *this;
    }



    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::~TLSF() noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
    { memFree(_buffer, _size); }


    /**************************************
     *             GETTERS                *
     **************************************/

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr size_t TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::size() const noexcept
    { return _size; }

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr size_t TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::usedSize() const noexcept
    { return _usedSize; }

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr size_t TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::freeSize() const noexcept
    { return _size - _usedSize; }



    /**************************************
     *         INTERNAL HELPERS           *
     **************************************/

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr typename TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::BitmapIndices TLSF<
        MemStrategy, TelPolicy, Safe, MTPolicy>::mappingInsert(size_t blockSize) noexcept
    {
        // The allocator always hands out memory in 64-byte chunks.
        // And the minimum chunk size we can insert into the freelist is 64-bytes
        // so, we assert that minimal value with debug asserts in unsafe mode or
        // round up it up to 64-bytes in safe mode.
        BitmapIndices indices;
        // TODO: Move rounding to safe mode only and introduce and assert for debug safety in
        //       non-safe mode.
        blockSize = std::max(MIN_BLOCK_SIZE, blockSize);
        // The flIndex can be found using floor(log_2(size)) and fls(First Last Set)
        // can be used to get the value using bit manipulation.
        const auto rawFL = utils::fls(blockSize);
        // The flIndex that we want to use needs to be offset by our FL_OFFSET.
        // This ensures that flIndex starts at FL_OFFSET(6 in our case) since going below that doesn't make sense for
        // SL-array boundary which is also 2^6 or 64 buckets (since gap between
        // the next lowest SL index is only 32, 32-64).
        indices.flIndex = rawFL > FL_OFFSET ? (rawFL - FL_OFFSET) : 0;
        // sl := (r right_shift (fl-L)) - 2^L
        indices.slIndex = (blockSize >> (rawFL - L)) - (1 << L);
        return indices;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr typename TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::BitmapIndices TLSF<
        MemStrategy, TelPolicy, Safe, MTPolicy>::mappingSearch(size_t blockSize) noexcept
    {
        // When allocating a block, we typically round to the next nearest bucket,
        // To make it work we need to add an offset of 2^(log_2(r) - L) - 1 which
        // cause the division in mapping insert to rounded fl/sl to the next block.
        // SL: [1024, 1040), [1040, 1056)...
        // Since the blockSize is not perfectly aligned to the bounds, we will expect it to be
        // rounded to 1040 or have an sl index of 1. So we increment the block size by the sl block width - 1
        // blockSize + (2^(10-6)) - 1 = blockSize + 15 (which will make the values round to [1040,1056) sl-index
        // if the value is not 1024(1025->1040, 1026->1041...)
        // which when divided by block width 1041/16 yields 65 which when subtracted from our bucket size(2^6 or 64)
        // gets us 1, which is the [1040, 1056) sl bucket.
        blockSize = blockSize + (1ULL << (utils::fls(blockSize) - L)) - 1;
        // Since after rounding its pretty much the same as mapping insert.
        return mappingInsert(blockSize);
    }

} // namespace pmm
