#pragma once
/**
 * @file TLSFTelemetry.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 22, 2026
 *
 * @brief Implementation of member functions declared in TLSFTelemetry.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "peregrine/utils/Preprocessors.h"

#include <algorithm>
#include <limits>

namespace pmm
{

    PMM_INLINE constexpr TLSFTelemetry::TLSFTelemetry(const size_t size) noexcept
        : _totalMemoryUsage(0),
          _minTotalMemoryUsage(std::numeric_limits<size_t>::max()),
          _peakTotalMemoryUsage(0),
          _metadataUsage(0),
          _minMetadataUsage(std::numeric_limits<size_t>::max()),
          _peakMetadataUsage(0),
          _size(size)
    {}


    PMM_INLINE constexpr void TLSFTelemetry::incUsage(const size_t reqSize, const size_t allocatedSize) noexcept
    {
        _totalMemoryUsage += allocatedSize;
        updateMinMemoryUsage(_totalMemoryUsage);
        updatePeakMemoryUsage(_totalMemoryUsage);

        const auto metaSize = allocatedSize - reqSize;
        _metadataUsage += metaSize;
        updateMinMetadataUsage(metaSize);
        updatePeakMetadataUsage(metaSize);
    }

    PMM_INLINE constexpr void TLSFTelemetry::decUsage(const size_t reqSize, const size_t allocatedSize) noexcept
    {
        _totalMemoryUsage -= allocatedSize;
        _metadataUsage -= allocatedSize - reqSize;
    }

    PMM_INLINE constexpr void TLSFTelemetry::updateMinMemoryUsage(const size_t usage) noexcept
    { _minTotalMemoryUsage = std::min(_minTotalMemoryUsage, usage); }


    PMM_INLINE constexpr void TLSFTelemetry::updatePeakMemoryUsage(const size_t usage) noexcept
    { _peakTotalMemoryUsage = std::max(_peakTotalMemoryUsage, usage); }


    PMM_INLINE constexpr void TLSFTelemetry::updateMinMetadataUsage(const size_t usage) noexcept
    { _minMetadataUsage = std::min(_minMetadataUsage, usage); }


    PMM_INLINE constexpr void TLSFTelemetry::updatePeakMetadataUsage(const size_t usage) noexcept
    { _peakMetadataUsage = std::max(_peakMetadataUsage, usage); }


    PMM_INLINE constexpr void TLSFTelemetry::resetCurrentUsage() noexcept
    {
        _totalMemoryUsage = 0;
        _metadataUsage    = 0;
    }


    PMM_INLINE constexpr void TLSFTelemetry::resetTelemetry() noexcept
    {
        _totalMemoryUsage     = 0;
        _minTotalMemoryUsage  = std::numeric_limits<size_t>::max();
        _peakTotalMemoryUsage = 0;

        _metadataUsage     = 0;
        _minMetadataUsage  = std::numeric_limits<size_t>::max();
        _peakMetadataUsage = 0;
    }



    /**************************************
     *                                    *
     *              GETTERS               *
     *                                    *
     **************************************/

    PMM_INLINE constexpr size_t TLSFTelemetry::getTLSFSize() const noexcept { return _size; }

    // TODO: Update naming terminology
    PMM_INLINE constexpr size_t TLSFTelemetry::getCurrentMemoryUsage() const noexcept
    { return _totalMemoryUsage - _metadataUsage; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getMinMemoryUsage() const noexcept { return _minTotalMemoryUsage; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getPeakMemoryUsage() const noexcept { return _peakTotalMemoryUsage; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getMetadataUsage() const noexcept { return _metadataUsage; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getPeakMetadataUsage() const noexcept { return _peakMetadataUsage; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getMinMetadataUsage() const noexcept { return _minMetadataUsage; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getTotalUsage() const noexcept { return _totalMemoryUsage; }

} // namespace pmm
