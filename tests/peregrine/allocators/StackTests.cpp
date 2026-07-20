/**
 * @file StackTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Verify @ref pmm::StackAllocator allocation, deallocation, and other related memory management logic.
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
class StackTests: public ::testing::Test
{

public:
    std::size_t stackSize{ STACK_SIZE };
    pmm::Stack<> stack{ stackSize };
};


class StackAllocationAlignment: public ::testing::TestWithParam<std::size_t>
{};
INSTANTIATE_TEST_SUITE_P(
    StackAlignmentTests, StackAllocationAlignment,
    ::testing::Values(4, 8, 16, 32, 64)); // TODO: Add more alignment and with different types of arena padding type

#ifndef NDEBUG
class StackAllocationAlignmentNonBinaryPowers: public ::testing::TestWithParam<std::size_t>
{};
INSTANTIATE_TEST_SUITE_P(NonPowersOfTwo, StackAllocationAlignmentNonBinaryPowers, ::testing::Values(0, 1, 3, 5, 111));
#endif

/**
 * @addtogroup T_PMM_Stack
 * @{
 */

/**************************************
 *                                    *
 *           RUNTIME TESTS            *
 *                                    *
 **************************************/

/**
 * @brief Verify that size() of stack allocator returns to the total size of the stack.
 */
TEST_F(StackTests, Size_ReturnsTheSizeOfTheStack) { EXPECT_EQ(stackSize, stack.size()); }


/**************************************
 *                                    *
 *            ALLOC BYTES             *
 *                                    *
 **************************************/

/** @brief Verify that stack allocation using allocBytes returns a valid pointer, given an empty stack. */
TEST_F(StackTests, AllocBytes_ReturnsNonNullPtrOnEmptyStack)
{
    const auto dataPtr = stack.allocBytes(120);
    EXPECT_NE(nullptr, dataPtr);
}


/**
 * @brief Verify that stack allocation using allocBytes returns a valid pointer,
 *        given a non-empty stack with memory to spare.
 */
TEST_F(StackTests, AllocBytes_ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.allocBytes(500);
    const auto dataPtr2 = stack.allocBytes(500);

    EXPECT_NE(nullptr, dataPtr1);
    EXPECT_NE(nullptr, dataPtr2);
    EXPECT_NE(dataPtr1, dataPtr2);
}



/** @brief Verify that memory allocated using allocBytes maintains data integrity. */
TEST_F(StackTests, AllocBytes_RepeatedAllocationAndWritesDoNotCorruptData)
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
TEST_F(StackTests, AllocBytes_HeaderIsStoredBehindReturnedAddress)
{
    constexpr auto alignment = 8;
    const auto memoryStart   = static_cast<char*>(stack.allocBytes(500, alignment));

    const auto header =
        reinterpret_cast<pmm::LooseStackHeader*>(reinterpret_cast<char*>(memoryStart) - sizeof(pmm::LooseStackHeader));
    EXPECT_GE(header->padding, alignment);
}


/**
 * @brief Verify that allocation using allocBytes always return an address aligned
 *        to the specified boundary.
 */
TEST_P(StackAllocationAlignment, AllocBytes_AlwaysReturnAnAlignedMemoryAddress)
{
    const auto alignment = this->GetParam();
    const auto blockSize = 5 * alignment;

    pmm::Stack<> stack{ 8192 }; // 8KB Stack
    const void* dataAddress = stack.allocBytes(blockSize, alignment);

    // Verify returned address is 0 by using 2^n module trick
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(dataAddress) & (alignment - 1));
}



/**************************************
 *                                    *
 *            ALLOC (OBJ)             *
 *                                    *
 **************************************/

/** @brief Verify that stack allocation returns a valid pointer, given an empty stack. */
TEST_F(StackTests, Alloc_ReturnsNonNullPtrOnEmptyStack)
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
TEST_F(StackTests, Alloc_ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    const auto dataPtr2 = stack.alloc<Vec4>(5.0f, 1.0f, 31.0f, 3.0f);

    EXPECT_NE(nullptr, dataPtr1);
    EXPECT_NE(nullptr, dataPtr2);
    EXPECT_NE(dataPtr1, dataPtr2);
}



/** @brief Verify that allocated using alloc memory maintains data integrity. */
TEST_F(StackTests, Alloc_RepeatedAllocationAndWritesDoNotCorruptData)
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
TEST_F(StackTests, Alloc_HeaderIsStoredBehindReturnedAddress)
{
    const auto vector = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    const auto header =
        reinterpret_cast<pmm::LooseStackHeader*>(reinterpret_cast<char*>(vector) - sizeof(pmm::LooseStackHeader));
    EXPECT_GE(header->padding, alignof(Vec4));
}


/** @brief Verify that allocation using alloc always return an address aligned to the alignment of the type. */
TEST_P(StackAllocationAlignment, Alloc_AlwaysReturnAnAlignedMemoryAddress)
{
    pmm::Stack<> stack{ 512 };
    const auto vector = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    // Verify returned address is 0 by using 2^n module trick
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vector) & (alignof(Vec4) - 1));
}


/** @brief Verify that allocV can allocate a span of primitives. */
TEST_F(StackTests, AllocV_AllocatesWriteablePrimitiveArray)
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
TEST_F(StackTests, AllocV_AllocatesWriteTypeArray)
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
TEST_F(StackTests, AllocV_BaseAddressAlignedToAlignmentOfType)
{
    // Allocate some memory to throw off alignment
    static_cast<void>(stack.allocBytes(2, 2));

    const auto vector = stack.allocV<Vec4>(10);
    // Base address % alignment == 0
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vector.data()) & (alignof(Vec4) - 1));
}


/**************************************
 *                                    *
 *            RESIZE TESTS            *
 *                                    *
 **************************************/

/** @brief Verify that resizing the latest allocation to a smaller size, returns the same address. */
TEST_F(StackTests, Resize_LatestAllocationSmallerSize_ReturnsSameAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resize(oldMemory, oldSize, newSize);

    EXPECT_EQ(oldMemory, newMemory);
}

/** @brief Verify that resizing the latest allocation to a smaller size, leaves memory for new allocations. */
TEST_F(StackTests, Resize_LatestAllocationSmallerSize_LeavesMemoryForNewAllocations)
{
    constexpr auto padding    = 128;
    constexpr auto count      = 1024;
    const std::size_t oldSize = stackSize - padding, newSize = 128;
    const auto oldMemory = stack.allocBytes(oldSize);
    // Resize
    static_cast<void>(stack.resize(oldMemory, oldSize, newSize));

    // Near limit memory allocation, but there is a lot of leeway. 4_KB out of ~5_KB
    constexpr auto newAllocationSize = sizeof(int) * count;
    const auto newMemory             = static_cast<int*>(stack.allocBytes(newAllocationSize));

    // NOTE: While the read write cycles are unnecessary in debug mode due to asserts,
    //       we need them to verify in release mode.
    // Write into memory
    for (std::size_t i = 0; i < count; ++i)
    {
        newMemory[i] = static_cast<int>(2813 + i);
    }
    // Allocate a new vector
    const auto vector = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    // Confirm neither of the latest allocations are corrupted
    EXPECT_FLOAT_EQ(1.0f, vector->x);
    EXPECT_FLOAT_EQ(2.0f, vector->y);
    EXPECT_FLOAT_EQ(3.0f, vector->z);
    EXPECT_FLOAT_EQ(4.0f, vector->w);

    for (std::size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }
}


/** @brief Verify that resizing the latest allocation to a larger size, returns the same address. */
TEST_F(StackTests, Resize_LatestAllocationLargerSize_ReturnsSameAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resize(oldMemory, oldSize, newSize);

    EXPECT_EQ(oldMemory, newMemory);
}


/** @brief Verify that resizing the latest allocation to a larger size, resizes the memory. */
TEST_F(StackTests, Resize_LatestAllocationLargerSize_ResizesMemory)
{
    constexpr auto count          = 128;
    constexpr std::size_t oldSize = 128, newSize = sizeof(int) * count;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = static_cast<int*>(stack.resize(oldMemory, oldSize, newSize));


    for (std::size_t i = 0; i < count; ++i)
    {
        newMemory[i] = static_cast<int>(2813 + i);
    }

    // Allocate a new vector
    static_cast<void>(stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));

    for (std::size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }
}


/** @brief Verify that resizing any allocation to a smaller size, returns the same address. */
TEST_F(StackTests, Resize_SmallerSize_ReturnsSameAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = stack.resize(oldMemory, oldSize, newSize);

    EXPECT_EQ(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation to a larger size, returns the new address. */
TEST_F(StackTests, Resize_LargerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = stack.resize(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation to a larger size, resizes the memory. */
TEST_F(StackTests, Resize_LargerSize_ResizesMemory)
{
    constexpr auto count          = 128;
    constexpr std::size_t oldSize = 128, newSize = sizeof(int) * count;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = static_cast<int*>(stack.resize(oldMemory, oldSize, newSize));

    for (std::size_t i = 0; i < count; ++i)
    {
        newMemory[i] = static_cast<int>(2813 + i);
    }


    // Allocate a new vector
    static_cast<void>(stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));

    for (std::size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }
}


/** @brief Verify that resizing the latest allocation using resizeFast to a smaller size, returns new address. */
TEST_F(StackTests, ResizeFast_LatestAllocationSmallerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resizeFast(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing the latest allocation to a larger size using resizeFast, returns new address. */
TEST_F(StackTests, ResizeFast_LatestAllocationLargerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resizeFast(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation to a smaller size using resizeFast, returns new address. */
TEST_F(StackTests, ResizeFast_SmallerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = stack.resizeFast(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation to a larger size using resizeFast, returns the new address. */
TEST_F(StackTests, ResizeFast_LargerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = stack.resizeFast(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation using resizeFast, resizes the memory. */
TEST_F(StackTests, ResizeFast_ResizesMemory)
{
    constexpr auto count          = 128;
    constexpr std::size_t oldSize = 128, newSize = sizeof(int) * count;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = static_cast<int*>(stack.resizeFast(oldMemory, oldSize, newSize));

    for (std::size_t i = 0; i < count; ++i)
    {
        newMemory[i] = static_cast<int>(2813 + i);
    }


    // Allocate a new vector
    static_cast<void>(stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));

    for (std::size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }
}


/**************************************
 *                                    *
 *            FREE TESTS              *
 *                                    *
 **************************************/

/** @brief Verify that stack free, frees up memory for subsequent allocations. */
TEST_F(StackTests, FreeBytes_FreesMemoryForSubsequentAllocations)
{
    constexpr std::size_t alignment = 8;
    const auto usableSize = stackSize - 128; // A big offset is used since we need to make room for header + alignment
    const auto freeMemory = static_cast<char*>(stack.allocBytes(usableSize, alignment));
    // Free the memory
    stack.freeBytes(freeMemory);

    // Allocation another buffer
    const auto elementCount  = (stackSize - alignment - 1) / sizeof(int);
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
TEST_F(StackTests, FreeBytes_MultipleTimesMakesRoomInTheStack)
{
    constexpr std::size_t alignment = 8;
    // Last 128 is the offset used to make room for header + alignment
    const std::array<std::size_t, 4> allocationSizes{ 128, 256, 1024, stackSize - 128 - 256 - 1024 - 128 };
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
    const auto elementCount  = (stackSize - alignment - 1) / sizeof(int);
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
TEST_F(StackTests, FreeAll_FreesTheEntireStack)
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
    const auto elementCount  = (stackSize - alignment - 1) / sizeof(int);
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
TEST_F(StackTests, Free_FreesMemoryForSubsequentAllocations)
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
TEST_F(StackTests, Free_CallsClassDestructorForNonTrivialTypes)
{
    int numDestructorCalls = 0;
    const auto nonTrivial  = stack.alloc<DestructionTracker>(&numDestructorCalls);

    stack.free(nonTrivial);

    EXPECT_EQ(1, numDestructorCalls);
}


/** @brief Verify that stack free, frees the buffer for future allocations. */
TEST_F(StackTests, FreeV_FreesMemoryForSubsequentAllocations)
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
TEST_F(StackTests, FreeV_CallsClassDestructorForNonTrivialTypes)
{
    // @Warning Not thread safe
    int numDestructorCalls = 0;
    constexpr auto numAllocation = 500;
    auto nonTrivial  = stack.allocV<DestructionTracker>(numAllocation);
    for (auto& item: nonTrivial)
        item.destructorCalledCount = &numDestructorCalls;

    stack.freeV(nonTrivial);

    EXPECT_EQ(numAllocation, numDestructorCalls);
}


#ifndef NDEBUG
/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*, when allocating memory greater
 *        than the stack capacity.
 */
TEST_F(StackTests, Allocation_GreaterThanCapacity_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(stackSize + 10)), ""); }


/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given a stack nearing its capacity(allocation > free).
 */
TEST_F(StackTests, Allocation_NearFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    static_cast<void>(stack.allocBytes(stackSize - 50));
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500)), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given an uneven alignment(non-powers of 2).
 */
TEST_P(StackAllocationAlignmentNonBinaryPowers, Allocation_InFullStack_TriggersAssertion)
{
    // Allocate a big chunk to fill the stack near capacity
    pmm::Stack<> stack(5120);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500, GetParam())), "");
}

/**
 * @brief Verify that stack allocation triggers assertion in *DEBUG MODE*,
 *        given alignment greater than 128.
 */
TEST_F(StackTests, Allocation_GreaterThanSize_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocBytes(500, 255)), ""); }


/**
 * @brief Verify that stack allocV triggers assertion in *DEBUG MODE*,
 *        when trying to allocate a zero size buffer.
 */
TEST_F(StackTests, AllocV_SizeZero_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.allocV<int>(0)), ""); }


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing nullptr.
 */
TEST_F(StackTests, FreeBytes_Nullptr_TriggersAssertion) { EXPECT_DEBUG_DEATH(stack.freeBytes(nullptr), ""); }


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing unallocated valid memory space.
 */
TEST_F(StackTests, FreeBytes_UnallocatedMemoryAddress_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size));
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory + size + 1), "");
}


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing memory space below the base memory address.
 */
TEST_F(StackTests, FreeBytes_BelowBufferMemoryAddress_TriggersAssertion)
{
    constexpr auto size      = 512;
    constexpr auto alignment = 8;
    // We are assuming worst case padding of 7 for alignment
    constexpr auto assumedHeaderSize = sizeof(pmm::LooseStackHeader) + alignment - 1;

    const auto memory = static_cast<char*>(stack.allocBytes(size, alignment));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory - assumedHeaderSize - 1), "");
}


/**
 * @brief Verify that stack free triggers assertion in *DEBUG MODE*,
 *        when freeing memory above maximum memory address.
 */
TEST_F(StackTests, FreeBytes_BeyondCapacity_TriggersAssertion)
{
    constexpr auto size = 512;
    const auto memory   = static_cast<char*>(stack.allocBytes(size, 8));
    // Move 1 below assume header size
    EXPECT_DEBUG_DEATH(stack.freeBytes(memory + stackSize), "");
}


/**
 * @brief Verify that stack resize triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(StackTests, Resize_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(StackTests, Resize_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resize(address, 128, 0)), "");
}


/**
 * @brief Verify that stack resize using resizeFast triggers assertion in *DEBUG MODE*,
 *        when trying to resize a nullptr.
 */
TEST_F(StackTests, ResizeFast_Nullptr_TriggersAssertion)
{ EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(nullptr, 120, 256)), ""); }


/**
 * @brief Verify that stack resize using resizeFast triggers assertion in *DEBUG MODE*,
 *        when trying to resize to 0.
 */
TEST_F(StackTests, ResizeFast_ToZero_TriggersAssertion)
{
    const auto address = stack.allocBytes(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(stack.resizeFast(address, 128, 0)), "");
}




#endif


/**************************************
 *                                    *
 *            FRIEND TESTS            *
 *                                    *
 **************************************/

namespace pmm
{

    /** @brief Verify that stack allocator is initialized with passed-in size. */
    TEST(StackInitialization, InitializesDefaultStateAndBuffer)
    {
        constexpr std::size_t sizeInBytes = 512;
        const Stack<> stack{ sizeInBytes };

        EXPECT_EQ(sizeInBytes, stack._size);
        EXPECT_EQ(0, stack._offset);
        EXPECT_NE(nullptr, stack._buffer); // TODO: Investigate leak-> Add Destructor
    }

    /** @brief Verify that allocation moves the stack offset by at least the request size. */
    TEST_F(StackTests, Initialization_MovesOffsetAtleastByAllocationSize)
    {
        // Note: Due to alignment and header storage we cannot guarantee
        // that the allocation will be exactly the size
        constexpr std::size_t allocationSize = 120;

        static_cast<void>(stack.allocBytes(allocationSize));
        EXPECT_GE(stack._offset, allocationSize);
    }

    /** @brief Verify that freeAll moves the offset back to zero. */
    TEST_F(StackTests, FreeAll_MovesOffsetToZero)
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
    }

} // namespace pmm
/** @} */
