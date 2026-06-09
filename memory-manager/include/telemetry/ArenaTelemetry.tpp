#pragma once
/**
 * @file ArenaTelemetry.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 30, 2026
 *
 * @brief Arena telemetry implementation of member functions defined in ArenaTelemetry.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <limits>
#include <algorithm>


namespace pmm
{

#ifdef ENABLE_PMM_TELEMETRY

    constexpr ArenaTelemetry::ArenaTelemetry(const std::size_t size) noexcept
        : currentUsage(0), peakUsage(0), minUsage(std::numeric_limits<std::size_t>::max()), size(size)
    {}


    constexpr void ArenaTelemetry::updateAllocationUsage(const std::size_t allocatedByteSize) noexcept
    {
        currentUsage += allocatedByteSize;
        minUsage = std::min(minUsage, allocatedByteSize);
        peakUsage = std::max(peakUsage, allocatedByteSize);
    }


    constexpr void ArenaTelemetry::resetCurrentUsage() noexcept
    {
        currentUsage = 0;
    }


    constexpr void ArenaTelemetry::resetTelemetry() noexcept
    {
        currentUsage = 0;
        minUsage = std::numeric_limits<std::size_t>::max();
        peakUsage = 0;
    }

#endif
} // namespace pmm