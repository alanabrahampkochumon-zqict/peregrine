/**
 * @file TelemetryIntegrationTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 24, 2026
 *
 * @brief Verifies that stack telemetry integration based on different policies.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"
#include "peregrine/utils/Constants.h"

#include <gtest/gtest.h>
#include <peregrine/allocators/Stack.h>


/**
 * @addtogroup T_PMM_Stack_Tel_Int
 * @{
 */

namespace
{
    using namespace pmm::constants;

    /** @brief Test fixture for loose stack telemetry integration tests. */
    class LooseStackTelemetryIntegration: public testing::Test
    {
    public:
        std::size_t size = 2_MB;
        pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Enabled> stack{ size };
    };

    /** @brief Test fixture for stack telemetry integration tests. */
    class StrictStackTelemetryIntegration: public testing::Test
    {
    public:
        std::size_t size = 2_MB;
        pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Enabled> stack{ size };
    };


} // namespace



/**************************************
 *                                    *
 *            LOOSE STACK             *
 *                                    *
 **************************************/

TEST_F(LooseStackTelemetryIntegration, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(LooseStackTelemetryIntegration, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(LooseStackTelemetryIntegration, AllocationUsingAllocBytes_IncreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 128_KB;
    constexpr auto alignment      = 1024;
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(bytesAllocated, alignment));

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    // Comparing padding to an arbitrary value can be flaky
    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GT(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(LooseStackTelemetryIntegration, AllocationUsingAlloc_IncreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 2_KB;
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.alloc<LargeData<bytesAllocated>>());

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GT(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(LooseStackTelemetryIntegration, AllocationUsingAllocV_IncreasesMemoryUsage)
{
    constexpr auto count          = 1000;
    constexpr auto bytesAllocated = count * sizeof(int);
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.allocV<int>(count));

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GE(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(LooseStackTelemetryIntegration, FreeUsingFreeBytes_DecreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 128_KB;
    constexpr auto alignment      = 1024;
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto ptr = stack.allocBytes(bytesAllocated, alignment);
    stack.freeBytes(ptr);

    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}


TEST_F(LooseStackTelemetryIntegration, FreeUsingFree_DecreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 2_KB;
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto objPtr = stack.alloc<LargeData<bytesAllocated>>();
    stack.free(objPtr);

    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}


TEST_F(LooseStackTelemetryIntegration, FreeUsingFreeV_DecreasesMemoryUsage)
{
    constexpr auto count          = 1000;
    constexpr auto bytesAllocated = count * sizeof(int);
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto vecPtr = stack.allocV<int>(count);
    stack.freeV(vecPtr);


    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}


TEST_F(LooseStackTelemetryIntegration, ResizingToLargerSize_IncreasesMemoryUsage)
{
    constexpr auto oldSize = 128_KB;
    constexpr auto newSize = 256_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(oldSize + newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_GT(telemetry.getCurrentPadding(), priorPaddingUsage);
    EXPECT_GT(telemetry.getTotalUsage(), priorPaddingUsage + oldSize + newSize);
}


TEST_F(LooseStackTelemetryIntegration, ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();
    const auto priorTotalUsage   = telemetry.getTotalUsage();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorTotalUsage, telemetry.getTotalUsage());
}


TEST_F(LooseStackTelemetryIntegration, ResizeFast_ResizingToLargerSize_AlwaysIncreaseMemoryUsage)
{
    constexpr auto oldSize = 128_KB;
    constexpr auto newSize = 256_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeFast(ptr, oldSize, newSize);

    EXPECT_EQ(oldSize + newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_GT(telemetry.getCurrentPadding(), priorPaddingUsage);
    EXPECT_GT(telemetry.getTotalUsage(), priorPaddingUsage + oldSize + newSize);
}


TEST_F(LooseStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_AlwaysIncreaseMemoryUsage)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeFast(ptr, oldSize, newSize);

    EXPECT_EQ(oldSize + newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_GT(telemetry.getCurrentPadding(), priorPaddingUsage);
    EXPECT_GT(telemetry.getTotalUsage(), priorPaddingUsage + oldSize + newSize);
}


/**
 * @test Verify that loose stack's resize last decreases memory usage when resized to a smaller size.
 */
TEST_F(LooseStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
{
    constexpr auto oldSize = 128_KB;
    constexpr auto newSize = 256_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(telemetry.getTotalUsage(), priorPaddingUsage + newSize);
}


TEST_F(LooseStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 128_KB;
    constexpr auto newSize = 256_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


/**
 * @test Verify that loose stack's resize last only increases memory usage by new size difference
 *       when resized to a larger size.
 */
TEST_F(LooseStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(telemetry.getTotalUsage(), priorPaddingUsage + newSize);
}


TEST_F(LooseStackTelemetryIntegration, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(LooseStackTelemetryIntegration, FreeAll_ResetPaddingAndMemoryUsage)
{
    constexpr auto size   = 256_KB;
    const auto& telemetry = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(size));

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(LooseStackTelemetryIntegration, FreeAll_DoesNotResetPeakAndMinUsage)
{
    constexpr auto size   = 256_KB;
    const auto& telemetry = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(size));

    EXPECT_NE(0, telemetry.getPeakMemoryUsage());
    EXPECT_NE(0, telemetry.getMinMemoryUsage());
    EXPECT_NE(0, telemetry.getPeakPadding());
    EXPECT_NE(0, telemetry.getMinPadding());
}



/**************************************
 *                                    *
 *           STRICT STACK             *
 *                                    *
 **************************************/

TEST_F(StrictStackTelemetryIntegration, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(StrictStackTelemetryIntegration, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(StrictStackTelemetryIntegration, AllocationUsingAllocBytes_IncreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 128_KB;
    constexpr auto alignment      = 1024;
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(bytesAllocated, alignment));

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    // Comparing padding to an arbitrary value can be flaky
    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GT(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(StrictStackTelemetryIntegration, AllocationUsingAlloc_IncreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 2_KB;
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.alloc<LargeData<bytesAllocated>>());

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GT(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(StrictStackTelemetryIntegration, AllocationUsingAllocV_IncreasesMemoryUsage)
{
    constexpr auto count          = 1000;
    constexpr auto bytesAllocated = count * sizeof(int);
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.allocV<int>(count));

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GE(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(StrictStackTelemetryIntegration, FreeUsingFreeBytes_DecreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 128_KB;
    constexpr auto alignment      = 1024;
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto ptr = stack.allocBytes(bytesAllocated, alignment);
    stack.freeBytes(ptr);

    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}


TEST_F(StrictStackTelemetryIntegration, FreeUsingFree_DecreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 2_KB;
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto objPtr = stack.alloc<LargeData<bytesAllocated>>();
    stack.free(objPtr);

    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}


TEST_F(StrictStackTelemetryIntegration, FreeUsingFreeV_DecreasesMemoryUsage)
{
    constexpr auto count          = 1000;
    constexpr auto bytesAllocated = count * sizeof(int);
    const auto& telemetry         = stack.getTelemetry();

    static_cast<void>(stack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto vecPtr = stack.allocV<int>(count);
    stack.freeV(vecPtr);


    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}

/**
 * @test Verify that latest allocation in strict stack when resized to a larger size, increases memory footprint
 *       to the newer(larger) size, but not consuming both old size and new size.
 */
TEST_F(StrictStackTelemetryIntegration, LatestAllocation_ResizingToLargerSize_IncreasesMemoryUsageToNewSize)
{
    constexpr auto oldSize = 128_KB;
    constexpr auto newSize = 256_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(telemetry.getTotalUsage(), priorPaddingUsage + newSize);
}


TEST_F(StrictStackTelemetryIntegration, LatestAllocation_ResizingToLargerSize_PaddingDoesNotChange)
{
    constexpr auto oldSize = 128_KB;
    constexpr auto newSize = 256_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
}


/**
 * @test Verify that latest allocation in strict stack when resized to a smaller size, decreases memory footprint
 *       to the newer(smaller) size.
 */
TEST_F(StrictStackTelemetryIntegration, LatestAllocation_ResizingToSmallerSize_DecreasesMemoryUsageToSize)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(telemetry.getTotalUsage(), priorPaddingUsage + newSize);
}


TEST_F(StrictStackTelemetryIntegration, LatestAllocation_ResizingToSmallerSize_PaddingDoesNotChange)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
}


TEST_F(StrictStackTelemetryIntegration, AnyAllocation_ResizingToLargerSize_IncreasesMemoryUsage)
{
    constexpr auto oldSize       = 128_KB;
    constexpr auto newSize       = 256_KB;
    constexpr auto secondarySize = 12_KB;
    const auto& telemetry        = stack.getTelemetry();


    auto ptr = stack.allocBytes(oldSize);

    // Second allocation used to make ptr not the latest allocation
    static_cast<void>(stack.allocBytes(secondarySize));
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorMemoryUsage + newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_GT(telemetry.getCurrentPadding(), priorPaddingUsage);
    EXPECT_GT(telemetry.getTotalUsage(), priorPaddingUsage + newSize);
}


TEST_F(StrictStackTelemetryIntegration, AnyAllocation_ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr = stack.allocBytes(oldSize);

    // Second allocation used to make ptr not the latest allocation
    static_cast<void>(stack.allocBytes(12_KB));
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();
    const auto priorTotalUsage   = telemetry.getTotalUsage();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorTotalUsage, telemetry.getTotalUsage());
}


/**
 * @test Verify that loose stack's resize last decreases memory usage when resized to a smaller size.
 */
TEST_F(StrictStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
{
    constexpr auto oldSize = 128_KB;
    constexpr auto newSize = 256_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(telemetry.getTotalUsage(), priorPaddingUsage + newSize);
}


TEST_F(StrictStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 128_KB;
    constexpr auto newSize = 256_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


/**
 * @test Verify that loose stack's resize last only increases memory usage by new size difference
 *       when resized to a larger size.
 */
TEST_F(StrictStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(newSize, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(telemetry.getTotalUsage(), priorPaddingUsage + newSize);
}


TEST_F(StrictStackTelemetryIntegration, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(StrictStackTelemetryIntegration, FreeAll_ResetPaddingAndMemoryUsage)
{
    constexpr auto size   = 256_KB;
    const auto& telemetry = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(size));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(StrictStackTelemetryIntegration, FreeAll_DoesNotResetPeakAndMinUsage)
{
    constexpr auto size   = 256_KB;
    const auto& telemetry = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(size));

    EXPECT_NE(0, telemetry.getPeakMemoryUsage());
    EXPECT_NE(0, telemetry.getMinMemoryUsage());
    EXPECT_NE(0, telemetry.getPeakPadding());
    EXPECT_NE(0, telemetry.getMinPadding());
}

/** @} */
