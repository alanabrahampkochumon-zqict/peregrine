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


    /** @brief Test fixture for managed loose stack telemetry integration tests. */
    class ManagedLooseStackTelemetryIntegration: public testing::Test
    {
    public:
        std::size_t size = 2_MB;
        pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Enabled> stack{ size };
    };


    /** @brief Test fixture for managed strict stack telemetry integration tests. */
    class ManagedStrictStackTelemetryIntegration: public testing::Test
    {
    public:
        std::size_t size = 2_MB;
        pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Enabled> stack{ size };
    };


    /** @brief Test fixture for unmanaged loose stack telemetry integration tests. */
    class UnmanagedLooseStackTelemetryIntegration: public testing::Test
    {
    public:
        static constexpr std::size_t size = 2_MB;
        uint8_t* buffer                   = new uint8_t[size];
        pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory, pmm::telemetry::Enabled> stack{ size, buffer };

        void TearDown() override { delete[] buffer; }
    };


    /** @brief Test fixture for unmanaged strict stack telemetry integration tests. */
    class UnmanagedStrictStackTelemetryIntegration: public testing::Test
    {
    public:
        static constexpr std::size_t size = 2_MB;
        uint8_t* buffer                   = new uint8_t[size];
        pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory, pmm::telemetry::Enabled> stack{ size, buffer };

        void TearDown() override { delete[] buffer; }
    };

} // namespace



/**************************************
 *         MANGED LOOSE STACK         *
 **************************************/

TEST_F(ManagedLooseStackTelemetryIntegration, TelemetryEnabled_IsTelemetryEnabled_ReturnsTrue)
{ EXPECT_TRUE(stack.isTelemetryEnabled()); }


TEST_F(ManagedLooseStackTelemetryIntegration, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(ManagedLooseStackTelemetryIntegration, TelemetryDisabled_IsTelemetryEnabled_ReturnsFalse)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    EXPECT_FALSE(noTelemetryStack.isTelemetryEnabled());
}


TEST_F(ManagedLooseStackTelemetryIntegration, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(ManagedLooseStackTelemetryIntegration, AllocationUsingAllocBytes_IncreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, AllocationUsingAlloc_IncreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, AllocationUsingAllocV_IncreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, FreeUsingFreeBytes_DecreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, FreeUsingFree_DecreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, FreeUsingFreeV_DecreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, ResizingToLargerSize_IncreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, ResizeFast_ResizingToLargerSize_AlwaysIncreaseMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_AlwaysIncreaseMemoryUsage)
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
TEST_F(ManagedLooseStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(ManagedLooseStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
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
TEST_F(ManagedLooseStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(ManagedLooseStackTelemetryIntegration, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(ManagedLooseStackTelemetryIntegration, Clear_ResetPaddingAndMemoryUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(ManagedLooseStackTelemetryIntegration, Clear_DoesNotResetPeakAndMinUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));

    EXPECT_NE(0, telemetry.getPeakMemoryUsage());
    EXPECT_NE(0, telemetry.getMinMemoryUsage());
    EXPECT_NE(0, telemetry.getPeakPadding());
    EXPECT_NE(0, telemetry.getMinPadding());
}



/**************************************
 *        MANAGED STRICT STACK        *
 **************************************/

TEST_F(ManagedStrictStackTelemetryIntegration, TelemetryEnabled_IsTelemetryEnabled_ReturnsTrue)
{ EXPECT_TRUE(stack.isTelemetryEnabled()); }


TEST_F(ManagedStrictStackTelemetryIntegration, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(ManagedStrictStackTelemetryIntegration, TelemetryDisabled_IsTelemetryEnabled_ReturnsFalse)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    EXPECT_FALSE(noTelemetryStack.isTelemetryEnabled());
}


TEST_F(ManagedStrictStackTelemetryIntegration, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(ManagedStrictStackTelemetryIntegration, AllocationUsingAllocBytes_IncreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegration, AllocationUsingAlloc_IncreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegration, AllocationUsingAllocV_IncreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegration, FreeUsingFreeBytes_DecreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegration, FreeUsingFree_DecreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegration, FreeUsingFreeV_DecreasesMemoryUsage)
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
TEST_F(ManagedStrictStackTelemetryIntegration, LatestAllocation_ResizingToLargerSize_IncreasesMemoryUsageToNewSize)
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


TEST_F(ManagedStrictStackTelemetryIntegration, LatestAllocation_ResizingToLargerSize_PaddingDoesNotChange)
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
TEST_F(ManagedStrictStackTelemetryIntegration, LatestAllocation_ResizingToSmallerSize_DecreasesMemoryUsageToSize)
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


TEST_F(ManagedStrictStackTelemetryIntegration, LatestAllocation_ResizingToSmallerSize_PaddingDoesNotChange)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
}


TEST_F(ManagedStrictStackTelemetryIntegration, AnyAllocation_ResizingToLargerSize_IncreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegration, AnyAllocation_ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
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
TEST_F(ManagedStrictStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(ManagedStrictStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
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
TEST_F(ManagedStrictStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(ManagedStrictStackTelemetryIntegration, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(ManagedStrictStackTelemetryIntegration, Clear_ResetPaddingAndMemoryUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(ManagedStrictStackTelemetryIntegration, Clear_DoesNotResetPeakAndMinUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));

    EXPECT_NE(0, telemetry.getPeakMemoryUsage());
    EXPECT_NE(0, telemetry.getMinMemoryUsage());
    EXPECT_NE(0, telemetry.getPeakPadding());
    EXPECT_NE(0, telemetry.getMinPadding());
}



/**************************************
 *       UN-MANGED LOOSE STACK        *
 **************************************/

TEST_F(UnmanagedLooseStackTelemetryIntegration, TelemetryEnabled_IsTelemetryEnabled_ReturnsTrue)
{ EXPECT_TRUE(stack.isTelemetryEnabled()); }


TEST_F(UnmanagedLooseStackTelemetryIntegration, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(UnmanagedLooseStackTelemetryIntegration, TelemetryDisabled_IsTelemetryEnabled_ReturnsFalse)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    EXPECT_FALSE(noTelemetryStack.isTelemetryEnabled());
}


TEST_F(UnmanagedLooseStackTelemetryIntegration, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(UnmanagedLooseStackTelemetryIntegration, AllocationUsingAllocBytes_IncreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, AllocationUsingAlloc_IncreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, AllocationUsingAllocV_IncreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, FreeUsingFreeBytes_DecreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, FreeUsingFree_DecreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, FreeUsingFreeV_DecreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, ResizingToLargerSize_IncreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, ResizeFast_ResizingToLargerSize_AlwaysIncreaseMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_AlwaysIncreaseMemoryUsage)
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
TEST_F(UnmanagedLooseStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
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
TEST_F(UnmanagedLooseStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedLooseStackTelemetryIntegration, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(UnmanagedLooseStackTelemetryIntegration, Clear_ResetPaddingAndMemoryUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(UnmanagedLooseStackTelemetryIntegration, Clear_DoesNotResetPeakAndMinUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));

    EXPECT_NE(0, telemetry.getPeakMemoryUsage());
    EXPECT_NE(0, telemetry.getMinMemoryUsage());
    EXPECT_NE(0, telemetry.getPeakPadding());
    EXPECT_NE(0, telemetry.getMinPadding());
}



/**************************************
 *      UN-MANAGED STRICT STACK       *
 **************************************/

TEST_F(UnmanagedStrictStackTelemetryIntegration, TelemetryEnabled_IsTelemetryEnabled_ReturnsTrue)
{ EXPECT_TRUE(stack.isTelemetryEnabled()); }


TEST_F(UnmanagedStrictStackTelemetryIntegration, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(UnmanagedStrictStackTelemetryIntegration, TelemetryDisabled_IsTelemetryEnabled_ReturnsFalse)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    EXPECT_FALSE(noTelemetryStack.isTelemetryEnabled());
}


TEST_F(UnmanagedStrictStackTelemetryIntegration, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(UnmanagedStrictStackTelemetryIntegration, AllocationUsingAllocBytes_IncreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, AllocationUsingAlloc_IncreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, AllocationUsingAllocV_IncreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, FreeUsingFreeBytes_DecreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, FreeUsingFree_DecreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, FreeUsingFreeV_DecreasesMemoryUsage)
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
TEST_F(UnmanagedStrictStackTelemetryIntegration, LatestAllocation_ResizingToLargerSize_IncreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, LatestAllocation_ResizingToLargerSize_PaddingDoesNotChange)
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
TEST_F(UnmanagedStrictStackTelemetryIntegration, LatestAllocation_ResizingToSmallerSize_DecreasesMemoryUsageToSize)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, LatestAllocation_ResizingToSmallerSize_PaddingDoesNotChange)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
}


TEST_F(UnmanagedStrictStackTelemetryIntegration, AnyAllocation_ResizingToLargerSize_IncreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, AnyAllocation_ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
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
TEST_F(UnmanagedStrictStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
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
TEST_F(UnmanagedStrictStackTelemetryIntegration, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedStrictStackTelemetryIntegration, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(UnmanagedStrictStackTelemetryIntegration, Clear_ResetPaddingAndMemoryUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(UnmanagedStrictStackTelemetryIntegration, Clear_DoesNotResetPeakAndMinUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));

    EXPECT_NE(0, telemetry.getPeakMemoryUsage());
    EXPECT_NE(0, telemetry.getMinMemoryUsage());
    EXPECT_NE(0, telemetry.getPeakPadding());
    EXPECT_NE(0, telemetry.getMinPadding());
}


/** @} */
