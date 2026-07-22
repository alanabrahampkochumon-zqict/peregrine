/**
 * @file StrictStackTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Verify @ref pmm::Stack<pmm::stack::Strict> allocation, deallocation, and other related memory management
 * logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"
#include "Utils.h"

#include <array>
#include <gtest/gtest.h>
#include <peregrine/allocators/Stack.h>
#include <peregrine/utils/Constants.h>


/**************************************
 *                                    *
 *             TEST SETUP             *
 *                                    *
 **************************************/

using namespace pmm::constants;
constexpr static auto STACK_SIZE = 5_KB;
class StrictStackTests: public ::testing::Test
{

public:
    std::size_t stackSize{ STACK_SIZE };
    pmm::Stack<pmm::stack::Strict> stack{ stackSize };
};


class StrictStackAllocationAlignment: public ::testing::TestWithParam<std::size_t>
{};
INSTANTIATE_TEST_SUITE_P(StackAlignmentTests, StrictStackAllocationAlignment,
                         ::testing::Values(4, 8, 16, 32, 64, 512, 4096));

#ifndef NDEBUG
class StrictStackAllocationAlignmentNonBinaryPowers: public ::testing::TestWithParam<std::size_t>
{};
INSTANTIATE_TEST_SUITE_P(NonPowersOfTwo, StrictStackAllocationAlignmentNonBinaryPowers,
                         ::testing::Values(0, 1, 3, 5, 111));
#endif



struct StrictStackResizeLastParams
{
    std::size_t oldSize, newSize;
};
class StrictStackResizeLast: public ::testing::TestWithParam<StrictStackResizeLastParams>
{};
INSTANTIATE_TEST_SUITE_P(StackResize, StrictStackResizeLast,
                         ::testing::Values(StrictStackResizeLastParams{ .oldSize = 256, .newSize = 128 },
                                           StrictStackResizeLastParams{ .oldSize = 128, .newSize = 512 },
                                           StrictStackResizeLastParams{ .oldSize = 2048, .newSize = 4096 },
                                           StrictStackResizeLastParams{ .oldSize = 4096, .newSize = 2048 }));


/**
 * @addtogroup T_PMM_Strict_Stack
 * @{
 */


/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/**
 * @brief Verify that size() returns the total size of the stack.
 */
TEST_F(StrictStackTests, Size_ReturnsTheSizeOfTheStack) { EXPECT_EQ(stackSize, stack.size()); }


/**
 * @brief Verify that freeSize() returns the total size of the stack, when no allocation is made.
 */
TEST_F(StrictStackTests, FreeSize_NoAllocations_ReturnsTheSizeOfTheStack) { EXPECT_EQ(stackSize, stack.freeSize()); }


/**
 * @brief Verify that freeSize() returns the size - (allocated size + padding),
 *        after an allocation is made.
 */
TEST_F(StrictStackTests, FreeSize_ReturnsStackSizeMinusPaddingAndAllocationSize)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    const auto header       = reinterpret_cast<pmm::StrictStackHeader*>(memory - sizeof(pmm::StrictStackHeader));
    const auto expectedSize = stackSize - (allocatedSize + header->padding);

    EXPECT_EQ(expectedSize, stack.freeSize());
}


/**
 * @brief Verify that freeSize() returns stack size, after an allocation is freed.
 */
// TEST_F(StrictStackTests, FreeSize_AfterFreeingAllocation_ReturnsStackSize)
// {
//     constexpr std::size_t allocatedSize = 512;
//     const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
//     stack.freeBytes(memory);
//     EXPECT_EQ(stackSize, stack.freeSize());
// }


/**
 * @brief Verify that freeSize() returns stack size, after all allocations are freed.
 */
TEST_F(StrictStackTests, FreeSize_AfterFreeingAllAllocations_ReturnsStackSize)
{
    constexpr std::size_t allocatedSize = 512;
    static_cast<void>(stack.allocBytes(allocatedSize));
    stack.freeAll();
    EXPECT_EQ(stackSize, stack.freeSize());
}


/**
 * @brief Verify that usedSize() returns 0, when no allocation is made.
 */
TEST_F(StrictStackTests, UsedSize_NoAllocations_ReturnsZero) { EXPECT_EQ(0, stack.usedSize()); }


/**
 * @brief Verify that usedSize() returns (allocated size + padding), after an allocation is made.
 */
TEST_F(StrictStackTests, UsedSize_ReturnsStackSizeMinusPaddingAndAllocationSize)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    const auto header       = reinterpret_cast<pmm::StrictStackHeader*>(memory - sizeof(pmm::StrictStackHeader));
    const auto expectedSize = allocatedSize + header->padding;

    EXPECT_EQ(expectedSize, stack.usedSize());
}


/**
 * @brief Verify that usedSize() returns zero, after an allocation is freed.
 */
TEST_F(StrictStackTests, UsedSize_AfterFreeingAllocation_ReturnsZero)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    stack.freeBytes(memory);
    EXPECT_EQ(0, stack.usedSize());
}


/**
 * @brief Verify that usedSize() returns zero, after all allocations are freed.
 */
TEST_F(StrictStackTests, UsedSize_FreeAllAllocation_ReturnsStackSize)
{
    constexpr std::size_t allocatedSize = 512;
    static_cast<void>(stack.allocBytes(allocatedSize));
    stack.freeAll();
    EXPECT_EQ(0, stack.usedSize());
}


/**************************************
 *                                    *
 *            ALLOC BYTES             *
 *                                    *
 **************************************/

/** @brief Verify that stack allocation using allocBytes returns a valid pointer, given an empty stack. */
TEST_F(StrictStackTests, AllocBytes_ReturnsNonNullPtrOnEmptyStack)
{
    const auto dataPtr = stack.allocBytes(120);
    EXPECT_NE(nullptr, dataPtr);
}


/**
 * @brief Verify that stack allocation using allocBytes returns a valid pointer,
 *        given a non-empty stack with memory to spare.
 */
TEST_F(StrictStackTests, AllocBytes_ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.allocBytes(500);
    const auto dataPtr2 = stack.allocBytes(500);

    EXPECT_NE(nullptr, dataPtr1);
    EXPECT_NE(nullptr, dataPtr2);
    EXPECT_NE(dataPtr1, dataPtr2);
}



/** @brief Verify that memory allocated using allocBytes maintains data integrity. */
TEST_F(StrictStackTests, AllocBytes_RepeatedAllocationAndWritesDoNotCorruptData)
{
    constexpr std::size_t array1Len = 50;
    constexpr std::size_t array2Len = 25;
    const auto array1               = static_cast<int*>(stack.allocBytes(array1Len * sizeof(int)));

    // Write into first array
    for (std::size_t i = 0; i < array1Len; ++i)
    {
        array1[i] = static_cast<int>(i + 212);
    }

    // Write into second array
    const auto array2 = static_cast<int*>(stack.allocBytes(array2Len * sizeof(int)));
    for (std::size_t i = 0; i < array2Len; ++i)
    {
        array2[i] = static_cast<int>(i + 515);
    }


    // Verify no overlap / data corruption has occurred
    for (std::size_t i = 0; i < array1Len; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 212), array1[i]);
    }

    for (std::size_t i = 0; i < array2Len; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 515), array2[i]);
    }
}


/** @brief Verify that allocation using allocBytes stores header before returned address. */
TEST_F(StrictStackTests, AllocBytes_HeaderIsStoredBehindReturnedAddress)
{
    constexpr auto alignment = 8;
    const auto memoryStart   = static_cast<char*>(stack.allocBytes(500, alignment));

    const auto header = reinterpret_cast<pmm::StrictStackHeader*>(reinterpret_cast<char*>(memoryStart) -
                                                                  sizeof(pmm::StrictStackHeader));
    EXPECT_GE(header->padding, alignment);
}


/**
 * @brief Verify that allocation using allocBytes always return an address aligned
 *        to the specified boundary.
 */
TEST_P(StrictStackAllocationAlignment, AllocBytes_AlwaysReturnAnAlignedMemoryAddress)
{
    const auto alignment = this->GetParam();
    const auto blockSize = 5 * alignment;
    const auto stackSize = 10 * alignment;
    pmm::Stack<pmm::stack::Strict> stack{ stackSize };
    const void* dataAddress = stack.allocBytes(blockSize, alignment);

    // Verify returned address is 0 by using 2^n modulo trick
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(dataAddress) & (alignment - 1));
}



/**************************************
 *                                    *
 *            ALLOC (OBJ)             *
 *                                    *
 **************************************/

/** @brief Verify that stack allocation returns a valid pointer, given an empty stack. */
TEST_F(StrictStackTests, Alloc_ReturnsNonNullPtrOnEmptyStack)
{
    const auto vector = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_NE(nullptr, vector);
    EXPECT_FLOAT_EQ(1.0f, vector->x);
    EXPECT_FLOAT_EQ(2.0f, vector->y);
    EXPECT_FLOAT_EQ(3.0f, vector->z);
    EXPECT_FLOAT_EQ(4.0f, vector->w);
}


/**
 * @brief Verify that stack allocation using alloc returns a valid pointer,
 *        given a non-empty stack with memory to spare.
 */
TEST_F(StrictStackTests, Alloc_ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    const auto dataPtr2 = stack.alloc<Vec4>(5.0f, 1.0f, 31.0f, 3.0f);

    EXPECT_NE(nullptr, dataPtr1);
    EXPECT_NE(nullptr, dataPtr2);
    EXPECT_NE(dataPtr1, dataPtr2);
}



/** @brief Verify that allocated using alloc memory maintains data integrity. */
TEST_F(StrictStackTests, Alloc_RepeatedAllocationAndWritesDoNotCorruptData)
{
    const auto dataPtr1 = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    const auto dataPtr2 = stack.alloc<Vec4>(5.0f, 1.0f, 31.0f, 3.0f);

    EXPECT_FLOAT_EQ(1.0f, dataPtr1->x);
    EXPECT_FLOAT_EQ(2.0f, dataPtr1->y);
    EXPECT_FLOAT_EQ(3.0f, dataPtr1->z);
    EXPECT_FLOAT_EQ(4.0f, dataPtr1->w);

    EXPECT_FLOAT_EQ(5.0f, dataPtr2->x);
    EXPECT_FLOAT_EQ(1.0f, dataPtr2->y);
    EXPECT_FLOAT_EQ(31.0f, dataPtr2->z);
    EXPECT_FLOAT_EQ(3.0f, dataPtr2->w);
}


/** @brief Verify that allocation using alloc stores header before returned address. */
TEST_F(StrictStackTests, Alloc_HeaderIsStoredBehindReturnedAddress)
{
    const auto vector = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    const auto header =
        reinterpret_cast<pmm::StrictStackHeader*>(reinterpret_cast<char*>(vector) - sizeof(pmm::StrictStackHeader));
    EXPECT_GE(header->padding, alignof(Vec4));
}


/** @brief Verify that allocation using alloc always return an address aligned to the alignment of the type. */
TEST_P(StrictStackAllocationAlignment, Alloc_AlwaysReturnAnAlignedMemoryAddress)
{
    pmm::Stack<pmm::stack::Strict> stack{ 512 };
    const auto vector = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    // Verify returned address is 0 by using 2^n module trick
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vector) & (alignof(Vec4) - 1));
}


/** @brief Verify that allocV can allocate a span of primitives. */
TEST_F(StrictStackTests, AllocV_AllocatesWriteablePrimitiveArray)
{
    auto vectors = stack.allocV<int>(10);

    // Write to buffer
    for (std::size_t i = 0; i < vectors.size(); ++i)
    {
        vectors[i] = static_cast<int>(i + 5);
    }


    // Read from buffer
    for (std::size_t i = 0; i < vectors.size(); ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 5), vectors[i]);
    }
}


/** @brief Verify that allocV can allocate a span of user defined types. */
TEST_F(StrictStackTests, AllocV_AllocatesWriteTypeArray)
{
    auto vectors = stack.allocV<Vec4>(10);

    // Write to buffer
    for (std::size_t i = 0; i < vectors.size(); ++i)
    {
        const auto mul = static_cast<float>(i);
        vectors[i]     = { 1.0f * mul, 5.0f * mul, 3.0f * mul, 2.0f * mul };
    }


    // Read from buffer
    for (std::size_t i = 0; i < vectors.size(); ++i)
    {
        const auto mul = static_cast<float>(i);
        EXPECT_FLOAT_EQ(1.0f * mul, vectors[i].x);
        EXPECT_FLOAT_EQ(5.0f * mul, vectors[i].y);
        EXPECT_FLOAT_EQ(3.0f * mul, vectors[i].z);
        EXPECT_FLOAT_EQ(2.0f * mul, vectors[i].w);
    }
}

/** @brief Verify that allocation using allocV aligns to the type's alignment. */
TEST_F(StrictStackTests, AllocV_BaseAddressAlignedToAlignmentOfType)
{
    // Allocate some memory to throw off alignment
    static_cast<void>(stack.allocBytes(2, 2));

    const auto vector = stack.allocV<Vec4>(10);
    // Base address % alignment == 0
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vector.data()) & (alignof(Vec4) - 1));
}


/**************************************
 *                                    *
 *            FREE TESTS              *
 *                                    *
 **************************************/

/** @brief Verify that stack free, frees up memory for subsequent allocations. */
TEST_F(StrictStackTests, FreeBytes_FreesMemoryForSubsequentAllocations)
{
    constexpr std::size_t alignment = 8;
    const auto usableSize = stackSize - 128; // A big offset is used since we need to make room for header + alignment
    const auto freeMemory = static_cast<char*>(stack.allocBytes(usableSize, alignment));
    // Free the memory
    stack.freeBytes(freeMemory);

    // Allocation another buffer
    const auto elementCount  = (stackSize - sizeof(pmm::StrictStackHeader) - alignment - 1) / sizeof(int);
    const auto newAllocation = static_cast<int*>(stack.allocBytes(elementCount * sizeof(int)));

    // Write to new allocation
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        newAllocation[i] = static_cast<int>(i + 3812);
    }

    // Verify the allocation is successful with data writes
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 3812), newAllocation[i]);
    }
}

/** @brief Verify that stack free, when called multiple times, free the allocated buffer. */
TEST_F(StrictStackTests, FreeBytes_MultipleTimesMakesRoomInTheStack)
{
    constexpr std::size_t alignment = 8;
    // Last 128 is the offset used to make room for header(at least 16 bytes) + alignment
    // 4 allocations 7 byte unalignment => 7 + 16 = 23 bytes per allocation
    // 4 * 23 = 92 bytes
    constexpr std::size_t leeway = 128;
    const std::array<std::size_t, 4> allocationSizes{ 128, 256, 1024, stackSize - 128 - 256 - 1024 - leeway };
    std::array<void*, 4> memory;

    // Allocate Memory
    for (std::size_t i = 0; i < 4; ++i)
    {
        memory[i] = stack.allocBytes(allocationSizes[i], alignment);
    }

    // Free the memory
    // Due to rotation of unsigned types, we need to start from sizeof(array) and inside the loop use -1 to
    // properly index into the array
    // Data must be freed in the reverse order of allocation
    for (std::size_t i = 4; i > 0; --i)
    {
        stack.freeBytes(memory[i - 1]);
    }

    // Allocation another buffer with a large enough size that proper allocation will not happen without proper frees
    const auto elementCount  = (stackSize - leeway) / sizeof(int);
    const auto newAllocation = static_cast<int*>(stack.allocBytes(elementCount * sizeof(int)));

    // Write to new allocation
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        newAllocation[i] = static_cast<int>(i + 3812);
    }

    // Verify the allocation is successful with data writes
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 3812), newAllocation[i]);
    }
}


/** @brief Verify that stack freeAll, frees the entire stack. */
TEST_F(StrictStackTests, FreeAll_FreesTheEntireStack)
{
    constexpr std::size_t alignment = 8;
    // Last 128 is the offset used to make room for header + alignment
    const std::array<std::size_t, 4> allocationSizes{ 128, 256, 1024, stackSize - 128 - 256 - 1024 - 128 };

    // Allocate Memory
    for (std::size_t i = 0; i < 4; ++i)
    {
        static_cast<void>(stack.allocBytes(allocationSizes[i], alignment));
    }

    // Free the entire memory
    stack.freeAll();

    // Allocation another buffer with a large enough size that proper allocation will not happen without proper frees
    const auto elementCount  = (stackSize - sizeof(pmm::StrictStackHeader) - alignment - 1) / sizeof(int);
    const auto newAllocation = static_cast<int*>(stack.allocBytes(elementCount * sizeof(int)));

    // Write to new allocation
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        newAllocation[i] = static_cast<int>(i + 3812);
    }

    // Verify the allocation is successful with data writes
    for (std::size_t i = 0; i < elementCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 3812), newAllocation[i]);
    }
}


/** @brief Verify that stack free, frees the buffer for future allocations. */
TEST_F(StrictStackTests, Free_FreesMemoryForSubsequentAllocations)
{

    // NOTE: 64 bytes is some leeway for buffer header and alignment
    constexpr auto leeway = 64;
    // Allocate some test data
    const auto largeData = stack.alloc<LargeData<STACK_SIZE - leeway>>();

    // Free it
    stack.free(largeData);

    const auto intV = stack.allocV<int>(STACK_SIZE / sizeof(int) - leeway);

    // Allocate Memory
    for (std::size_t i = 0; i < intV.size(); ++i)
    {
        intV[i] = static_cast<int>(i + 316);
    }

    // Verify the allocation is successful with data writes
    for (std::size_t i = 0; i < intV.size(); ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 316), intV[i]);
    }
}


/** @brief Verify that stack free, calls class destructor. */
TEST_F(StrictStackTests, Free_CallsClassDestructorForNonTrivialTypes)
{
    int numDestructorCalls = 0;
    const auto nonTrivial  = stack.alloc<DestructionTracker>(&numDestructorCalls);

    stack.free(nonTrivial);

    EXPECT_EQ(1, numDestructorCalls);
}


/** @brief Verify that stack free, frees the buffer for future allocations. */
TEST_F(StrictStackTests, FreeV_FreesMemoryForSubsequentAllocations)
{

    // NOTE: 64 bytes is some leeway for buffer header and alignment
    constexpr auto leeway = 64;
    // Should saturate the buffer as 4 * 1200 = 4800, near buffer size of 5_KB
    // Allocate some test data
    const auto listData = stack.allocV<int>(1200);
    // Free it
    stack.freeV(listData);

    const auto intV = stack.allocV<int>(STACK_SIZE / sizeof(int) - leeway);

    // Allocate Memory
    for (std::size_t i = 0; i < intV.size(); ++i)
    {
        intV[i] = static_cast<int>(i + 316);
    }

    // Verify the allocation is successful with data writes
    for (std::size_t i = 0; i < intV.size(); ++i)
    {
        EXPECT_EQ(static_cast<int>(i + 316), intV[i]);
    }
}


/** @brief Verify that stack freeV, calls class destructor for each data member. */
TEST_F(StrictStackTests, FreeV_CallsClassDestructorForNonTrivialTypes)
{
    // @Warning Not thread safe
    int numDestructorCalls       = 0;
    constexpr auto numAllocation = 500;
    auto nonTrivial              = stack.allocV<DestructionTracker>(numAllocation);
    for (auto& item : nonTrivial)
    {
        item.destructorCalledCount = &numDestructorCalls;
    }

    stack.freeV(nonTrivial);

    EXPECT_EQ(numAllocation, numDestructorCalls);
}


#ifndef NDEBUG
/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*, when allocating memory greater
 *        than the stack capacity.
 */
TEST_F(StrictStackTests, Allocation_GreaterThanCapacity_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(stackSize + 10)), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given a stack nearing its capacity(allocation > free).
 */
TEST_F(StrictStackTests, Allocation_NearFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    static_cast<void>(stack.allocBytes(stackSize - 50));
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500)), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given an uneven alignment(non-powers of 2).
 */
TEST_P(StrictStackAllocationAlignmentNonBinaryPowers, Allocation_InFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    pmm::Stack<pmm::stack::Strict> stack(5120);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500, GetParam())), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given alignment greater than 128.
 */
TEST_F(StrictStackTests, Allocation_GreaterThanSize_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500, 255)), ""); }


/**
 * @brief Verify that stack allocV triggers assertion in *DEBUG MODE*,
 *        when trying to allocate a zero size buffer.
 */
TEST_F(StrictStackTests, AllocV_SizeZero_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocV<int>(0)), ""); }


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing nullptr.
 */
TEST_F(StrictStackTests, FreeBytes_Nullptr_TriggersAssertion) { EXPECT_DEBUG_DEATH(stack.freeBytes(nullptr), ""); }


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing unallocated valid memory space.
 */
TEST_F(StrictStackTests, FreeBytes_UnallocatedMemoryAddress_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size));
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory + size + 1), "");
}


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing memory space below the base memory address.
 */
TEST_F(StrictStackTests, FreeBytes_BelowBufferMemoryAddress_TriggersAssertion)
{
    constexpr auto size      = 512;
    constexpr auto alignment = 8;
    // We are assuming worst case padding of 7 for alignment
    constexpr auto assumedHeaderSize = sizeof(pmm::StrictStackHeader) + alignment - 1;

    const auto memory = static_cast<char*>(stack.allocBytes(size, alignment));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory - assumedHeaderSize - 1), "");
}


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing memory above maximum memory address.
 */
TEST_F(StrictStackTests, FreeBytes_BeyondCapacity_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size, 8));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory + stackSize), "");
}

/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when memory out of order of allocation.
 */
TEST_F(StrictStackTests, FreeBytes_OutOfOrder_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size, 8));

    // Second allocation
    static_cast<void>(stack.allocBytes(40));

    EXPECT_DEBUG_DEATH(stack.freeBytes(memory), "");
}

/**
 * @brief Verify that stack resize triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(StrictStackTests, Resize_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(StrictStackTests, Resize_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(address, 128, 0)), "");
}


/**
 * @brief Verify that stack resize using resizeFast triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(StrictStackTests, ResizeFast_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize using resizeFast triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(StrictStackTests, ResizeFast_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(address, 128, 0)), "");
}


/**
 * @brief Verify that stack resize using resizeLast triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(StrictStackTests, ResizeLast_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize using resizeLast triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(StrictStackTests, ResizeLast_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeLast(address, 128, 0)), "");
}
// TODO: Resize last tests

#endif


/**************************************
 *                                    *
 *            FRIEND TESTS            *
 *                                    *
 **************************************/

namespace pmm
{

    /** @brief Verify that stack allocator is initialized with passed-in size. */
    TEST(StrictStackInitialization, InitializesDefaultStateAndBuffer)
    {
        constexpr std::size_t sizeInBytes = 512;
        const Stack<pmm::stack::Strict> stack{ sizeInBytes };

        EXPECT_EQ(sizeInBytes, stack._size);
        EXPECT_EQ(0, stack._offset);
        EXPECT_NE(nullptr, stack._buffer);
    }

    /** @brief Verify that allocation moves the stack offset by at least the request size. */
    TEST_F(StrictStackTests, Initialization_MovesOffsetAtleastByAllocationSize)
    {
        // Note: Due to alignment and header storage we cannot guarantee
        // that the allocation will be exactly the size
        constexpr std::size_t allocationSize = 120;

        static_cast<void>(stack.allocBytes(allocationSize));
        EXPECT_GE(stack._offset, allocationSize);
    }

    /** @brief Verify that freeAll moves the offset back to zero. */
    TEST_F(StrictStackTests, FreeAll_MovesOffsetAndPreviousOffsetToZero)
    {
        constexpr auto size = 512;
        // Initially allocate some memory
        static_cast<void>(stack.allocBytes(size));
        // Assert initial offset state is greater than size
        EXPECT_GT(stack._offset, size);

        // Free the entire stack
        stack.freeAll();

        // Offset is reset to 0
        EXPECT_EQ(0, stack._offset);
        EXPECT_EQ(0, stack._prevOffset);
    }

    /** @brief Verify that stack.resizeLast, moves the offset in the correct direction. */
    TEST_P(StrictStackResizeLast, ResizeLast_MovesOffsetInCorrectDirection)
    {
        Stack<pmm::stack::Strict> stack(20_KB);
        const auto [oldSize, newSize] = GetParam();
        auto oldMemory                = stack.allocBytes(oldSize);
        const auto oldOffset          = stack._offset;

        oldMemory = stack.resizeLast(oldMemory, oldSize, newSize);

        // If not cast to long long or any signed type, the difference will rotate to a large unsigned number
        const auto expectedOffsetDiff = static_cast<long long>(newSize - oldSize);
        const auto actualOffsetDiff   = static_cast<long long>(stack._offset - oldOffset);

        EXPECT_EQ(expectedOffsetDiff, actualOffsetDiff);
    }


} // namespace pmm
/** @} */
