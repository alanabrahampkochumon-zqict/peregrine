/**
 * @file TelemetryIntegrationTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 24, 2026
 *
 * @brief Verifies that stack telemetry integration based on different policies.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


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
    const auto telemetry = telemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::StackTelemetry> == true);
    EXPECT_EQ(telemetry.getStackSize(), size);
}


TEST_F(StackTelemetryIntegration, TelemetryDisabledStack_ReturnsZeroForTelemetryData)
{
    const auto telemetry = noTelemetryStack.getTelemetry();
    static_assert(std::is_same_v<decltype(telemetry), const pmm::DummyStackTelemetry> == true);
    EXPECT_EQ(0, telemetry.getStackSize());
}

/** @} */
