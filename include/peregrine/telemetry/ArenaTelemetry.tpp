#pragma once
/**
 * @file ArenaTelemetry.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 30, 2026
 *
 * @brief Implementation of member functions declared in ArenaTelemetry.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "peregrine/utils/Preprocessors.h"

#include <algorithm>
#include <limits>


namespace pmm
{
    PMM_INLINE constexpr ArenaTelemetry::ArenaTelemetry(const size_t size) noexcept
        : _minUsage{ std::numeric_limits<size_t>::max() }, _arenaSize{ size }
    {}


    PMM_INLINE constexpr void ArenaTelemetry::logAllocationUsage(const size_t allocatedByteSize) noexcept
    {
        _currentUsage += allocatedByteSize;
        logMinUsage(allocatedByteSize);
        logPeakUsage(allocatedByteSize);
    }


    PMM_INLINE constexpr void ArenaTelemetry::logPaddingUsage(const size_t padding) noexcept
    { _totalPadding += padding; }


    PMM_INLINE constexpr void ArenaTelemetry::logMinUsage(const size_t usage) noexcept
    { _minUsage = std::min(_minUsage, usage); }


    PMM_INLINE constexpr void ArenaTelemetry::logPeakUsage(const size_t usage) noexcept
    { _peakUsage = std::max(_peakUsage, usage); }


    PMM_INLINE constexpr void ArenaTelemetry::resetCurrentUsage() noexcept
    {
        _currentUsage = 0;
        _totalPadding = 0;
    }


    PMM_INLINE constexpr void ArenaTelemetry::resetTelemetry() noexcept
    {
        _currentUsage = 0;
        _minUsage     = std::numeric_limits<size_t>::max();
        _peakUsage    = 0;
        _totalPadding = 0;
    }


    /**************************************
     *              GETTERS               *
     **************************************/

    PMM_INLINE constexpr size_t ArenaTelemetry::getArenaSize() const noexcept { return _arenaSize; }

    PMM_INLINE constexpr size_t ArenaTelemetry::getFreeSize() const noexcept { return _arenaSize - _currentUsage; }

    PMM_INLINE constexpr size_t ArenaTelemetry::getUsedSize() const noexcept { return _currentUsage; }

    PMM_INLINE constexpr size_t ArenaTelemetry::getMinUsage() const noexcept { return _minUsage; }

    PMM_INLINE constexpr size_t ArenaTelemetry::getPeakUsage() const noexcept { return _peakUsage; }

    PMM_INLINE constexpr size_t ArenaTelemetry::getTotalPadding() const noexcept { return _totalPadding; }

} // namespace pmm
