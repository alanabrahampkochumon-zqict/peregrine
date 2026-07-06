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
     *                                    *
     *          INITIALIZATIONS           *
     *                                    *
     **************************************/

    constexpr TempArena::TempArena(Arena* arena) noexcept
        : arena(arena), prevOffset(arena->_prevOffset), currentOffset(arena->_offset)
    {}


    constexpr TempArena::~TempArena() noexcept
    {
        arena->_prevOffset = prevOffset;
        arena->_offset     = currentOffset;
    }


    /**************************************
     *                                    *
     *            ALLOCATIONS             *
     *                                    *
     **************************************/

    inline void* TempArena::allocBytes(const std::size_t bytes, const std::size_t alignment) const noexcept
    {
        return arena->allocBytes(bytes, alignment);
    }


    template <typename T, typename... Args>
    constexpr T* TempArena::alloc(Args... args) noexcept
    {
        return arena->alloc<T>(args...);
    }


    template <typename T, typename... Args>
    constexpr T* TempArena::allocAs(const std::size_t alignment, Args... args) noexcept
    {
        return arena->allocAs<T>(alignment, args...);
    }


    template <typename T>
    constexpr std::span<T> TempArena::allocV(const std::size_t count) noexcept
    {
        return arena->allocV<T>(count);
    }


} // namespace pmm
