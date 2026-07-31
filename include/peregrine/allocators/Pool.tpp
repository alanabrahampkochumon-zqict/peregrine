#pragma once
/**
 * @file Pool.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 27, 2026
 *
 * @brief Implementation of member functions defined in Pool.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */



#include "peregrine/utils/Helpers.h"
#include "peregrine/utils/Preprocessors.h"


namespace pmm
{

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr Pool<MemStrategy, TelPolicy>::Pool(const size_t poolSize, const size_t chuckSize,
                                                            const size_t chunkAlignment) noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
        : _buffer{ new uint8_t[poolSize] },
          _poolSize{ poolSize },
          _chunkSize{ chuckSize },
          _chunkAlignment{ chunkAlignment },
          _head(nullptr)
    {
        const auto baseAddress = reinterpret_cast<uintptr_t>(_buffer);

        // Align base address and chunk size to the target padding.
        _initialAlignmentPadding = calcAlignmentPadding(baseAddress, chunkAlignment);
        _chunkSize += calcAlignmentPadding(_chunkSize, chunkAlignment);
        _chunkCount = (poolSize - _initialAlignmentPadding) / _chunkSize;

        PMM_ASSERT_MSG(_chunkSize >= sizeof(PoolFreeNode), "Inadequate chunk size");
        PMM_ASSERT_MSG(_poolSize - _initialAlignmentPadding >= _chunkSize, "Backing buffer smaller than chunk size");

        // TODO: clear()
        // Telemetry use
        // const auto numChunks = (baseAddress - _initialAlignmentPadding) / _chunkSize;
        // const auto usableSize = numChunks * _chunkSize;

        // TODO: Update buffer init to HAL
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr Pool<MemStrategy, TelPolicy>::Pool(uint8_t* backingBuffer, const size_t bufferSize,
                                                            const size_t chuckSize,
                                                            const size_t chunkAlignment) noexcept
        requires std::same_as<MemStrategy, UnmanagedMemory>
        : _buffer{ backingBuffer }, _poolSize{ bufferSize }, _chunkSize{ chuckSize }, _chunkAlignment{ chunkAlignment }
    {
        const auto baseAddress = reinterpret_cast<uintptr_t>(_buffer);

        // Align base address and chunk size to the target padding.
        _initialAlignmentPadding = calcAlignmentPadding(baseAddress, chunkAlignment);
        _chunkSize += calcAlignmentPadding(_chunkSize, chunkAlignment);
        _chunkCount = (bufferSize - _initialAlignmentPadding) / _chunkSize;

        PMM_ASSERT_MSG(_chunkSize >= sizeof(PoolFreeNode), "Inadequate chunk size");
        PMM_ASSERT_MSG(_poolSize - _initialAlignmentPadding >= _chunkSize, "Backing buffer smaller than chunk size");

        // TODO: clear()
        // Telemetry use
        // const auto numChunks = (baseAddress - _initialAlignmentPadding) / _chunkSize;
        // const auto usableSize = numChunks * _chunkSize;

        // TODO: Update buffer init to HAL
    }



    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE void Pool<MemStrategy, TelPolicy>::clear()
    {}

} // namespace pmm
