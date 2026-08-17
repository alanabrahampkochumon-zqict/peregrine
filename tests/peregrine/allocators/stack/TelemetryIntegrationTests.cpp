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
    class ManagedLooseStackTelemetryIntegrationTests: public testing::Test
    {
    public:
        std::size_t size = 2_MB;
        pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Enabled> stack{ size };
    };


    /** @brief Test fixture for managed strict stack telemetry integration tests. */
    class ManagedStrictStackTelemetryIntegrationTests: public testing::Test
    {
    public:
        std::size_t size = 2_MB;
        pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Enabled> stack{ size };
    };


    /** @brief Test fixture for unmanaged loose stack telemetry integration tests. */
    class UnmanagedLooseStackTelemetryIntegrationTests: public testing::Test
    {
    public:
        static constexpr std::size_t size = 2_MB;
        uint8_t* buffer                   = new uint8_t[size];
        pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory, pmm::telemetry::Enabled> stack{ buffer, size };

    protected:
        void TearDown() override { delete[] buffer; }
    };


    /** @brief Test fixture for unmanaged strict stack telemetry integration tests. */
    class UnmanagedStrictStackTelemetryIntegrationTests: public testing::Test
    {
    public:
        static constexpr std::size_t size = 2_MB;
        uint8_t* buffer                   = new uint8_t[size];
        pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory, pmm::telemetry::Enabled> stack{ buffer, size };

    protected:
        void TearDown() override { delete[] buffer; }
    };

} // namespace



/**************************************
 *         MANAGED LOOSE STACK        *
 **************************************/

TEST_F(ManagedLooseStackTelemetryIntegrationTests, EnabledTelemetry_ReturnsRealTelemetry)
{
    [[maybe_unused]] auto telemetry = stack.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::StackTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(ManagedLooseStackTelemetryIntegrationTests, DisabledTelemetry_ReturnsDummyTelemetry)
{
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ size };
    [[maybe_unused]] const auto telemetry = noTelemetryStack.getTelemetry();
    const bool result                     = std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(ManagedLooseStackTelemetryIntegrationTests, TelemetryEnabled_IsTelemetryEnabled_ReturnsTrue)
{ EXPECT_TRUE(stack.isTelemetryEnabled()); }


TEST_F(ManagedLooseStackTelemetryIntegrationTests, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(ManagedLooseStackTelemetryIntegrationTests, TelemetryDisabled_IsTelemetryEnabled_ReturnsFalse)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    EXPECT_FALSE(noTelemetryStack.isTelemetryEnabled());
}


TEST_F(ManagedLooseStackTelemetryIntegrationTests, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(ManagedLooseStackTelemetryIntegrationTests, AllocationUsingAllocBytes_IncreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, AllocationUsingAlloc_IncreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, AllocationUsingAllocV_IncreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, FreeUsingFreeBytes_DecreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, FreeUsingFree_DecreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, FreeUsingFreeV_DecreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, ResizingToLargerSize_IncreasesMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, ResizeFast_ResizingToLargerSize_AlwaysIncreaseMemoryUsage)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, ResizeFast_ResizingToSmallerSize_AlwaysIncreaseMemoryUsage)
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
TEST_F(ManagedLooseStackTelemetryIntegrationTests, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
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
TEST_F(ManagedLooseStackTelemetryIntegrationTests, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(ManagedLooseStackTelemetryIntegrationTests, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(ManagedLooseStackTelemetryIntegrationTests, Clear_ResetPaddingAndMemoryUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(ManagedLooseStackTelemetryIntegrationTests, Clear_DoesNotResetPeakAndMinUsage)
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

TEST_F(ManagedStrictStackTelemetryIntegrationTests, EnabledTelemetry_ReturnsRealTelemetry)
{
    [[maybe_unused]] auto telemetry = stack.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::StackTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(ManagedStrictStackTelemetryIntegrationTests, DisabledTelemetry_ReturnsDummyTelemetry)
{
    const pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ size };
    [[maybe_unused]] const auto telemetry = noTelemetryStack.getTelemetry();
    const bool result                     = std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(ManagedStrictStackTelemetryIntegrationTests, TelemetryEnabled_IsTelemetryEnabled_ReturnsTrue)
{ EXPECT_TRUE(stack.isTelemetryEnabled()); }


TEST_F(ManagedStrictStackTelemetryIntegrationTests, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(ManagedStrictStackTelemetryIntegrationTests, TelemetryDisabled_IsTelemetryEnabled_ReturnsFalse)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    EXPECT_FALSE(noTelemetryStack.isTelemetryEnabled());
}


TEST_F(ManagedStrictStackTelemetryIntegrationTests, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    constexpr auto noTelStackSize = 2_KB;
    const pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{
        noTelStackSize
    };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(ManagedStrictStackTelemetryIntegrationTests, AllocationUsingAllocBytes_IncreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, AllocationUsingAlloc_IncreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, AllocationUsingAllocV_IncreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, FreeUsingFreeBytes_DecreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, FreeUsingFree_DecreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, FreeUsingFreeV_DecreasesMemoryUsage)
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
TEST_F(ManagedStrictStackTelemetryIntegrationTests, LatestAllocation_ResizingToLargerSize_IncreasesMemoryUsageToNewSize)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, LatestAllocation_ResizingToLargerSize_PaddingDoesNotChange)
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
TEST_F(ManagedStrictStackTelemetryIntegrationTests, LatestAllocation_ResizingToSmallerSize_DecreasesMemoryUsageToSize)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, LatestAllocation_ResizingToSmallerSize_PaddingDoesNotChange)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
}


TEST_F(ManagedStrictStackTelemetryIntegrationTests, AnyAllocation_ResizingToLargerSize_IncreasesMemoryUsage)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, AnyAllocation_ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
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
TEST_F(ManagedStrictStackTelemetryIntegrationTests, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
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
TEST_F(ManagedStrictStackTelemetryIntegrationTests, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(ManagedStrictStackTelemetryIntegrationTests, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(ManagedStrictStackTelemetryIntegrationTests, Clear_ResetPaddingAndMemoryUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(ManagedStrictStackTelemetryIntegrationTests, Clear_DoesNotResetPeakAndMinUsage)
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
 *       UN-MANAGED LOOSE STACK       *
 **************************************/

TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, EnabledTelemetry_ReturnsRealTelemetry)
{
    [[maybe_unused]] auto telemetry = stack.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::StackTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, DisabledTelemetry_ReturnsDummyTelemetry)
{
    const pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ buffer,
                                                                                                          size };
    [[maybe_unused]] const auto telemetry = noTelemetryStack.getTelemetry();
    const bool result                     = std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, TelemetryEnabled_IsTelemetryEnabled_ReturnsTrue)
{ EXPECT_TRUE(stack.isTelemetryEnabled()); }


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, TelemetryDisabled_IsTelemetryEnabled_ReturnsFalse)
{
    const pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ buffer,
                                                                                                          size };
    EXPECT_FALSE(noTelemetryStack.isTelemetryEnabled());
}


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    const pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ buffer,
                                                                                                          size };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, AllocationUsingAllocBytes_IncreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, AllocationUsingAlloc_IncreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, AllocationUsingAllocV_IncreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, FreeUsingFreeBytes_DecreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, FreeUsingFree_DecreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, FreeUsingFreeV_DecreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, ResizingToLargerSize_IncreasesMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, ResizeFast_ResizingToLargerSize_AlwaysIncreaseMemoryUsage)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, ResizeFast_ResizingToSmallerSize_AlwaysIncreaseMemoryUsage)
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
TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
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
TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, Clear_ResetPaddingAndMemoryUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(UnmanagedLooseStackTelemetryIntegrationTests, Clear_DoesNotResetPeakAndMinUsage)
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

TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, EnabledTelemetry_ReturnsRealTelemetry)
{
    [[maybe_unused]] auto telemetry = stack.getTelemetry();
    const bool result               = std::is_same_v<decltype(telemetry), pmm::StackTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, DisabledTelemetry_ReturnsDummyTelemetry)
{
    const pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ buffer,
                                                                                                           size };
    [[maybe_unused]] const auto telemetry = noTelemetryStack.getTelemetry();
    const bool result                     = std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry>;
    EXPECT_TRUE(result);
}


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, TelemetryEnabled_IsTelemetryEnabled_ReturnsTrue)
{ EXPECT_TRUE(stack.isTelemetryEnabled()); }


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = stack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, TelemetryDisabled_IsTelemetryEnabled_ReturnsFalse)
{
    const pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ buffer,
                                                                                                           size };
    EXPECT_FALSE(noTelemetryStack.isTelemetryEnabled());
}


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{

    const pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ buffer,
                                                                                                           size };
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, AllocationUsingAllocBytes_IncreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, AllocationUsingAlloc_IncreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, AllocationUsingAllocV_IncreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, FreeUsingFreeBytes_DecreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, FreeUsingFree_DecreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, FreeUsingFreeV_DecreasesMemoryUsage)
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
TEST_F(UnmanagedStrictStackTelemetryIntegrationTests,
       LatestAllocation_ResizingToLargerSize_IncreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, LatestAllocation_ResizingToLargerSize_PaddingDoesNotChange)
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
TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, LatestAllocation_ResizingToSmallerSize_DecreasesMemoryUsageToSize)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, LatestAllocation_ResizingToSmallerSize_PaddingDoesNotChange)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();

    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resize(ptr, oldSize, newSize);

    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
}


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, AnyAllocation_ResizingToLargerSize_IncreasesMemoryUsage)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, AnyAllocation_ResizingToSmallerSize_DoesNotIncreaseMemoryUsage)
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
TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, ResizeLast_ResizingToLargerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, ResizeLast_ResizingToLargerSize_DoesNotChangePadding)
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
TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, ResizeFast_ResizingToSmallerSize_DecreasesMemoryUsageToNewSize)
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


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, ResizeLast_ResizingToSmallerSize_DoesNotChangePadding)
{
    constexpr auto oldSize = 256_KB;
    constexpr auto newSize = 128_KB;
    const auto& telemetry  = stack.getTelemetry();


    auto ptr                     = stack.allocBytes(oldSize);
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    ptr = stack.resizeLast(ptr, oldSize, newSize);

    EXPECT_EQ(telemetry.getCurrentPadding(), priorPaddingUsage);
}


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, Clear_ResetPaddingAndMemoryUsage)
{
    constexpr auto allocSize = 256_KB;
    const auto& telemetry    = stack.getTelemetry();

    static_cast<void>(stack.allocBytes(allocSize));
    stack.clear();

    EXPECT_EQ(0, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(0, telemetry.getCurrentPadding());
    EXPECT_EQ(0, telemetry.getTotalUsage());
}


TEST_F(UnmanagedStrictStackTelemetryIntegrationTests, Clear_DoesNotResetPeakAndMinUsage)
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
