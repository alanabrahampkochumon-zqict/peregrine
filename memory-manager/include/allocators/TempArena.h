#pragma once
/**
 * @file TempArena.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 30, 2026
 *
 * @brief Temporary arena for memory save points.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Arena.h"

namespace pmm
{
    /**
     * @brief A lightweight, RAII-based scratch pad for temporary allocations.
     *        TempArena does NOT allocate OS memory. It acts as a bookmark for an existing
     *        Arena. When the TempArena goes out of scope, it automatically rewinds the
     *        backing Arena's offsets to their exact state at the moment of creation,
     *        instantly "freeing" all memory allocated during its lifetime.
     *
     * @warning You MUST assign this to a named local variable to maintain its scope.
     */
    struct [[nodiscard]] TempArena
    {
        Arena* arena;
        std::size_t prevOffset, currentOffset;

        /**
         * @brief Create a temporary snapshot of the @p arena.
         *
         * @param arena The arena to take a snapshot of.
         */
        [[nodiscard]] explicit constexpr TempArena(Arena* arena) noexcept;


        /**
         * @brief Delete the arena snapshot and restore the arena's state to the last known state.
         */
        constexpr ~TempArena() noexcept;
    };

} // namespace pmm


#include "TempArena.tpp"