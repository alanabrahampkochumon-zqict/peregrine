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



namespace pmm
{
    /**************************************
     *          INITIALIZATIONS           *
     **************************************/

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    constexpr TempArena<MemStrategy, TelPolicy, Safe>::TempArena(Arena* arena) noexcept
        : targetArena(arena), prevOffset(arena->_prevOffset), currentOffset(arena->_offset)
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    constexpr TempArena<MemStrategy, TelPolicy, Safe>::~TempArena() noexcept
    {
        targetArena->_prevOffset = prevOffset;
        targetArena->_offset     = currentOffset;
    }


    /**************************************
     *            ALLOCATIONS             *
     **************************************/

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    inline void* TempArena<MemStrategy, TelPolicy, Safe>::allocBytes(const std::size_t bytes, const std::size_t alignment) const noexcept
    {
        return targetArena->allocBytes(bytes, alignment);
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    template <typename T, typename... Args>
    constexpr T* TempArena<MemStrategy, TelPolicy, Safe>::alloc(Args... args) noexcept
    {
        return targetArena->alloc<T>(args...);
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy, bool Safe>
    template <typename T>
    constexpr std::span<T> TempArena<MemStrategy, TelPolicy, Safe>::allocV(const std::size_t count) noexcept
    {
        return targetArena->allocV<T>(count);
    }


} // namespace pmm
