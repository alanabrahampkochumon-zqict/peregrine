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
    constexpr TempArena::TempArena(Arena* arena) noexcept
        : arena(arena), prevOffset(arena->_prevOffset), currentOffset(arena->_offset)
    {}


    constexpr TempArena::~TempArena() noexcept
    {
        arena->_prevOffset = prevOffset;
        arena->_offset = currentOffset;
    }

} // namespace pmm