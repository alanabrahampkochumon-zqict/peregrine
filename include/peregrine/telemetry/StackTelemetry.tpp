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


#include "peregrine/utils/Preprocessors.h"

#include <algorithm>
#include <limits>

namespace pmm
{
    PMM_INLINE constexpr StackTelemetry::StackTelemetry(const std::size_t size) noexcept
        : _currentUsage(0),
          _peakUsage(0),
          _padding(0),
          _totalPadding(0),
          _peakPaddingUsage(0),
          _minPaddingUsage(std::numeric_limits<std::size_t>::max()),
          _minUsage(std::numeric_limits<std::size_t>::max()),
          _size(size)
    {}


    PMM_INLINE constexpr void StackTelemetry::incStackUsage(const std::size_t size, const std::size_t padding) noexcept
    {
        _currentUsage += size;
        updateMinUsage(size);
        updatePeakUsage(size);

        _padding += padding;
        updateMinPaddingUsage(padding);
        updatePeakPaddingUsage(padding);
    }

    PMM_INLINE constexpr void StackTelemetry::decStackUsage(const std::size_t size, const std::size_t padding) noexcept
    {
        _currentUsage -= size;
        _padding -= padding;
    }

    PMM_INLINE constexpr void StackTelemetry::updateMinUsage(const std::size_t usage) noexcept
    { _minUsage = std::min(_minUsage, usage); }


    PMM_INLINE constexpr void StackTelemetry::updatePeakUsage(const std::size_t usage) noexcept
    { _peakUsage = std::max(_peakUsage, usage); }


    PMM_INLINE constexpr void StackTelemetry::updateMinPaddingUsage(const std::size_t usage) noexcept
    { _minPaddingUsage = std::min(_minPaddingUsage, usage); }

    PMM_INLINE constexpr void StackTelemetry::updatePeakPaddingUsage(const std::size_t usage) noexcept
    { _peakPaddingUsage = std::max(_peakPaddingUsage, usage); }

    PMM_INLINE constexpr void StackTelemetry::resetCurrentUsage() noexcept
    {
        _currentUsage = 0;
        _padding      = 0;
    }

    PMM_INLINE constexpr void StackTelemetry::resetTelemetry() noexcept
    {
        _currentUsage = 0;
        _minUsage     = std::numeric_limits<std::size_t>::max();
        _peakUsage    = 0;

        _padding          = 0;
        _minPaddingUsage  = std::numeric_limits<std::size_t>::max();
        _peakPaddingUsage = 0;
    }



    /**************************************
     *                                    *
     *              GETTERS               *
     *                                    *
     **************************************/

    PMM_INLINE constexpr std::size_t StackTelemetry::getStackSize() const noexcept { return _size; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getCurrentUsage() const noexcept { return _currentUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getMinUsage() const noexcept { return _minUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getPeakUsage() const noexcept { return _peakUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getTotalPadding() const noexcept { return _padding; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getPeakPaddingUsage() const noexcept { return _minPaddingUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getMinPaddingUsage() const noexcept { return _peakPaddingUsage; }

} // namespace pmm
