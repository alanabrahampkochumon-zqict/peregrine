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
#define ENABLE_PMM_TELEMETRY // TODO: Remove
#ifdef ENABLE_PMM_TELEMETRY
    struct ArenaTelemetry
    {
        std::size_t currentUsage, peakUsage, minUsage, size;

        /**
         * @brief Initialize a new Arena telemetry instance with the current arena size.
         *
         * @param size The size of the Arena.
         */
        [[nodiscard]] explicit constexpr ArenaTelemetry(std::size_t size) noexcept;


        /**
         * @brief Update the current arena usage by updating with the current allocation byte size.
         *
         * @note The function expects the size of current allocation, not the size of the arena's buffer,
         *       or the current offset.
         *
         * @param allocatedByteSize The byte allocated in the current allocation call of the arena.
         */
        constexpr void updateAllocationUsage(std::size_t allocatedByteSize) noexcept;


        /**
         * @brief Update the telemetry usage statistics to zero.
         */
        constexpr void resetTelemetry() noexcept;
    };


    #include "ArenaTelemetry.tpp"


#else
    struct ArenaTelemetry
    {
        /**
         * @brief Initialize a dummy arena telemetry.
         *
         * @note STUB Method. Telemetry is disabled.
         */
        [[nodiscard]] explicit constexpr ArenaTelemetry(std::size_t size) noexcept {}


        /**
         * @brief Update the current arena usage by updating with the current allocation byte size.
         *
         * @note STUB Method. Telemetry is disabled.
         */
         constexpr void updateAllocationUsage(std::size_t allocatedByteSize) noexcept {}


        /**
         * @brief Update the telemetry usage statistics to zero.
         *
         * @note STUB Method. Telemetry is disabled.
         */
        constexpr void resetTelemetry() noexcept {}
    };

#endif
} // namespace pmm