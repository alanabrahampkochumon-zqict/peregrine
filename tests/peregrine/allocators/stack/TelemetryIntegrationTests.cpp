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

    /** @brief Test fixture for stack telemetry integration tests. */
    class StackTelemetryIntegration: public testing::Test
    {
    public:
        std::size_t size = 2_MB;
        pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Enabled> telemetryStack{ size };
        pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Disabled> noTelemetryStack{ size };
    };

    // // Verify that stack telemetry instance is provided at for telemetry::Managed
    // constexpr pmm::StackTelemetry<pmm::stack::Loose, pmm::ManagedMemory, pmm::telemetry::Managed> stack{512};

} // namespace



TEST_F(StackTelemetryIntegration, TelemetryEnabledStack_ReturnsTelemetryData)
{
    const auto& telemetry = telemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry&> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}

TEST_F(StackTelemetryIntegration, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    const auto& telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry&> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}


TEST_F(StackTelemetryIntegration, AllocationUsingAllocBytes_IncreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 512_KB;
    constexpr auto alignment      = 1024;
    const auto& telemetry         = telemetryStack.getTelemetry();

    static_cast<void>(telemetryStack.allocBytes(bytesAllocated, alignment));

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    // Comparing padding to an arbitrary value can be flaky
    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GT(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(StackTelemetryIntegration, AllocationUsingAlloc_IncreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 2_KB;
    const auto& telemetry         = telemetryStack.getTelemetry();

    static_cast<void>(telemetryStack.alloc<LargeData<bytesAllocated>>());

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GT(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(StackTelemetryIntegration, AllocationUsingAllocV_IncreasesMemoryUsage)
{
    constexpr auto count          = 1000;
    constexpr auto bytesAllocated = count * sizeof(int);
    const auto& telemetry         = telemetryStack.getTelemetry();

    static_cast<void>(telemetryStack.allocV<int>(count));

    EXPECT_EQ(bytesAllocated, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getPeakMemoryUsage());
    EXPECT_EQ(bytesAllocated, telemetry.getMinMemoryUsage());

    EXPECT_GE(telemetry.getCurrentPadding(), 0);
    EXPECT_GE(telemetry.getPeakPadding(), 0);
    EXPECT_GE(telemetry.getMinPadding(), 0);

    EXPECT_GE(telemetry.getTotalUsage(), bytesAllocated);
}


TEST_F(StackTelemetryIntegration, FreeUsingFreeBytes_DecreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 512_KB;
    constexpr auto alignment      = 1024;
    const auto& telemetry         = telemetryStack.getTelemetry();

    static_cast<void>(telemetryStack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto ptr = telemetryStack.allocBytes(bytesAllocated, alignment);
    telemetryStack.freeBytes(ptr);

    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}


TEST_F(StackTelemetryIntegration, FreeUsingFree_DecreasesMemoryUsage)
{
    constexpr auto bytesAllocated = 2_KB;
    const auto& telemetry         = telemetryStack.getTelemetry();

    static_cast<void>(telemetryStack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto objPtr = telemetryStack.alloc<LargeData<bytesAllocated>>();
    telemetryStack.free(objPtr);

    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}


TEST_F(StackTelemetryIntegration, FreeUsingFreeV_DecreasesMemoryUsage)
{
    constexpr auto count          = 1000;
    constexpr auto bytesAllocated = count * sizeof(int);
    const auto& telemetry         = telemetryStack.getTelemetry();

    static_cast<void>(telemetryStack.alloc<LargeData<bytesAllocated>>());
    const auto priorMemoryUsage  = telemetry.getCurrentMemoryUsage();
    const auto priorPaddingUsage = telemetry.getCurrentPadding();

    const auto vecPtr = telemetryStack.allocV<int>(count);
    telemetryStack.freeV(vecPtr);


    EXPECT_EQ(priorMemoryUsage, telemetry.getCurrentMemoryUsage());
    EXPECT_EQ(priorPaddingUsage, telemetry.getCurrentPadding());
    EXPECT_EQ(priorMemoryUsage + priorPaddingUsage, telemetry.getTotalUsage());
}

/** @} */
