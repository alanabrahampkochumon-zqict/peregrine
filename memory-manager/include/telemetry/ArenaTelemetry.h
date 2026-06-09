#pragma once
/**
 * @file ArenaTelemetry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 30, 2026
 *
 * @brief Define structures and methods for Arena allocators telemetry collection.
 *
 * @note Telemetry is off by default and requires `ENABLE_PMM_TELEMETRY` macro defined to enable telemetry.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstddef>

namespace pmm
{

    /**
     * @addtogroup PMM_Telemetry
     * @{
     */

#ifdef ENABLE_PMM_TELEMETRY
    struct ArenaTelemetry
    {
        std::size_t currentUsage, peakUsage, minUsage, size;

        /**
         * @brief Default construct an arena telemetry with uninitialized parameters.
         *
         * @warning It is not recommended to use this method.
         *          Only provided for internal use.
         *          Use @ref Arena(std::size_t) to construct an ArenaTelemetry with an arena size.
         */
        ArenaTelemetry() = default;

        /**
         * @brief Initialize a new Arena telemetry instance with the current arena size.
         *
         * @param[in] size The size of the Arena.
         */
        [[nodiscard]] explicit constexpr ArenaTelemetry(std::size_t size) noexcept;


        /**
         * @brief Update the current arena usage by updating with the current allocation byte size.
         *
         * @note The function expects the size of current allocation, not the size of the arena's buffer,
         *       or the current offset.
         *
         * @param[in] allocatedByteSize The byte allocated in the current allocation call of the arena.
         */
        constexpr void updateAllocationUsage(std::size_t allocatedByteSize) noexcept;


        /**
         * @brief Reset the currentUsage, while preserving the peak and minimum usage.
         */
        constexpr void resetCurrentUsage() noexcept;


        /**
         * @brief Update the telemetry usage statistics to zero.
         */
        constexpr void resetTelemetry() noexcept;
    };

#else
    struct ArenaTelemetry
    {
        /**
         * @brief Default construct an arena telemetry with uninitialized parameters.
         *
         * @warning It is not recommended to use this method.
         *          Only provided for internal use.
         *          Use @ref Arena(std::size_t) to construct an ArenaTelemetry with an arena size.
         */
        ArenaTelemetry() = default;

        /**
         * @brief Initialize a dummy arena telemetry.
         *
         * @note STUB Method. Telemetry is disabled.
         */
        [[nodiscard]] explicit constexpr ArenaTelemetry(std::size_t) noexcept {}


        /**
         * @brief Update the current arena usage by updating with the current allocation byte size.
         *
         * @note STUB Method. Telemetry is disabled.
         */
        constexpr void updateAllocationUsage(std::size_t) noexcept {}


        /**
         * @brief Reset the currentUsage, while preserving the peak and minimum usage.
         *
         * @note STUB Method. Telemetry is disabled.
         */
        constexpr void resetCurrentUsage() noexcept {}

        /**
         * @brief Update the telemetry usage statistics to zero.
         *
         * @note STUB Method. Telemetry is disabled.
         */
        constexpr void resetTelemetry() noexcept {}
    };

#endif

    /** @} */

} // namespace pmm


#include "ArenaTelemetry.tpp"