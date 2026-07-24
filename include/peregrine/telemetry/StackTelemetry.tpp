#pragma once
/**
 * @file StackTelemetry.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 24, 2026
 *
 * @brief Arena telemetry implementation of member functions defined in StackTelemetry.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <algorithm>
#include <limits>

namespace pmm
{
    constexpr StackTelemetry::StackTelemetry(const std::size_t size) noexcept
        : _currentUsage(0), _peakUsage(0), _minUsage(std::numeric_limits<std::size_t>::max()), _size(size)
    {}


    constexpr void StackTelemetry::updateAllocationUsage(const std::size_t allocatedByteSize) noexcept
    {
        _currentUsage += allocatedByteSize;
        updateMinUsage(allocatedByteSize);
        updatePeakUsage(allocatedByteSize);
    }

    constexpr void StackTelemetry::updateMinUsage(std::size_t usage) noexcept
    { _minUsage = std::min(_minUsage, usage); }


    constexpr void StackTelemetry::updatePeakUsage(std::size_t usage) noexcept
    { _peakUsage = std::max(_peakUsage, usage); }


    constexpr void StackTelemetry::resetCurrentUsage() noexcept { _currentUsage = 0; }


    constexpr void StackTelemetry::resetTelemetry() noexcept
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

    constexpr std::size_t StackTelemetry::getArenaSize() const noexcept { return _size; }

    constexpr std::size_t StackTelemetry::getCurrentUsage() const noexcept { return _currentUsage; }

    constexpr std::size_t StackTelemetry::getMinUsage() const noexcept { return _minUsage; }

    constexpr std::size_t StackTelemetry::getPeakUsage() const noexcept { return _peakUsage; }

    constexpr std::size_t StackTelemetry::getPadding() const noexcept { return _padding; }

} // namespace pmm
