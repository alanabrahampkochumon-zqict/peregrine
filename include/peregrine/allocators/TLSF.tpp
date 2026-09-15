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
    PMM_INLINE constexpr void* TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::alloc(const size_t size,
                                                                                   const size_t alignment) noexcept
    {
        // [Header][Padding][OffsetToHeader][Ptr* returned to user]
        // The offset to header acts as a way for bidirectional access to header. From the base address,
        // as well as from the pointer handed over to the user.

        const auto metadataSize = sizeof(Header) + sizeof(HeaderOffset_t);
        // We need to allocate space for the metadata, size and for the worst case alignment of alignment - 1
        const auto requiredSize = metadataSize + (alignment - 1) + size;

        // For allocating memory we need to find the FL and SL indices.
        auto index     = mappingSearch(requiredSize);
        auto freeBlock = searchSuitableBlock(index);

        // searchSuitable block doesn't return a nullptr by default on in safe mode
        // so we can skip the nullptr check and assume a valid block is returned. TODO(SAFEMODE)

        // Unlink the free node from the bitmap
        unlinkNode(freeBlock);

        // TODO: REMOVE
        // Clear the SL bitmask. This is only applicable if the block is the only node is freelist.
        // if (freeBlock->next == nullptr)
        // {
        //     // Let SLIndex be 2, then shifting gives use 0100 and mask is 1011.
        //     _slBitmap[index.flIndex] &= ~(1ULL << index.slIndex);
        // }
        // // Clear the FL Bitmask if the sl bitmask is zero.
        // _flBitmap = _slBitmap[index.flIndex] == 0 ? _flBitmap & ~(1ULL << index.flIndex) : _flBitmap;


        // For rewriting the header we need to get the free block's size.
        // Write the header with the used size and padding.
        // NOTE: Used size is the entire size of the block including padding and metadata.
        Header* freeBlockHeader = getHeader(freeBlock);
        const auto freeSize     = freeBlockHeader->getSize();
        // Remove and insert the appropriate header for and add calculate padding for the block.
        // We need to calculate padding based on the address that is offset by the size of header and header offset.
        // Calculation representation: [Header][HeaderOffset][Padding][Aligned Memory Ptr]
        // But padding will be placed in the middle in real usage.
        auto basePtr       = static_cast<uint8_t*>(freeBlockHeader);
        const auto padding = (reinterpret_cast<uintptr_t>(basePtr) + metadataSize) & (alignment - 1);
        // [Header][Padding][HeaderOffset][Aligned Memory Ptr(Returned to user)]
        freeBlockHeader->markUsed();
        freeBlockHeader->padding = padding;

        // Since we know real padding right now we can use it.
        auto usedSize       = size + metadataSize + padding;
        const auto sizeLeft = freeSize - usedSize;
        // If we don't carve out free block if it is smaller than threshold size, we need ot add it's size to used size.
        usedSize += sizeLeft < SPLIT_SIZE_THRESHOLD ? sizeLeft : 0;
        freeBlockHeader->setSize(usedSize);

        // If there is more free space than split threshold, split and store that memory
        if (sizeLeft >= SPLIT_SIZE_THRESHOLD)
        {
            auto remainingBlock = static_cast<uint8_t*>(freeBlock) + usedSize;
            insertBlock(remainingBlock, sizeLeft);
        }

        // Add the offset after adding the padding
        const auto offsetAmount = sizeof(Header) + padding;
        HeaderOffset_t* offset  = static_cast<uint8_t*>(freeBlock) + offsetAmount;
        *offset                 = offsetAmount;

        const auto memoryStart = static_cast<uint8_t*>(freeBlock) + metadataSize + padding;
        return memoryStart;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr void TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::free(void* block) noexcept
    {
        block            = mergePrevious(block);
        block            = mergeNext(block);
        auto header      = static_cast<Header*>(block);
        const auto index = mappingInsert(header->getSize());

        // TODO: Insert block needs to be made more granular since we are doing repeated work.
        insertBlock(block, header->getSize());

        // Mark the next block's prevFree.
        // If this is not the final block, then we can mark the next block's prevFreeBlock as true.
        if (reinterpret_cast<uintptr_t>(block) + header->getSize() <
            reinterpret_cast<uintptr_t>(_buffer) + _size + sizeof(Header))
        {
            auto nextHeader = static_cast<Header*>(block + header->getSize());
            nextHeader->markPrevFree();
        }
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
        if (bitmapTemp != 0)
        {
            nonEmptyFL = index.flIndex;
            nonEmptySL = utils::ffs(bitmapTemp);
        }
        else
        {
            bitmapTemp = _flBitmap & (~0ULL << (index.flIndex + 1));
            PMM_ASSERT_MSG(bitmapTemp > 0, "Out of memory: No suitable FL bucket.");
            nonEmptyFL = utils::ffs(bitmapTemp);
            nonEmptySL = utils::ffs(_slBitmap[nonEmptyFL]);
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
        //                       ____________________________
        //                      |                            |
        // FreeList = [[Header][FreeNode][....] <=> [Header][FreeNode][....]]
        // [FL][SL] = nullptr(START) <- existingNode* -> nullptr(END)
        //            nullptr(START) <- freeNode* <=> existingNode* -> nullptr(END)
        TLSFFreeNode* freeNode     = static_cast<TLSFFreeNode*>(block + sizeof(Header));
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
        // Note: Header is placed below the free node.
        return static_cast<Header*>(static_cast<uint8_t*>(node) - sizeof(TLSFFreeNode));
    }



    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    constexpr uint8_t* TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::mergePrevious(uint8_t* block) noexcept
    {
        // prev_offset := sizeof(block) - padding(block) - sizeof(size_t)
        // [[Header][...][size_t]] + [[Header][padding][HeaderOffset][block....]] =COALESCED=> [[Header][....]]
        // ^             ^            ^                              ^
        // |             |            |                              |
        // start       offset   block - headerOffset               block
        // <--- PREV BLK SIZE --->   <------------ CURRENT BLK SIZE ------------>
        const HeaderOffset_t* headerOffset = static_cast<HeaderOffset_t*>(block - sizeof(HeaderOffset_t));
        const Header* header               = static_cast<Header*>(block - *headerOffset);
        size_t totalSize                   = header->getSize(); // Get size gives the entire block size.
        if (header->isPrevFree())
        {
            // Get the previous block's size from it's footer.
            const size_t* prevBlockSize = static_cast<size_t*>(block - (*headerOffset + sizeof(size_t)));

            // Get the start of previous header.
            // The previous block ends at this blocks header so subtracting that from previous block's size
            // gives the starting address of
            uint8_t* startAddress = static_cast<uint8_t*>(header) - *prevBlockSize;

            // Unlink next node
            // [Header][FreeNode]
            TLSFFreeNode* nextFreeNode = static_cast<TLSFFreeNode*>(startAddress + sizeof(Header));
            unlinkNode(nextFreeNode);

            // Write the new header
            Header* newHeader = static_cast<Header*>(startAddress);
            newHeader->setSize(totalSize);
            newHeader->markFree();

            // Write the footer with block size
            const auto footer = static_cast<size_t*>(block + totalSize - sizeof(size_t));
            *footer           = totalSize;

            // return the start address
            return startAddress;
        }
        else
        {
            return block;
        }
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr uint8_t* TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::mergeNext(uint8_t* block) noexcept
    {
        // To merge with the next block we need to check if the next block is free
        // Access the next block's header
        Header* currentHeader = static_cast<Header*>(block);
        Header* nextHeader    = static_cast<Header*>(static_cast<uint8_t*>(block) + currentHeader->getSize());
        // Coalesce current and next blocks if next block is free.
        if (nextHeader->isFree())
        {
            // Unlink next node
            // [Header][FreeNode]
            TLSFFreeNode* nextFreeNode = static_cast<TLSFFreeNode*>(static_cast<uint8_t*>(nextHeader) + sizeof(Header));
            unlinkNode(nextFreeNode);

            // Unify the header
            // Since our current header is where our new header will be we can overwrite the values
            // to include the new size as well, and reset the padding to zero
            currentHeader->setSize(currentHeader->getSize() + nextHeader->getSize());
            currentHeader->markFree();
            currentHeader->padding = 0;
        }

        return block;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    constexpr void TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::unlinkNode(TLSFFreeNode* block) noexcept
    {
        const auto header = getHeader(block);
        const auto index  = mappingInsert(header->getSize());
        // If the node has a previous link, update it's next node to the current block next node
        // essentially removing this block.
        if (block->prev != nullptr)
        {
            block->prev->next = block->next;
        }
        else
        // If there is no previous node then this could mean either the current block is the first node
        // or its the only node so, reassign freelist head to the block next node.
        {
            freeList[index.flIndex][index.slIndex] = block->next;
        }

        // Reset the sl flag if sl bucket is empty
        const auto newSL = _slBitmap[index.flIndex] & ~(1ULL << index.slIndex);
        // Ternary will get translated into a single cmovcc instruction so no branching.
        _slBitmap[index.flIndex] = freeList[index.flIndex][index.slIndex] == nullptr ? newSL : _slBitmap[index.flIndex];
        const auto newFL         = _flBitmap & ~(1ULL << index.flIndex);
        _flBitmap                = _slBitmap[index.flIndex] == 0 ? newFL : _flBitmap;
    }


} // namespace pmm
