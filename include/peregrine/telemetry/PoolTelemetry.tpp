#pragma once
/**
 * @file PoolTelemetry.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 03, 2026
 *
 * @brief Implementation of member functions declared in PoolTelemetry.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "peregrine/utils/Preprocessors.h"

#include <algorithm>


namespace pmm
{
    PMM_INLINE constexpr PoolTelemetry::PoolTelemetry(const std::size_t poolSize, const std::size_t chunkSize,
                                                      const std::size_t alignment) noexcept
        : _poolSize(poolSize),
          _realChunkSize(chunkSize),
          _alignedChunkSize(chunkSize),
          _basePadding(0),
          _alignment(alignment),
          _maxAllocationCount(poolSize / chunkSize),
          _usedAllocationCount(0)
    {}




    PMM_INLINE constexpr void PoolTelemetry::setPadding(std::size_t padding) noexcept
    {
        _basePadding = padding;
        _maxAllocationCount = (_poolSize - _basePadding) / _alignedChunkSize;
    }


    PMM_INLINE constexpr void PoolTelemetry::setAlignedChunkSize(std::size_t chunkSize) noexcept
    {
        _alignedChunkSize = chunkSize;
        _maxAllocationCount = (_poolSize - _basePadding) / _alignedChunkSize;
    }


    /**************************************
     *              GETTERS               *
     **************************************/

    PMM_INLINE constexpr std::size_t PoolTelemetry::getPoolSize() const noexcept { return _poolSize; }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getChunkSize() const noexcept { return _realChunkSize; }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getAlignedChunkSize() const noexcept { return _alignedChunkSize; }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getMaxAllocationCount() const noexcept
    { return _maxAllocationCount; }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getUsedAllocationCount() const noexcept
    { return _usedAllocationCount; }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getFreeAllocationCount() const noexcept
    { return _maxAllocationCount - _usedAllocationCount; }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getAlignment() const noexcept { return _alignment; }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getPadding() const noexcept { return _basePadding; }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getUsedSize() const noexcept
    {
        return _alignedChunkSize * _usedAllocationCount + _basePadding;
    }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getWastedSize() const noexcept
    {
        return (_alignedChunkSize - _realChunkSize) * _usedAllocationCount + _basePadding;
    }

    PMM_INLINE constexpr std::size_t PoolTelemetry::getFreeSize() const noexcept
    {
        return _poolSize - getUsedSize();
    }

} // namespace pmm
