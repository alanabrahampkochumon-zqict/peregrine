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



namespace pmm
{
    #define ENABLE_PMM_TELEMETRY // TODO: Remove
#ifdef ENABLE_PMM_TELEMETRY
    constexpr ArenaTelemetry::ArenaTelemetry(std::size_t size) noexcept
        : size(size), currentUsage(0), minUsage(0), peakUsage(0)
    {}


    constexpr void ArenaTelemetry::updateAllocationUsage(std::size_t allocatedByteSize) noexcept
    {
        currentUsage += allocatedByteSize;
        minUsage = std::min(minUsage, allocatedByteSize);
        peakUsage = std::max(peakUsage, allocatedByteSize);
    }


    constexpr void ArenaTelemetry::resetTelemetry() noexcept
    {
        currentUsage = 0;
        minUsage = 0;
        peakUsage = 0;
    }
#endif
} // namespace pmm