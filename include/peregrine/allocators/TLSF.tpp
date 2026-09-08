#pragma once
/**
 * @file TLSF.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 08, 2026
 *
 * @brief Implmentation of member functions defined in TLSF.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "../memory/Memory.h"


namespace pmm
{
    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(uint8_t* buffer,
                                                                            const size_t memorySize) noexcept
        requires std::same_as<MemStrategy, UnmanagedMemory>
        : _buffer{ buffer }, _size{ memorySize }, _usedSize{ 0 }, _flBitmask{ 0 }, _slBitmask{}
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(const size_t allocatorSize) noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
        : _buffer{ static_cast<uint8_t*>(memAlloc(allocatorSize)) },
          _size{ allocatorSize },
          _usedSize{ 0 },
          _flBitmask{ 0 },
          _slBitmask{}
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::TLSF(TLSF&& tlsf) noexcept
        : _buffer{ std::exchange(tlsf._buffer, nullptr) },
          _size{ tlsf._size },
          _usedSize{ tlsf._usedSize },
          _flBitmask{ tlsf._flBitmask }
    { std::move(tlsf._slBitmask.begin(), tlsf._slBitmask.end(), _slBitmask.begin()); }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>& TLSF<MemStrategy, TelPolicy, Safe,
                                                                            MTPolicy>::operator=(TLSF&& tlsf) noexcept
    {
        // If the memory is internally managed, we need to free it before moving over the other allocator's
        // internal buffer.
        if constexpr (std::is_same_v<MemStrategy, ManagedMemory>)
        {
            memFree(_buffer, _size);
        }
        // TODO: Update to assert if hampering performance
        if (this == &tlsf)
        {
            return *this;
        }

        _buffer    = std::exchange(tlsf._buffer, nullptr);
        _size      = tlsf._size;
        _usedSize  = tlsf._usedSize;
        _flBitmask = tlsf._flBitmask;
        std::move(tlsf._slBitmask.begin(), tlsf._slBitmask.end(), _slBitmask.begin());

        return *this;
    }



    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::~TLSF() noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
    { memFree(_buffer, _size); }


    /**************************************
     *             GETTERS                *
     **************************************/

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr size_t TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::size() const noexcept
    { return _size; }

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr size_t TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::usedSize() const noexcept
    { return _usedSize; }

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe, mt::MTPolicy MTPolicy>
    PMM_INLINE constexpr size_t TLSF<MemStrategy, TelPolicy, Safe, MTPolicy>::freeSize() const noexcept
    { return _size - _usedSize; }
} // namespace pmm
