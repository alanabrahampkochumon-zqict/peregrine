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

#include <cstring>
#include <format>

namespace pmm
{
    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::TLSF(
        uint8_t* buffer, const size_t memorySize) noexcept
        requires(MemoryPolicy == MemPolicy::External)
        : _buffer{ buffer }, _size{ memorySize }, _usedSize{ 0 }, _flBitmap{ 0 }, _slBitmap{}
    { insertBlock(_buffer, _size); }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::TLSF(
        const size_t allocatorSize) noexcept
        requires(MemoryPolicy == MemPolicy::Internal)
        : _buffer{ static_cast<uint8_t*>(memAlloc(allocatorSize)) },
          _size{ allocatorSize },
          _usedSize{ 0 },
          _flBitmap{ 0 },
          _slBitmap{},
          _freeList{}
    { insertBlock(_buffer, _size); }

    // TODO: Update move ctor to move bitmaps
    // TODO: Update Strategy, and Policy to enum
    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::TLSF(TLSF&& tlsf) noexcept
        : _buffer{ std::exchange(tlsf._buffer, nullptr) },
          _size{ tlsf._size },
          _usedSize{ tlsf._usedSize },
          _flBitmap{ tlsf._flBitmap }
    { std::move(tlsf._slBitmap.begin(), tlsf._slBitmap.end(), _slBitmap.begin()); }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>& TLSF<
        MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::operator=(TLSF&& tlsf) noexcept
    {
        // If the memory is internally managed, we need to free it before moving over the other allocator's
        // internal buffer.
        if constexpr (MemoryPolicy == MemPolicy::Internal)
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



    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::~TLSF() noexcept
        requires(MemoryPolicy == MemPolicy::Internal)
    { memFree(_buffer, _size); }


    /**************************************
     *             GETTERS                *
     **************************************/

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr size_t TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::size()
        const noexcept
    { return _size; }

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr size_t TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::usedSize()
        const noexcept
    { return _usedSize; }

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr size_t TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::freeSize()
        const noexcept
    { return _size - _usedSize; }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr void* TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::malloc(
        const size_t size, const size_t alignment) noexcept
    {
        PMM_ASSERT_MSG(std::has_single_bit(alignment), "Alignment must be a power of 2");
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
        // Remove and insert the appropriate header and add calculate padding for the block.
        // We need to calculate padding based on the address that is offset by the size of header and header offset.
        // Calculation representation: [Header][HeaderOffset][Padding][Aligned Memory Ptr]
        // But padding will be placed in the middle in real usage.
        // [Header][Offset][Addr Start]
        // |
        // basePtr(points to the start of header)
        auto basePtr            = reinterpret_cast<uint8_t*>(freeBlockHeader);
        const auto misalignment = (reinterpret_cast<uintptr_t>(basePtr) + metadataSize) & (alignment - 1);
        const auto padding      = alignment - misalignment;
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
            /// The remaining block starts where the allocated block ends
            auto remainingBlockStart = basePtr + usedSize;
            insertBlock(remainingBlockStart, sizeLeft);
        }
        // TODO: If the padding can store the header, then do so
        //                Offset
        //                  ↓
        // [Header][Padding][OffsetAmount][Address Returned to user]
        // <------- OFFSET AMOUNT ------->
        // Add the offset after adding the padding
        const auto offsetAmount = metadataSize + padding;
        const auto offset       = reinterpret_cast<HeaderOffset_t*>(basePtr + sizeof(Header) + padding);
        *offset                 = offsetAmount;
        // We only need to offset the memory by the size of header offset and padding as header is prepopulated before
        // the "freeBlock".
        const auto memoryStart = basePtr + metadataSize + padding;
        return memoryStart;
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr void TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::mfree(
        void* block) noexcept
    {
        // prev_offset := sizeof(block) - padding(block) - sizeof(size_t)
        // [[Header][...][size_t]] + [[Header][padding][HeaderOffset][block....]] =COALESCED=> [[Header][....]]
        // ^             ^            ^                              ^
        // |             |            |                              |
        // start       offset   block - headerOffset               block
        // <--- PREV BLK SIZE --->   <------------ CURRENT BLK SIZE ------------>
        // We need to offset the block to the header's base which is the real real starting address of the block.
        // since all mergePrevious, mergeNext, and mappingInsert expects the block to start at Header
        const HeaderOffset_t* headerOffset =
            reinterpret_cast<HeaderOffset_t*>(static_cast<uint8_t*>(block) - sizeof(HeaderOffset_t));
        block       = static_cast<uint8_t*>(block) - *headerOffset;
        block       = mergePrevious(static_cast<uint8_t*>(block));
        block       = mergeNext(static_cast<uint8_t*>(block));
        auto header = static_cast<Header*>(block);
        // const auto index = mappingInsert(header->getSize()); // TODO: REMOVE

        // TODO: Insert block needs to be made more granular since we are doing repeated work.
        insertBlock(static_cast<uint8_t*>(block), header->getSize());

        // Mark the next block's prevFree.
        // If this is not the final block, then we can mark the next block's prevFreeBlock as true.
        // The currently freed block is considered non-final if and only if the current block with its size leaves
        // enough space for at least a header.
        // TODO: Check if its much safe to use the SPLIT_SIZE_THRESHOLD as the invalidation boundary.
        if (reinterpret_cast<uintptr_t>(block) + header->getSize() + sizeof(Header) <
            reinterpret_cast<uintptr_t>(_buffer) + _size)
        {
            // [CurrentHeader][Block Space...][Footer] [NextBlockHeader]
            const auto nextBlockAddress = static_cast<uint8_t*>(block) + header->getSize();
            // Mark the next block header's isPrevFree flag
            const auto nextHeader = reinterpret_cast<Header*>(nextBlockAddress);
            nextHeader->markPrevFree();
            // Write the size current block to the footer.
            const auto footer = reinterpret_cast<size_t*>(nextBlockAddress - sizeof(size_t));
            *footer           = header->getSize();
        }
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr void* TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::resize(
        void* block, const size_t oldSize, const size_t newSize) noexcept
    {
        const auto startAddress = static_cast<uint8_t*>(block);
        // If the old header and new header have equal sizes or if we are trying to resize to a smaller size
        // smaller than the split threshold, then we can just return the block.
        // We don't have verbosely check for equality since the conditional will cover the equal case for us.
        if (newSize <= oldSize && oldSize - newSize < SPLIT_SIZE_THRESHOLD)
        {
            return block;
        }
        else if (newSize < oldSize)
        {
            // If the new block is smaller we can update the header and store the cleaved block
            // Update the header
            const auto oldOffset = reinterpret_cast<HeaderOffset_t*>(startAddress - sizeof(HeaderOffset_t));
            const auto oldHeader = reinterpret_cast<Header*>(startAddress - *oldOffset);
            oldHeader->setSize(newSize);
            // Insert the block
            insertBlock(startAddress + newSize);
            // Return
            return block;
        }
        else
        {
            // If a larger memory is requested, get a new memory block
            // copy the existing content, free the old memory and return the new memory
            const auto newMemory = malloc(newSize);
            std::memcpy(newMemory, block, oldSize);
            mfree(block);
            return newMemory;
        }
    }



    /**************************************
     *         INTERNAL HELPERS           *
     **************************************/

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr typename TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::BitmapIndices
    TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::mappingInsert(size_t blockSize) noexcept
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


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr typename TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::BitmapIndices
    TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::mappingSearch(size_t blockSize) noexcept
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
        // Round block size to be at least 2^L to prevent shifting math errors.
        blockSize = std::max(1ULL << L, blockSize);
        blockSize = blockSize + (1ULL << (utils::fls(blockSize) - L)) - 1;
        // Since after rounding its pretty much the same as mapping insert.
        return mappingInsert(blockSize);
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr typename TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::TLSFFreeNode*
    TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::searchSuitableBlock(
        BitmapIndices& index) const noexcept
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
                index.flIndex, index.slIndex, nonEmptyFL, nonEmptySL)
                .c_str());

        index.flIndex = nonEmptyFL;
        index.slIndex = nonEmptySL;
        return _freeList[nonEmptyFL][nonEmptySL];
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr void TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::insertBlock(
        uint8_t* block, const size_t blockSize) noexcept
    {
        /// Since the function is internal this check is unnecessary but kept for safety.
        PMM_ASSERT_MSG(block != nullptr && blockSize > 0, "Cannot insert a zero sized or nullptr block.");

        // Get the FL and SL index
        auto index = mappingInsert(blockSize);

        // Insert the freenode
        //                       ____________________________
        //                      |                            |
        // FreeList = [[Header][FreeNode][....] <=> [Header][FreeNode][....]]
        // [FL][SL] = nullptr(START) <- existingNode* -> nullptr(END)
        //            nullptr(START) <- freeNode* <=> existingNode* -> nullptr(END)
        TLSFFreeNode* freeNode     = reinterpret_cast<TLSFFreeNode*>(block + sizeof(Header));
        TLSFFreeNode* existingNode = _freeList[index.flIndex][index.slIndex];
        freeNode->prev             = nullptr; // Can have garbage values so we need to assign nullptr.
        freeNode->next             = existingNode;
        if (existingNode != nullptr)
        {
            existingNode->prev = freeNode;
        }
        _freeList[index.flIndex][index.slIndex] = freeNode;

        // Update the FL and SL bitmasks
        _flBitmap |= 1ULL << index.flIndex;
        _slBitmap[index.flIndex] |= 1ULL << index.slIndex;

        // Update the Header of the free node
        Header* header = getHeader(freeNode);
        header->setSize(blockSize);
        header->markFree();
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr typename TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::Header* TLSF<
        MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::getHeader(TLSFFreeNode* node) noexcept
    {
        // Note: Header is placed below the free node.
        return reinterpret_cast<Header*>(reinterpret_cast<uint8_t*>(node) - sizeof(TLSFFreeNode));
    }



    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    constexpr uint8_t* TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::mergePrevious(
        uint8_t* block) noexcept
    {

        const Header* header     = reinterpret_cast<Header*>(block);
        size_t currentHeaderSize = header->getSize(); // Get size gives the entire block size.
        if (header->isPrevFree())
        {
            // Get the previous block's size from it's footer.
            const size_t* prevBlockSize = reinterpret_cast<size_t*>(block - sizeof(size_t));

            // Get the start of previous header.
            // The previous block ends at this blocks header so subtracting that from previous block's size
            // gives the starting address of
            uint8_t* prevStartAddress = block - *prevBlockSize;

            // Unlink next node
            // [Header][FreeNode]
            TLSFFreeNode* prevFreeNode = reinterpret_cast<TLSFFreeNode*>(prevStartAddress + sizeof(Header));
            unlinkNode(prevFreeNode);

            // Write the new header
            Header* newHeader = reinterpret_cast<Header*>(prevStartAddress);
            // The new header is the previous free block's header so we can query and story its size
            const auto prevHeaderSize = newHeader->getSize();
            newHeader->setSize(prevHeaderSize + currentHeaderSize);
            newHeader->markFree();

            // return the start address
            return prevStartAddress;
        }
        else
        {
            return block;
        }
    }

    // TODO: Add tests with looped allocations
    //       1. Descending free
    //       1. Ascending free
    //       1. Free odd allocations and then even allocations
    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr uint8_t* TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::mergeNext(
        uint8_t* block) noexcept
    {
        // To merge with the next block we need to check if the next block is free
        // Access the next block's header
        Header* currentHeader = reinterpret_cast<Header*>(block);
        // Bounds checking to ensure that we are not trying to access invalid memory in beyond
        // our application's addressable space.
        // We can't also the the boundary byte.
        Header* nextHeader = block + currentHeader->getSize() >= _buffer + _size
            ? nullptr
            : reinterpret_cast<Header*>(block + currentHeader->getSize());
        // Coalesce current and next blocks if next block is free.
        if (nextHeader != nullptr && nextHeader->isFree())
        {
            // Unlink next node
            // [Header][FreeNode]
            TLSFFreeNode* nextFreeNode =
                reinterpret_cast<TLSFFreeNode*>(reinterpret_cast<uint8_t*>(nextHeader) + sizeof(Header));
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


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode, MTPolicy MultithreadingPolicy>
    constexpr void TLSF<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::unlinkNode(
        TLSFFreeNode* block) noexcept
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
            _freeList[index.flIndex][index.slIndex] = block->next;
        }

        // Reset the sl flag if sl bucket is empty
        const auto newSL = _slBitmap[index.flIndex] & ~(1ULL << index.slIndex);
        // Ternary will get translated into a single cmovcc instruction so no branching.
        _slBitmap[index.flIndex] =
            _freeList[index.flIndex][index.slIndex] == nullptr ? newSL : _slBitmap[index.flIndex];
        const auto newFL = _flBitmap & ~(1ULL << index.flIndex);
        _flBitmap        = _slBitmap[index.flIndex] == 0 ? newFL : _flBitmap;
    }


} // namespace pmm
