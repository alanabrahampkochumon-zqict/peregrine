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

#include <format>

namespace pmm
{
    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(uint8_t* buffer,
                                                                            const size_t memorySize) noexcept
        requires std::same_as<MemStrategy, UnmanagedMemory>
        : _buffer{ buffer }, _size{ memorySize }, _usedSize{ 0 }, _flBitmap{ 0 }, _slBitmap{}
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(const size_t allocatorSize) noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
        : _buffer{ static_cast<uint8_t*>(memAlloc(allocatorSize)) },
          _size{ allocatorSize },
          _usedSize{ 0 },
          _flBitmap{ 0 },
          _slBitmap{}
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(TLSF&& tlsf) noexcept
        : _buffer{ std::exchange(tlsf._buffer, nullptr) },
          _size{ tlsf._size },
          _usedSize{ tlsf._usedSize },
          _flBitmap{ tlsf._flBitmap }
    { std::move(tlsf._slBitmap.begin(), tlsf._slBitmap.end(), _slBitmap.begin()); }


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

        _buffer   = std::exchange(tlsf._buffer, nullptr);
        _size     = tlsf._size;
        _usedSize = tlsf._usedSize;
        _flBitmap = tlsf._flBitmap;
        std::move(tlsf._slBitmap.begin(), tlsf._slBitmap.end(), _slBitmap.begin());

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


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr void* TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::malloc(const size_t size,
                                                                                    const size_t alignment) noexcept
    {
        // We need to allocate space for the header as well as assume the worst case alignment of alignment - 1
        const auto requiredSize = sizeof(Header) + alignment + size;

        // For allocating memory we need to find the fl and sl indices.
        auto index     = mappingSearch(requiredSize);
        auto freeBlock = searchSuitableBlock(index);

        // searchSuitable block doesn't return a nullptr by default on in safe mode
        // so we can skip the nullptr check and assume a valid block is returned. TODO(SAFEMODE)


        // Clear the SL bitmask
        if (freeBlock->next == nullptr)
        {
            _slBitmap[index.flIndex] &= ~(1ULL << index.slIndex);
        }
        // Clear the FL Bitmask if the sl bitmask is zero.
        _flBitmap = _slBitmap[index.flIndex] == 0 ? _flBitmap & ~(1ULL << index.flIndex) : _flBitmap;


        Header* freeBlockHeader = getHeader(freeBlock);
        const auto freeSize     = freeBlockHeader->getSize();
        // Remove and insert the appropriate header for and add calculate padding for the block.
        const auto padding = static_cast<uintptr_t>(freeBlock) & (alignment - 1);
        // [padding][header][*ptr returned to user]
        Header* header = static_cast<Header*>(freeBlock);
        header->markUsed();
        header->setSize(requiredSize);
        header->padding = padding;

        // Since we know real padding right now we can use it.
        const auto usedSize = size + sizeof(Header) + padding;

        // If there is more free space than split threshold, split and store that memory
        if (const auto sizeLeft = freeSize - usedSize; sizeLeft >= SPLIT_SIZE_THRESHOLD)
        {
            auto remainingBlock = static_cast<uint8_t*>(freeBlock) + usedSize;
            insertBlock(remainingBlock, sizeLeft);
        }

        const auto memoryStart = static_cast<uint8_t*>(freeBlock) + padding + sizeof(Header);

        return memoryStart;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr void TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::free(void* block) noexcept
    {
        // block       = mergePrevious(block);
        // block       = mergeNext(block);
        // auto header = static_cast<Header*>(block);
        //
        // // TODO: Mark the next block's prevFree.
        // auto nextHeader = static_cast<Header*>(block + header->getSize());
    }



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
        blockSize = std::max(SPLIT_SIZE_THRESHOLD, blockSize);
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


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr typename TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSFFreeNode* TLSF<
        MemStrategy, TelPolicy, Safe, MTPolicy>::searchSuitableBlock(BitmapIndices& index) const noexcept
    {
        // For finding the suitable block we first get the SL-bitmask for the given fl index and
        // mask out all the values that are less than our sl index.
        // Eg: _slBitmask[indices.flIndex] = 0010 1100, SL = 1, then
        //     bitTemp = 0010 1100 & 1111 1111 << 1
        //             = 0010 1100 & 1111 1110
        //             = 0010 1100
        // Then, we use ffs(bitTemp) to get the non-empty SL, ie 2 in our case.
        // But if our SL index returns a zero, we mask out of flBitmap and get the ffs of that.
        // Eg: _flIndex = 0010 1100, and our FL = 1, then bitmapTemp will return 0
        // so we take 0010 1100 & 1111 1111 << 2(FL + 1, since our slBitmap at FL is empty)
        //          = 0010 1100 & 1111 1100
        //          = 0010 1100
        //    newFL = ffs(00101100) which returns 2.
        // and taking the ffs(_slBitmap[newFL]), gives the correct sl index since block are arranged in order.
        // TODO: Update to branchless variant(exploiting ?: which compiles down to cmovcc)
        //       after adding allocation and tests
        auto bitmapTemp = _slBitmap[index.flIndex] & (~0ULL << index.slIndex);
        size_t nonEmptyFL, nonEmptySL;
        if (bitmapTemp == 0)
        {
            nonEmptyFL = index.flIndex;
            nonEmptySL = utils::ffs(bitmapTemp);
        }
        else
        {
            bitmapTemp = index.flIndex & (~0ULL << (index.flIndex + 1));
            nonEmptyFL = utils::ffs(bitmapTemp);
            nonEmptySL = utils::ffs(bitmapTemp);
        }
        // Out of range FL and SL index indicate that there is no memory left with
        // that satisfies the FL and SL requirements.
        PMM_ASSERT_MSG(
            nonEmptyFL < FL_SIZE && nonEmptySL < SL_SIZE,
            std::format(
                "FL and/or SL indices out-of-range.\nProvided\n\tFL: {} and SL: {}.\nDeduced\n\tFL: {} and SL: {}.\n",
                index.flIndex, index.slIndex, nonEmptyFL, nonEmptySL));

        index.flIndex = nonEmptyFL;
        index.slIndex = nonEmptySL;
        return freeList[nonEmptyFL][nonEmptySL];
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr void TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::insertBlock(uint8_t* block,
                                                                                        const size_t blockSize) noexcept
    {
        /// Since the function is internal this check is necessary but kept for safety.
        PMM_ASSERT_MSG(block != nullptr && blockSize > 0, "Cannot insert a zero sized or nullptr block.");

        // Get the FL and SL index
        auto index = mappingInsert(blockSize);

        // Insert the freenode
        // FreeList = [[FreeNode][Header][....] <=> [FreeNode][Header][....]]
        // [FL][SL] = nullptr(START) <- existingNode* -> nullptr(END)
        //            nullptr(START) <- freeNode* <=> existingNode* -> nullptr(END)
        TLSFFreeNode* freeNode     = static_cast<TLSFFreeNode*>(block);
        TLSFFreeNode* existingNode = freeList[index.flIndex][index.slIndex];
        freeNode->next             = existingNode;
        if (existingNode != nullptr)
        {
            existingNode->prev = freeNode;
        }
        freeList[index.flIndex][index.slIndex] = freeNode;

        // Update the Header of the free node
        Header* header = getHeader(freeNode);
        header->setSize(blockSize);
        header->markFree();
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr typename TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::Header* TLSF<
        MemStrategy, TelPolicy, Safe, MTPolicy>::getHeader(TLSFFreeNode* node) noexcept
    {
        // Note: Header is placed above the free node.
        return static_cast<Header*>(static_cast<uint8_t*>(node) + sizeof(TLSFFreeNode));
    }

} // namespace pmm
