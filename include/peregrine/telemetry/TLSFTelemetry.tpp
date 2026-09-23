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
        : _currentBufferUsage(0),
          _minBufferUsage(std::numeric_limits<size_t>::max()),
          _peakBufferUsage(0),
          _currentMetadataUsage(0),
          _minMetadataUsage(std::numeric_limits<size_t>::max()),
          _peakMetadataUsage(0),
          _currentPayloadUsage(0),
          _minPayloadUsage(std::numeric_limits<size_t>::max()),
          _peakPayloadUsage(0),
          _allocatorSize(size),
          _largestFreeBlockSize(0),
          _freeBlockCount(0),
          _activeAllocationCount(0),
          _lifetimeAllocationCount(0),
          _lifetimeFreeCount(0)
    {}


    PMM_INLINE constexpr void TLSFTelemetry::incUsage(const size_t reqSize, const size_t overhead) noexcept
    {
        _currentPayloadUsage += reqSize;
        _minPayloadUsage  = std::min(_minPayloadUsage, reqSize);
        _peakPayloadUsage = std::max(_peakPayloadUsage, reqSize);

        _currentMetadataUsage += overhead;
        _minMetadataUsage  = std::min(_minMetadataUsage, overhead);
        _peakMetadataUsage = std::max(_peakMetadataUsage, overhead);

        const auto bufferSize = reqSize + overhead;
        _currentBufferUsage += bufferSize;
        _minBufferUsage  = std::min(_minBufferUsage, bufferSize);
        _peakBufferUsage = std::max(_peakBufferUsage, bufferSize);

        ++_activeAllocationCount;
        ++_lifetimeAllocationCount;
    }

    PMM_INLINE constexpr void TLSFTelemetry::decUsage(const size_t reqSize, const size_t overhead) noexcept
    {
        _currentPayloadUsage -= reqSize;
        _currentMetadataUsage -= overhead;
        _currentBufferUsage -= reqSize + overhead;

        ++_lifetimeFreeCount;
        --_activeAllocationCount;
    }


    PMM_INLINE constexpr void TLSFTelemetry::updateLargestFreeBlockSize(const size_t newSize) noexcept
    { _largestFreeBlockSize = newSize; }

    PMM_INLINE constexpr void TLSFTelemetry::incFreeBlockCount() noexcept { ++_freeBlockCount; }

    PMM_INLINE constexpr void TLSFTelemetry::decFreeBlockCount() noexcept { --_freeBlockCount; }

    PMM_INLINE constexpr void TLSFTelemetry::resetTelemetry() noexcept
    {
        _currentBufferUsage = 0;
        _minBufferUsage     = std::numeric_limits<size_t>::max();
        _peakBufferUsage    = 0;

        _currentMetadataUsage = 0;
        _minMetadataUsage     = std::numeric_limits<size_t>::max();
        _peakMetadataUsage    = 0;
    }



    /**************************************
     *                                    *
     *              GETTERS               *
     *                                    *
     **************************************/


    PMM_INLINE constexpr size_t TLSFTelemetry::getTotalCapacity() const noexcept { return _allocatorSize; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getCurrentBufferUsage() const noexcept { return _currentBufferUsage; }
    PMM_INLINE constexpr size_t TLSFTelemetry::getMinBufferUsage() const noexcept { return _minBufferUsage; }
    PMM_INLINE constexpr size_t TLSFTelemetry::getPeakBufferUsage() const noexcept { return _peakBufferUsage; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getCurrentPayloadUsage() const noexcept { return _currentPayloadUsage; }
    PMM_INLINE constexpr size_t TLSFTelemetry::getMinPayloadUsage() const noexcept { return _minPayloadUsage; }
    PMM_INLINE constexpr size_t TLSFTelemetry::getPeakPayloadUsage() const noexcept { return _peakPayloadUsage; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getCurrentMetadataUsage() const noexcept
    { return _currentMetadataUsage; }
    PMM_INLINE constexpr size_t TLSFTelemetry::getMinMetadataUsage() const noexcept { return _minMetadataUsage; }
    PMM_INLINE constexpr size_t TLSFTelemetry::getPeakMetadataUsage() const noexcept { return _peakMetadataUsage; }


    PMM_INLINE constexpr size_t TLSFTelemetry::getLargestFreeBlockSize() const noexcept
    { return _largestFreeBlockSize; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getFreeBlockCount() const noexcept { return _freeBlockCount; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getActiveAllocations() const noexcept { return _activeAllocationCount; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getLifetimeAllocations() const noexcept
    { return _lifetimeAllocationCount; }

    PMM_INLINE constexpr size_t TLSFTelemetry::getLifetimeFrees() const noexcept { return _lifetimeFreeCount; }

    PMM_INLINE constexpr bool TLSFTelemetry::hasMemoryLeak() const noexcept
    {
        // We can consider the memory to be leaking if the total allocations and free match
        // (meaning all addresses are freed), but there are active allocations.
        return _lifetimeAllocationCount == _lifetimeFreeCount && _activeAllocationCount > 0;
    }



} // namespace pmm
