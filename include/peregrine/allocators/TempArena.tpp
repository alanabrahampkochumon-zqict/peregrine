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

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::TempArena(
        Arena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>* arena) noexcept
        : targetArena(arena), prevOffset(arena->_prevOffset), currentOffset(arena->_offset)
    {}


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::~TempArena() noexcept
    {
        targetArena->_prevOffset = prevOffset;
        targetArena->_offset     = currentOffset;
    }


    /**************************************
     *            ALLOCATIONS             *
     **************************************/

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::allocBytes(const std::size_t bytes,
                                                                         const std::size_t alignment) const noexcept
    { return targetArena->allocBytes(bytes, alignment); }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T, typename... Args>
    PMM_INLINE constexpr T* TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::alloc(Args... args) noexcept
    { return targetArena->template alloc<T>(args...); }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T>
    PMM_INLINE constexpr std::span<T> TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::allocV(const std::size_t count) noexcept
    { return targetArena->template allocV<T>(count); }


} // namespace pmm
