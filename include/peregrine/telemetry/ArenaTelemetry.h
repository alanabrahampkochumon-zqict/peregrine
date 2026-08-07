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


#include "../allocators/Policy.h"

namespace pmm
{
    /**
     * @addtogroup PMM_Telemetry
     * @{
     */

    struct ArenaTelemetry
    {
        /**
         * @brief Initialize a new Arena telemetry instance with the current arena size.
         *
         * @param[in] size The size of the Arena.
         */
        [[nodiscard]] explicit constexpr ArenaTelemetry(size_t size) noexcept;


        /**
         * @brief Update the current arena usage.
         *
         * @note The function expects the size of current allocation, not the size of the arena's buffer,
         *       or the current offset.
         *
         * @param[in] allocatedByteSize The byte allocated in the current allocation call of the arena.
         */
        constexpr void logAllocationUsage(size_t allocatedByteSize) noexcept;


        /**
         * @brief Update the current minimum usage with @p usage if it is lower than the recorded minimum.
         * @param usage The new minimum usage.
         */
        constexpr void logMinUsage(size_t usage) noexcept;


        /**
         * @brief Update the current peak usage with @p usage if it is higher than the recorded peak.
         * @param usage The new peak usage.
         */
        constexpr void logPeakUsage(size_t usage) noexcept;


        /**
         * @brief Reset the currentUsage, while preserving the peak and minimum usage.
         */
        constexpr void resetCurrentUsage() noexcept;


        /**
         * @brief Update the telemetry usage statistics to zero.
         */
        constexpr void resetTelemetry() noexcept;


        /**
         * @brief Get the size of the arena.
         */
        [[nodiscard]] constexpr size_t getArenaSize() const noexcept;


        /**
         * @brief Get the current memory usage of the arena.
         */
        [[nodiscard]] constexpr size_t getCurrentUsage() const noexcept;

        /**
         * @brief Get the all-time minimum memory usage of the arena.
         */
        [[nodiscard]] constexpr size_t getMinUsage() const noexcept;


        /**
         * @brief Get the all-time maximum memory usage of the arena.
         */
        [[nodiscard]] constexpr size_t getPeakUsage() const noexcept;

    private:
        size_t _currentUsage{ 0 };
        size_t _peakUsage{ 0 };
        size_t _minUsage;
        size_t _arenaSize;
    };

    /**
     * @brief Dummy telemetry used when telemetry policy is set to @p pmm::telemetry::Disabled.
     */
    struct DummyArenaTelemetry
    {
        [[nodiscard]] explicit constexpr DummyArenaTelemetry(size_t) noexcept {}
        constexpr void logAllocationUsage(size_t) noexcept {}
        constexpr void logMinUsage(size_t) noexcept {}
        constexpr void logPeakUsage(size_t) noexcept {}
        constexpr void resetCurrentUsage() noexcept {}
        constexpr void resetTelemetry() noexcept {}
        [[nodiscard]] constexpr size_t getArenaSize() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getCurrentUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMinUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getPeakUsage() const noexcept { return 0; }
    };


    /**
     * @brief Define the type of arena telemetry based on the current telemetry policy.
     *
     * @tparam Policy Telemetry Policy used by the arena.
     */
    template <telemetry::TelemetryPolicy Policy>
    using ArenaTelemetryType =
        std::conditional_t<std::is_same_v<Policy, telemetry::Disabled>, DummyArenaTelemetry, ArenaTelemetry>;


    /**
     * @brief Get a telemetry instance depending the telemetry policy in use by the target arena.
     *
     * @tparam Policy The Telemetry policy in use by the target arena.
     *
     * @param[in] arenaSize  The size of the Arena.
     *
     * @return A arena telemetry instance suited for the telemetry policy.
     */
    template <telemetry::TelemetryPolicy Policy>
    constexpr ArenaTelemetryType<Policy> getTelemetryInstance(const std::size_t arenaSize) noexcept
    {
        if constexpr (std::same_as<Policy, telemetry::Disabled>)
        {
            return DummyArenaTelemetry(arenaSize);
        }
        else
        {
            return ArenaTelemetry(arenaSize);
        }
    }


    /** @} */

} // namespace pmm


#include "ArenaTelemetry.tpp"
