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
     * @addtogroup PMM_TArena
     * @{
     */

    /**
     * @brief A lightweight, RAII-based scratch pad for temporary allocations.
     *        TempArena does NOT allocate OS memory. It acts as a bookmark for an existing
     *        Arena. When the TempArena goes out of scope, it automatically rewinds the
     *        backing Arena's offsets to their exact state at the moment of creation,
     *        instantly "freeing" all memory allocated during its lifetime.
     *
     * @warning You MUST assign this to a named local variable to maintain its scope.
     * @tparam MemoryPolicy         Memory management type. See @ref pmm::MemPolicy.
     * @tparam TelemetryPolicy      Policy indicating whether telemetry is enabled for this allocator.
     *                              See @ref pmm::TelPolicy. Disabled by default.
     * @tparam SafeMode             Policy dictating the safety of this allocator. When in safe mode, certain actions
     *                              like `nullptr` resize or free are handled gracefully, in *Release Mode*. Assertions
     *                              acts as safety value in both mode, given *Debug Mode* is enabled. Disabled by
     *                              default to prevent any performance stalls incurred by conditional checks.
     * @tparam MultithreadingPolicy Policy hinting whether the instance can handle multithreading safety.
     *                              Default: @ref mt::MTPolicy::NoMTPolicy.
     */
    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled,
              SafeModePolicy SafeMode = SafeModePolicy::Unsafe, mt::MTPolicy MultithreadingPolicy = mt::MTPolicy::NoMT>
    struct [[nodiscard]] TempArena
    {
        Arena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>* targetArena;
        std::size_t prevOffset, currentOffset;

        /**
         * @brief Create a temporary snapshot of the @p arena.
         *
         * @param arena The arena to take a snapshot of.
         */
        [[nodiscard]] explicit constexpr TempArena(
            Arena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>* arena) noexcept;


        /**
         * @brief Delete the arena snapshot and restore the arena's state to the last known state.
         */
        constexpr ~TempArena() noexcept;


        /**
         * @copydoc pmm::Arena::allocBytes(std::size_t, std::size_t)
         */
        [[nodiscard]] void* allocBytes(std::size_t bytes, std::size_t alignment = sizeof(void*)) const noexcept;


        /**
         *  @copydoc pmm::Arena::alloc(Args...)
         */
        template <typename T, typename... Args>
        [[nodiscard]] constexpr T* alloc(Args... args) noexcept;


        /**
         *  @copydoc pmm::Arena::allocV(std::size_t)
         */
        template <typename T>
        [[nodiscard]] constexpr std::span<T> allocV(std::size_t count) noexcept;
    };

    /** @} */

} // namespace pmm


#include "TempArena.tpp"
