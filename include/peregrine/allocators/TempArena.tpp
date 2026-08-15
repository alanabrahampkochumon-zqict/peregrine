#pragma once
/**
 * @file TempArena.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 30, 2026
 *
 * @brief Implementation for Temporary Arena member functions defined in
 *        TempArena.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "peregrine/utils/Preprocessors.h"


namespace pmm
{
    /**************************************
     *          INITIALIZATIONS           *
     **************************************/

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr TempArena<MemStrategy, TelPolicy, Safe>::TempArena(
        Arena<MemStrategy, TelPolicy, Safe>* arena) noexcept
        : targetArena(arena), prevOffset(arena->_prevOffset), currentOffset(arena->_offset)
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE constexpr TempArena<MemStrategy, TelPolicy, Safe>::~TempArena() noexcept
    {
        targetArena->_prevOffset = prevOffset;
        targetArena->_offset     = currentOffset;
    }


    /**************************************
     *            ALLOCATIONS             *
     **************************************/

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    PMM_INLINE void* TempArena<MemStrategy, TelPolicy, Safe>::allocBytes(const std::size_t bytes,
                                                                         const std::size_t alignment) const noexcept
    { return targetArena->allocBytes(bytes, alignment); }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    template <typename T, typename... Args>
    PMM_INLINE constexpr T* TempArena<MemStrategy, TelPolicy, Safe>::alloc(Args... args) noexcept
    { return targetArena->template alloc<T>(args...); }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    template <typename T>
    PMM_INLINE constexpr std::span<T> TempArena<MemStrategy, TelPolicy, Safe>::allocV(const std::size_t count) noexcept
    { return targetArena->template allocV<T>(count); }


} // namespace pmm
