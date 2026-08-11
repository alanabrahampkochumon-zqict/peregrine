/**
 * @file LooseStackTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Verify @ref pmm::Stack<pmm::stack::Loose> allocation, deallocation, and other related memory management logic.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "StackTestSetup.h"

#include <utility>

/**
 * @addtogroup T_PMM_Loose_Stack
 * @{
 */

namespace
{

    /**************************************
     *             TEST SETUP             *
     **************************************/

    /// @test Parameterized test fixture for loose stack's allocation alignment.
    class LooseStackAllocationAlignmentTests: public testing::TestWithParam<std::size_t>
    {};
    INSTANTIATE_TEST_SUITE_P(StackAlignmentTests, LooseStackAllocationAlignmentTests,
                             ::testing::Values(4, 8, 16, 32, 64, 512, 4096));


    /// @test Parameterized test fixture for loose stack's resize last <oldSize, newSize>.
    class LooseStackResizeLastTests: public testing::TestWithParam<std::pair<size_t, size_t>>
    {};
    INSTANTIATE_TEST_SUITE_P(StackResize, LooseStackResizeLastTests,
                             ::testing::Values(std::make_pair(256, 128), std::make_pair(128, 512),
                                               std::make_pair(2048, 4096), std::make_pair(4096, 2048)));
} // namespace



/**************************************
 *           RUNTIME TESTS            *
 **************************************/

/**
 * @brief Verify that size() returns the total size of the stack.
 */
TEST_F(LooseStackTests, Size_ReturnsTheSizeOfTheStack) { EXPECT_EQ(stackSize, stack.size()); }


/**
 * @brief Verify that freeSize() returns the total size of the stack, when no allocation is made.
 */
TEST_F(LooseStackTests, FreeSize_NoAllocations_ReturnsTheSizeOfTheStack) { EXPECT_EQ(stackSize, stack.freeSize()); }


/**
 * @brief Verify that freeSize() returns the size - (allocated size + padding),
 *        after an allocation is made.
 */
TEST_F(LooseStackTests, FreeSize_ReturnsStackSizeMinusPaddingAndAllocationSize)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    const auto header       = reinterpret_cast<pmm::LooseStackHeader*>(memory - sizeof(pmm::LooseStackHeader));
    const auto expectedSize = stackSize - (allocatedSize + header->padding);

    EXPECT_EQ(expectedSize, stack.freeSize());
}


/**
 * @brief Verify that freeSize() returns stack size, after an allocation is freed.
 */
TEST_F(LooseStackTests, FreeSize_AfterFreeingAllocation_ReturnsStackSize)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    stack.freeBytes(memory);
    EXPECT_EQ(stackSize, stack.freeSize());
}


/**
 * @brief Verify that freeSize() returns stack size, after all allocations are freed.
 */
TEST_F(LooseStackTests, FreeSize_AfterFreeingAllAllocations_ReturnsStackSize)
{
    constexpr std::size_t allocatedSize = 512;
    static_cast<void>(stack.allocBytes(allocatedSize));
    stack.clear();
    EXPECT_EQ(stackSize, stack.freeSize());
}


/**
 * @brief Verify that usedSize() returns 0, when no allocation is made.
 */
TEST_F(LooseStackTests, UsedSize_NoAllocations_ReturnsZero) { EXPECT_EQ(0, stack.usedSize()); }


/**
 * @brief Verify that usedSize() returns (allocated size + padding), after an allocation is made.
 */
TEST_F(LooseStackTests, UsedSize_ReturnsStackSizeMinusPaddingAndAllocationSize)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    const auto header       = reinterpret_cast<pmm::LooseStackHeader*>(memory - sizeof(pmm::LooseStackHeader));
    const auto expectedSize = allocatedSize + header->padding;

    EXPECT_EQ(expectedSize, stack.usedSize());
}


/**
 * @brief Verify that usedSize() returns zero, after an allocation is freed.
 */
TEST_F(LooseStackTests, UsedSize_AfterFreeingAllocation_ReturnsZero)
{
    constexpr std::size_t allocatedSize = 512;
    const auto memory                   = static_cast<char*>(stack.allocBytes(allocatedSize));
    stack.freeBytes(memory);
    EXPECT_EQ(0, stack.usedSize());
}


/**
 * @brief Verify that usedSize() returns zero, after all allocations are freed.
 */
TEST_F(LooseStackTests, UsedSize_ClearAllocation_ReturnsStackSize)
{
    constexpr std::size_t allocatedSize = 512;
    static_cast<void>(stack.allocBytes(allocatedSize));
    stack.clear();
    EXPECT_EQ(0, stack.usedSize());
}


TEST_F(LooseStackTests, MoveCtor_CopiesAttributesToNewObject)
{
    const pmm::Stack<> stack2 = std::move(stack);
    EXPECT_EQ(stackSize, stack2.freeSize());
    EXPECT_EQ(stackSize, stack2.size());
    EXPECT_EQ(0, stack2.usedSize());
    EXPECT_EQ(stackSize, stack2.getTelemetry().getStackSize());
}


TEST_F(LooseStackTests, MoveCtor_MovesTelemetry)
{

    static_cast<void>(stack.allocBytes(250, 16));
    // Must get the telemetry by value here else the reference will be reset
    const auto initialTelemetry = stack.getTelemetry();
    const pmm::Stack<> stack2   = std::move(stack);

    // Checking for telemetry equality
    EXPECT_EQ(initialTelemetry.getCurrentMemoryUsage(), stack2.getTelemetry().getCurrentMemoryUsage());
    EXPECT_EQ(initialTelemetry.getPeakMemoryUsage(), stack2.getTelemetry().getPeakMemoryUsage());
    EXPECT_EQ(initialTelemetry.getMinMemoryUsage(), stack2.getTelemetry().getMinMemoryUsage());
    EXPECT_EQ(initialTelemetry.getPeakPadding(), stack2.getTelemetry().getPeakPadding());
    EXPECT_EQ(initialTelemetry.getMinPadding(), stack2.getTelemetry().getMinPadding());
    EXPECT_EQ(initialTelemetry.getStackSize(), stack2.getTelemetry().getStackSize());
}



/**************************************
 *            ALLOC BYTES             *
 **************************************/

/** @brief Verify that stack allocation using allocBytes returns a valid pointer, given an empty stack. */
TEST_F(LooseStackTests, AllocBytes_ReturnsNonNullPtrOnEmptyStack)
{
    const auto dataPtr = stack.allocBytes(120);
    EXPECT_NE(nullptr, dataPtr);
}


/**
 * @brief Verify that stack allocation using allocBytes returns a valid pointer,
 *        given a non-empty stack with memory to spare.
 */
TEST_F(LooseStackTests, AllocBytes_ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.allocBytes(500);
    const auto dataPtr2 = stack.allocBytes(500);

    EXPECT_NE(nullptr, dataPtr1);
    EXPECT_NE(nullptr, dataPtr2);
    EXPECT_NE(dataPtr1, dataPtr2);
}



/** @brief Verify that memory allocated using allocBytes maintains data integrity. */
TEST_F(LooseStackTests, AllocBytes_RepeatedAllocationAndWritesDoNotCorruptData)
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
TEST_F(LooseStackTests, AllocBytes_HeaderIsStoredBehindReturnedAddress)
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
TEST_P(LooseStackAllocationAlignmentTests, AllocBytes_AlwaysReturnAnAlignedMemoryAddress)
{
    const auto alignment = this->GetParam();
    const auto blockSize = 5 * alignment;
    const auto stackSize = 10 * alignment;
    // pmm::Stack<> stack(stackSize);
    pmm::Stack<pmm::stack::Loose> stack{ stackSize };
    const void* dataAddress = stack.allocBytes(blockSize, alignment);

    // Verify returned address is 0 by using 2^n modulo trick
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(dataAddress) & (alignment - 1));
}



/**************************************
 *            ALLOC (OBJ)             *
 **************************************/

/** @brief Verify that stack allocation returns a valid pointer, given an empty stack. */
TEST_F(LooseStackTests, Alloc_ReturnsNonNullPtrOnEmptyStack)
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
TEST_F(LooseStackTests, Alloc_ReturnNonNullPtrOnNonEmptyStack)
{
    const auto dataPtr1 = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);
    const auto dataPtr2 = stack.alloc<Vec4>(5.0f, 1.0f, 31.0f, 3.0f);

    EXPECT_NE(nullptr, dataPtr1);
    EXPECT_NE(nullptr, dataPtr2);
    EXPECT_NE(dataPtr1, dataPtr2);
}



/** @brief Verify that allocated using alloc memory maintains data integrity. */
TEST_F(LooseStackTests, Alloc_RepeatedAllocationAndWritesDoNotCorruptData)
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
TEST_F(LooseStackTests, Alloc_HeaderIsStoredBehindReturnedAddress)
{
    const auto vector = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    const auto header =
        reinterpret_cast<pmm::LooseStackHeader*>(reinterpret_cast<char*>(vector) - sizeof(pmm::LooseStackHeader));
    EXPECT_GE(header->padding, alignof(Vec4));
}


/** @brief Verify that allocation using alloc always return an address aligned to the alignment of the type. */
TEST_P(LooseStackAllocationAlignmentTests, Alloc_AlwaysReturnAnAlignedMemoryAddress)
{
    pmm::Stack<pmm::stack::Loose> stack{ 512 };
    const auto vector = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    // Verify returned address is 0 by using 2^n module trick
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vector) & (alignof(Vec4) - 1));
}


/** @brief Verify that allocV can allocate a span of primitives. */
TEST_F(LooseStackTests, AllocV_AllocatesWriteablePrimitiveArray)
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
TEST_F(LooseStackTests, AllocV_AllocatesWriteTypeArray)
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
TEST_F(LooseStackTests, AllocV_BaseAddressAlignedToAlignmentOfType)
{
    // Allocate some memory to throw off alignment
    static_cast<void>(stack.allocBytes(2, 2));

    const auto vector = stack.allocV<Vec4>(10);
    // Base address % alignment == 0
    EXPECT_EQ(0, reinterpret_cast<uintptr_t>(vector.data()) & (alignof(Vec4) - 1));
}


/**************************************
 *            RESIZE TESTS            *
 **************************************/

/** @brief Verify that resizing the latest allocation to a smaller size, returns the same address. */
TEST_F(LooseStackTests, Resize_LatestAllocationSmallerSize_ReturnsSameAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resize(oldMemory, oldSize, newSize);

    EXPECT_EQ(oldMemory, newMemory);
}


/** @brief Verify that resizing the latest allocation to a larger size, returns new address. */
TEST_F(LooseStackTests, Resize_LatestAllocationLargerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resize(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing the latest allocation to a larger size, resizes the memory. */
TEST_F(LooseStackTests, Resize_LatestAllocationLargerSize_ResizesMemory)
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
TEST_F(LooseStackTests, Resize_SmallerSize_ReturnsSameAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = stack.resize(oldMemory, oldSize, newSize);

    EXPECT_EQ(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation to a larger size, returns the new address. */
TEST_F(LooseStackTests, Resize_LargerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = stack.resize(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation to a larger size, resizes the memory. */
TEST_F(LooseStackTests, Resize_LargerSize_ResizesMemory)
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
    const auto vec = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    for (std::size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }

    EXPECT_FLOAT_EQ(1.0f, vec->x);
    EXPECT_FLOAT_EQ(2.0f, vec->y);
    EXPECT_FLOAT_EQ(3.0f, vec->z);
    EXPECT_FLOAT_EQ(4.0f, vec->w);
}


/** @brief Verify that resizing any allocation to a larger size, copies over old memory contents. */
TEST_F(LooseStackTests, Resize_CopiesOverOldMemory)
{
    constexpr auto oldCount       = 48;
    constexpr auto newCount       = 128;
    constexpr std::size_t oldSize = sizeof(int) * oldCount, newSize = sizeof(int) * newCount;
    const auto oldMemory = static_cast<int*>(stack.allocBytes(oldSize));

    // Since the copy is logically triggered only for allocations that are "moved"
    // We need a second allocation to trigger it
    static_cast<void>(stack.allocBytes(oldSize));

    // Store some random value in memory
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        oldMemory[i] = static_cast<int>(2813 + i);
    }

    const auto newMemory = static_cast<int*>(stack.resize(oldMemory, oldSize, newSize));

    // Verify that memory is copied.
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }
}


/** @brief Verify that resizing a allocation allocated prior to latest allocation to smaller size, does not corrupt
 * memory. */
TEST_F(LooseStackTests, Resize_NonLatestAllocShrinking_CorruptsNoMemory)
{
    constexpr auto oldCount       = 64;
    constexpr auto newCount       = 32;
    constexpr std::size_t oldSize = sizeof(int) * oldCount, newSize = sizeof(int) * newCount;

    // Allocate the memory
    const auto firstAlloc = static_cast<int*>(stack.allocBytes(oldSize));
    // Fill the first allocation with data
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        firstAlloc[i] = static_cast<int>(2813 + i);
    }
    // Allocate another memory
    const auto secondAlloc = static_cast<int*>(stack.allocBytes(oldSize));
    // Fill the buffer
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        secondAlloc[i] = static_cast<int>(5123 + i);
    }
    // Resize first buffer to be of smaller size
    const auto newMemory = static_cast<int*>(stack.resize(firstAlloc, oldSize, newSize));

    // Verify both memory are uncorrupted
    for (std::size_t i = 0; i < newCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }

    // Since second buffer is not resized, it must be uncorrupted till `oldCount`
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(5123 + i), secondAlloc[i]);
    }
}


/** @brief Verify that resizing a allocation allocated prior to latest allocation to smaller size, does not corrupt
 * memory. */
TEST_F(LooseStackTests, Resize_NonLatestAllocExpansion_CorruptsNoMemory)
{
    constexpr auto oldCount       = 16;
    constexpr auto newCount       = 32;
    constexpr std::size_t oldSize = sizeof(int) * oldCount, newSize = sizeof(int) * newCount;

    // Allocate the memory
    const auto firstAlloc = static_cast<int*>(stack.allocBytes(oldSize));
    // Fill the first allocation with data
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        firstAlloc[i] = static_cast<int>(2813 + i);
    }
    // Allocate another memory
    const auto secondAlloc = static_cast<int*>(stack.allocBytes(oldSize));
    // Fill the buffer
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        secondAlloc[i] = static_cast<int>(5123 + i);
    }
    // Resize first buffer to be of smaller size
    const auto newMemory = static_cast<int*>(stack.resize(firstAlloc, oldSize, newSize));

    // Write the pattern into expanded region
    for (std::size_t i = oldCount; i < newCount; ++i)
    {
        newMemory[i] = static_cast<int>(2813 + i);
    }

    // Verify both memory are uncorrupted
    for (std::size_t i = 0; i < newCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }

    for (std::size_t i = 0; i < oldCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(5123 + i), secondAlloc[i]);
    }
}


/** @brief Verify that resizing the latest allocation to smaller size, does not corrupt memory. */
TEST_F(LooseStackTests, Resize_LatestAllocationShrinking_CorruptsNoMemory)
{
    constexpr auto oldCount       = 64;
    constexpr auto newCount       = 32;
    constexpr std::size_t oldSize = sizeof(int) * oldCount, newSize = sizeof(int) * newCount;

    // Allocate the memory
    const auto firstAlloc = static_cast<int*>(stack.allocBytes(oldSize));
    // Fill the first allocation with data
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        firstAlloc[i] = static_cast<int>(2813 + i);
    }
    // Buffer is resized here to ensure that we have the latest allocated resized
    // Resize first buffer to be of smaller size
    const auto newMemory = static_cast<int*>(stack.resize(firstAlloc, oldSize, newSize));

    // Allocate another memory
    const auto secondAlloc = static_cast<int*>(stack.allocBytes(oldSize));
    // Fill the buffer
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        secondAlloc[i] = static_cast<int>(5123 + i);
    }

    // Verify both memory are uncorrupted
    for (std::size_t i = 0; i < newCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }

    for (std::size_t i = 0; i < oldCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(5123 + i), secondAlloc[i]);
    }
}


/** @brief Verify that resizing the latest allocation to larger size, does not corrupt memory. */
TEST_F(LooseStackTests, Resize_LatestAllocationExpansion_CorruptsNoMemory)
{
    constexpr auto oldCount       = 16;
    constexpr auto newCount       = 32;
    constexpr std::size_t oldSize = sizeof(int) * oldCount, newSize = sizeof(int) * newCount;

    // Allocate the memory
    const auto firstAlloc = static_cast<int*>(stack.allocBytes(oldSize));
    // Fill the first allocation with data
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        firstAlloc[i] = static_cast<int>(2813 + i);
    }

    // Buffer is resized here to ensure that we have the latest allocated resized
    // Resize first buffer to be of smaller size
    const auto newMemory = static_cast<int*>(stack.resize(firstAlloc, oldSize, newSize));
    // Write the pattern into expanded region
    for (std::size_t i = oldCount; i < newCount; ++i)
    {
        newMemory[i] = static_cast<int>(2813 + i);
    }

    // Allocate another memory
    const auto secondAlloc = static_cast<int*>(stack.allocBytes(oldSize));
    // Fill the buffer
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        secondAlloc[i] = static_cast<int>(5123 + i);
    }

    // Verify both memory are uncorrupted
    for (std::size_t i = 0; i < newCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }

    for (std::size_t i = 0; i < oldCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(5123 + i), secondAlloc[i]);
    }
}


/** @brief Verify that resizing the latest allocation using to a smaller size resizeFast, returns new address. */
TEST_F(LooseStackTests, ResizeFast_LatestAllocationSmallerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resizeFast(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing the latest allocation to a larger size using resizeFast, returns new address. */
TEST_F(LooseStackTests, ResizeFast_LatestAllocationLargerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resizeFast(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation to a smaller size using resizeFast, returns new address. */
TEST_F(LooseStackTests, ResizeFast_SmallerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = stack.resizeFast(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation to a larger size using resizeFast, returns the new address. */
TEST_F(LooseStackTests, ResizeFast_LargerSize_ReturnsNewAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);

    static_cast<void>(stack.allocBytes(oldSize)); // Second allocation

    const auto newMemory = stack.resizeFast(oldMemory, oldSize, newSize);

    EXPECT_NE(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation using resizeFast, resizes the memory. */
TEST_F(LooseStackTests, ResizeFast_ResizesMemory)
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
    const auto vec = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    for (std::size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }

    EXPECT_FLOAT_EQ(1.0f, vec->x);
    EXPECT_FLOAT_EQ(2.0f, vec->y);
    EXPECT_FLOAT_EQ(3.0f, vec->z);
    EXPECT_FLOAT_EQ(4.0f, vec->w);
}


/** @brief Verify that resizing any allocation to a larger size using resizeFast, copies over old memory contents. */
TEST_F(LooseStackTests, ResizeFast_CopiesOverOldMemory)
{
    constexpr auto oldCount       = 48;
    constexpr auto newCount       = 128;
    constexpr std::size_t oldSize = sizeof(int) * oldCount, newSize = sizeof(int) * newCount;
    const auto oldMemory = static_cast<int*>(stack.allocBytes(oldSize));

    // Store some random value in memory
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        oldMemory[i] = static_cast<int>(2813 + i);
    }

    const auto newMemory = static_cast<int*>(stack.resizeFast(oldMemory, oldSize, newSize));

    // Verify that memory is copied.
    for (std::size_t i = 0; i < oldCount; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }
}


/** @brief Verify that resizing the latest allocation to a smaller size using resizeLast, returns the same address. */
TEST_F(LooseStackTests, ResizeLast_LatestAllocationSmallerSize_ReturnsSameAddress)
{
    constexpr auto oldSize = 128, newSize = 64;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resizeLast(oldMemory, oldSize, newSize);

    EXPECT_EQ(oldMemory, newMemory);
}


/** @brief Verify that resizing the latest allocation to a larger size using resizeLast, returns the same address. */
TEST_F(LooseStackTests, ResizeLast_LatestAllocationLargerSize_ReturnsSameAddress)
{
    constexpr auto oldSize = 128, newSize = 256;
    const auto oldMemory = stack.allocBytes(oldSize);
    const auto newMemory = stack.resizeLast(oldMemory, oldSize, newSize);

    EXPECT_EQ(oldMemory, newMemory);
}


/** @brief Verify that resizing any allocation using resizeLast, resizes the memory. */
TEST_F(LooseStackTests, ResizeLast_ResizesMemory)
{
    constexpr auto count          = 128;
    constexpr std::size_t oldSize = 128, newSize = sizeof(int) * count;
    const auto oldMemory = stack.allocBytes(oldSize);

    const auto newMemory = static_cast<int*>(stack.resizeLast(oldMemory, oldSize, newSize));

    for (std::size_t i = 0; i < count; ++i)
    {
        newMemory[i] = static_cast<int>(2813 + i);
    }


    // Allocate a new vector
    const auto vec = stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f);

    for (std::size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(static_cast<int>(2813 + i), newMemory[i]);
    }

    EXPECT_FLOAT_EQ(1.0f, vec->x);
    EXPECT_FLOAT_EQ(2.0f, vec->y);
    EXPECT_FLOAT_EQ(3.0f, vec->z);
    EXPECT_FLOAT_EQ(4.0f, vec->w);
}



/**************************************
 *            FREE TESTS              *
 **************************************/

/** @brief Verify that stack free, frees up memory for subsequent allocations. */
TEST_F(LooseStackTests, FreeBytes_FreesMemoryForSubsequentAllocations)
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
TEST_F(LooseStackTests, FreeBytes_MultipleTimesMakesRoomInTheStack)
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
TEST_F(LooseStackTests, Clear_FreesTheEntireStack)
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
    stack.clear();

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
TEST_F(LooseStackTests, Free_FreesMemoryForSubsequentAllocations)
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
TEST_F(LooseStackTests, Free_CallsClassDestructorForNonTrivialTypes)
{
    int numDestructorCalls = 0;
    const auto nonTrivial  = stack.alloc<DestructionTracker>(&numDestructorCalls);

    stack.free(nonTrivial);

    EXPECT_EQ(1, numDestructorCalls);
}


/** @brief Verify that stack free, frees the buffer for future allocations. */
TEST_F(LooseStackTests, FreeV_FreesMemoryForSubsequentAllocations)
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
TEST_F(LooseStackTests, FreeV_CallsClassDestructorForNonTrivialTypes)
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




/**************************************
 *            FRIEND TESTS            *
 **************************************/

namespace pmm
{

    /** @brief Verify that stack allocator is initialized with passed-in size. */
    TEST(LooseStackInitializationTests, InitializesDefaultStateAndBuffer)
    {
        constexpr std::size_t sizeInBytes = 512;
        const Stack<pmm::stack::Loose> stack{ sizeInBytes };

        EXPECT_EQ(sizeInBytes, stack._stackSize);
        EXPECT_EQ(0, stack._offset);
        EXPECT_NE(nullptr, stack._buffer);
    }

    /** @brief Verify that allocation moves the stack offset by at least the request size. */
    TEST_F(LooseStackTests, Initialization_MovesOffsetAtleastByAllocationSize)
    {
        // Note: Due to alignment and header storage we cannot guarantee
        // that the allocation will be exactly the size
        constexpr std::size_t allocationSize = 120;

        static_cast<void>(stack.allocBytes(allocationSize));
        EXPECT_GE(stack._offset, allocationSize);
    }

    /** @brief Verify that freeAll moves the offset back to zero. */
    TEST_F(LooseStackTests, Clear_MovesOffsetToZero)
    {
        constexpr auto size = 512;
        // Initially allocate some memory
        static_cast<void>(stack.allocBytes(size));
        // Assert initial offset state is greater than size
        EXPECT_GT(stack._offset, size);

        // Free the entire stack
        stack.clear();

        // Offset is reset to 0
        EXPECT_EQ(0, stack._offset);
    }

    /** @brief Verify that stack.resizeLast, moves the offset in the correct direction. */
    TEST_P(LooseStackResizeLastTests, ResizeLast_MovesOffsetInCorrectDirection)
    {
        Stack<pmm::stack::Loose> stack(20_KB);
        const auto [oldSize, newSize] = GetParam();
        auto oldMemory                = stack.allocBytes(oldSize);
        const auto oldOffset          = stack._offset;

        oldMemory = stack.resizeLast(oldMemory, oldSize, newSize);

        // If not cast to long long or any signed type, the difference will rotate to a large unsigned number
        const auto expectedOffsetDiff = static_cast<long long>(newSize - oldSize);
        const auto actualOffsetDiff   = static_cast<long long>(stack._offset - oldOffset);

        EXPECT_EQ(expectedOffsetDiff, actualOffsetDiff);
    }


    TEST_F(LooseStackTests, MoveCtor_NullsOutInternalBuffer)
    {
        [[maybe_unused]] const Stack<> stack2 = std::move(stack);
        // NOLINT(bugprone-use-after-move)
        EXPECT_EQ(nullptr, stack._buffer);
        EXPECT_EQ(0, stack._offset);
        EXPECT_EQ(0, stack._stackSize);
    }

    /**
     * @brief Verify that move constructor moves all data members, including buffer into new object.
     */
    TEST_F(LooseStackTests, MoveCtor_MovesBufferIntoNewObject)
    {
        const auto initialPointer = stack._buffer;
        const auto initialOffset  = stack._offset;

        const Stack<> stack2 = std::move(stack);
        EXPECT_EQ(initialPointer, stack2._buffer);
        EXPECT_EQ(initialOffset, stack2._offset);
        EXPECT_EQ(stackSize, stack2._stackSize);
    }


    TEST_F(LooseStackTests, MoveOperator_NullsOutInternalBuffer)
    {
        [[maybe_unused]] Stack<> stack2(256);
        static_cast<void>(stack2 = std::move(stack));
        EXPECT_EQ(nullptr, stack._buffer);
        EXPECT_EQ(0, stack._offset);
        EXPECT_EQ(0, stack._stackSize);
    }



    TEST_F(LooseStackTests, MoveOperator_MovesBufferIntoNewObject)
    {
        const auto initialPointer = stack._buffer;

        Stack<> stack2(256);
        stack2 = std::move(stack);

        EXPECT_EQ(initialPointer, stack2._buffer);
        EXPECT_EQ(0, stack2._offset);
        EXPECT_EQ(stackSize, stack2._stackSize);
        // EXPECT_EQ(0, stack2._prevOffset);
    }


    TEST_F(LooseStackTests, MoveOperator_SelfAssignmentReturnsTheSameStack)
    {
        const auto initialAddress = reinterpret_cast<uintptr_t>(stack._buffer);
        const auto initialOffset  = stack._offset;
        // const auto initialPrevOffset = stack._prevOffset;

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wself-move"
#endif
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-move"
#endif
        stack = std::move(stack);
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

        EXPECT_EQ(initialAddress, reinterpret_cast<uintptr_t>(stack._buffer));
        EXPECT_EQ(initialOffset, stack._offset);
        // EXPECT_EQ(initialPrevOffset, stack._prevOffset);
    }


    TEST_F(LooseStackTests, MoveOperator_DeletingOriginalStackDoNotDeleteTheNewStacksMemory)
    {
        Stack<> stack2(256);
        constexpr auto size = 512;

        {
            stack2 = std::move(stack);
        }
        EXPECT_NE(nullptr, stack2._buffer);

        // Write arbitrary data into the buffer
        // NOTE: i % 255 ensures that uint8_t does not overflow
        for (uint32_t i = 0; i < size; ++i)
        {
            stack2._buffer[i] = i % 255;
        }

        // Read the value from buffer
        for (uint32_t i = 0; i < size / 4; i += 4)
        {
            EXPECT_EQ(i % 255, stack2._buffer[i]);
        }
    }



    TEST_F(LooseStackTests, ZeroOut_ZeroesOutTheInternalBuffer)
    {
        // Fill the buffer with arbitrary data
        for (size_t i = 0; i < stack.size(); ++i)
        {
            stack._buffer[i] = static_cast<uint8_t>(i % 255);
        }
        // This call unnecessary for testing, but adhering to how the method is supposed to be called,
        // we are leaving it here.
        stack.clear();

        // Zero-out
        stack.zeroOut();

        // Assert stack's buffer is cleared
        for (size_t i = 0; i < stack.size(); ++i)
        {
            EXPECT_EQ(0, stack._buffer[i]);
        }
    }


    TEST(UmanagedLooseStackTests, ZeroOut_ZeroesOutTheInternalBuffer)
    {
        // Allocate and clear the stack to ensure there is some data
        const auto stackSize = 4_KB;
        const auto buffer    = new uint8_t[stackSize];
        Stack<stack::Loose, UnmanagedMemory> stack{ buffer, stackSize };

        // Fill the buffer with arbitrary data
        for (size_t i = 0; i < stack.size(); ++i)
        {
            buffer[i] = static_cast<uint8_t>(i % 255);
        }
        // This call unnecessary for testing, but adhering to how the method is supposed to be called,
        // we are leaving it here.
        stack.clear();

        // Zero-out
        stack.zeroOut();

        // Assert stack's buffer is cleared
        for (size_t i = 0; i < stack.size(); ++i)
        {
            EXPECT_EQ(0, buffer[i]);
        }

        delete[] buffer;
    }


} // namespace pmm
/** @} */
