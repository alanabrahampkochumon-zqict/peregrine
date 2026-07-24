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


#include "peregrine/utils/Preprocessors.h"

#include <algorithm>
#include <limits>



namespace pmm
{

#ifdef ENABLE_PMM_TELEMETRY

    PMM_INLINE constexpr ArenaTelemetry::ArenaTelemetry(const std::size_t size) noexcept
        : _currentUsage(0), _peakUsage(0), _minUsage(std::numeric_limits<std::size_t>::max()), _size(size)
    {}


    PMM_INLINE constexpr void ArenaTelemetry::updateAllocationUsage(const std::size_t allocatedByteSize) noexcept
    {
        _currentUsage += allocatedByteSize;
        updateMinUsage(allocatedByteSize);
        updatePeakUsage(allocatedByteSize);
    }

    PMM_INLINE constexpr void ArenaTelemetry::updateMinUsage(std::size_t usage) noexcept
    { _minUsage = std::min(_minUsage, usage); }


    PMM_INLINE constexpr void ArenaTelemetry::updatePeakUsage(std::size_t usage) noexcept
    { _peakUsage = std::max(_peakUsage, usage); }


    PMM_INLINE constexpr void ArenaTelemetry::resetCurrentUsage() noexcept { _currentUsage = 0; }


    PMM_INLINE constexpr void ArenaTelemetry::resetTelemetry() noexcept
    {
        _currentUsage = 0;
        _minUsage     = std::numeric_limits<std::size_t>::max();
        _peakUsage    = 0;
    }



    /**************************************
     *                                    *
     *              GETTERS               *
     *                                    *
     **************************************/

    PMM_INLINE constexpr std::size_t ArenaTelemetry::getArenaSize() const noexcept { return _size; }

    PMM_INLINE constexpr std::size_t ArenaTelemetry::getCurrentUsage() const noexcept { return _currentUsage; }

    PMM_INLINE constexpr std::size_t ArenaTelemetry::getMinUsage() const noexcept { return _minUsage; }

    PMM_INLINE constexpr std::size_t ArenaTelemetry::getPeakUsage() const noexcept { return _peakUsage; }

#endif
} // namespace pmm
