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
          _minMemoryUsage(std::numeric_limits<std::size_t>::max()),
          _peakMemoryUsage(0),
          _totalPadding(0),
          _minPaddingUsage(std::numeric_limits<std::size_t>::max()),
          _peakPaddingUsage(0),
          _size(size)
    {}


    PMM_INLINE constexpr void StackTelemetry::incStackUsage(const std::size_t size, const std::size_t padding) noexcept
    {
        _currentUsage += size;
        updateMinMemoryUsage(size);
        updatePeakMemoryUsage(size);

        _totalPadding += padding;
        updateMinPaddingUsage(padding);
        updatePeakPaddingUsage(padding);
    }

    PMM_INLINE constexpr void StackTelemetry::decStackUsage(const std::size_t size, const std::size_t padding) noexcept
    {
        _currentUsage -= size;
        _totalPadding -= padding;
    }

    PMM_INLINE constexpr void StackTelemetry::updateMinMemoryUsage(const std::size_t usage) noexcept
    { _minMemoryUsage = std::min(_minMemoryUsage, usage); }


    PMM_INLINE constexpr void StackTelemetry::updatePeakMemoryUsage(const std::size_t usage) noexcept
    { _peakMemoryUsage = std::max(_peakMemoryUsage, usage); }


    PMM_INLINE constexpr void StackTelemetry::updateMinPaddingUsage(const std::size_t usage) noexcept
    { _minPaddingUsage = std::min(_minPaddingUsage, usage); }


    PMM_INLINE constexpr void StackTelemetry::updatePeakPaddingUsage(const std::size_t usage) noexcept
    { _peakPaddingUsage = std::max(_peakPaddingUsage, usage); }


    PMM_INLINE constexpr void StackTelemetry::resetCurrentUsage() noexcept
    {
        _currentUsage = 0;
        _totalPadding      = 0;
    }


    PMM_INLINE constexpr void StackTelemetry::resetTelemetry() noexcept
    {
        _currentUsage = 0;
        _minMemoryUsage     = std::numeric_limits<std::size_t>::max();
        _peakMemoryUsage    = 0;

        _totalPadding          = 0;
        _minPaddingUsage  = std::numeric_limits<std::size_t>::max();
        _peakPaddingUsage = 0;
    }



    /**************************************
     *                                    *
     *              GETTERS               *
     *                                    *
     **************************************/

    PMM_INLINE constexpr std::size_t StackTelemetry::getStackSize() const noexcept { return _size; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getCurrentMemoryUsage() const noexcept { return _currentUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getMinMemoryUsage() const noexcept { return _minMemoryUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getPeakMemoryUsage() const noexcept { return _peakMemoryUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getCurrentPadding() const noexcept { return _totalPadding; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getPeakPadding() const noexcept { return _peakPaddingUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getMinPadding() const noexcept { return _minPaddingUsage; }

    PMM_INLINE constexpr std::size_t StackTelemetry::getTotalUsage() const noexcept { return _totalPadding + _currentUsage; }

} // namespace pmm
