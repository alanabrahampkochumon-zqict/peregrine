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



#include "peregrine/utils/Preprocessors.h"


namespace pmm
{

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr Pool<MemStrategy, TelPolicy>::Pool(const size_t poolSize, const size_t chuckSize,
                                                            const size_t chunkAlignment) noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
        : _poolSize{ poolSize }, _chunkSize{ chuckSize }, _chunkAlignment{ chunkAlignment }
    {
        // TODO: Update to HAL
        _buffer = new uint8_t[poolSize];
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr Pool<MemStrategy, TelPolicy>::Pool(uint8_t* backingBuffer, const size_t bufferSize,
                                                            const size_t chuckSize,
                                                            const size_t chunkAlignment) noexcept
        requires std::same_as<MemStrategy, UnmanagedMemory>
        : _buffer{ backingBuffer }, _poolSize{ bufferSize }, _chunkSize{ chuckSize }, _chunkAlignment{ chunkAlignment }
    {}

} // namespace pmm
