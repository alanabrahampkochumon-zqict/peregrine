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

        /**
         * @brief Default construction of arena telemetry with no size is strictly prohibited.
         *
         * @note Use @ref Arena(std::size_t) to construct an ArenaTelemetry with an arena size.
         */
        ArenaTelemetry() = delete;


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
         * @brief Update the current minimum usage with @p usage if it is lower than the recorded minimum.
         * @param usage The new minimum usage.
         */
        constexpr void updateMinUsage(std::size_t usage) noexcept;


        /**
         * @brief Update the current peak usage with @p usage if it is higher than the recorded peak.
         * @param usage The new peak usage.
         */
        constexpr void updatePeakUsage(std::size_t usage) noexcept;


        /**
         * @brief Reset the currentUsage, while preserving the peak and minimum usage.
         */
        constexpr void resetCurrentUsage() noexcept;


        /**
         * @brief Update the telemetry usage statistics to zero.
         */
        constexpr void resetTelemetry() noexcept;


        /**
         * @brief Get the size of the arena that the telemetry is attached to.
         * @return Size of the arena.
         */
        [[nodiscard]] constexpr std::size_t getArenaSize() const noexcept;

        /**
         * @brief Get the current memory usage of the arena, the telemetry is attached to.
         * @return Current memory used in the arena.
         */
        [[nodiscard]] constexpr std::size_t getCurrentUsage() const noexcept;

        /**
         * @brief Get the all-time minimum of memory used in the arena, that the telemetry is attached to.
         * @return Minimum memory used in the arena.
         */
        [[nodiscard]] constexpr std::size_t getMinUsage() const noexcept;

        /**
         * @brief Get the all-time maximum of memory used in the arena, that the telemetry is attached to.
         * @return Minimum memory used in the arena.
         */
        [[nodiscard]] constexpr std::size_t getPeakUsage() const noexcept;

    private:
        std::size_t _currentUsage;
        std::size_t _peakUsage;
        std::size_t _minUsage;
        std::size_t _size;
    };
#else
    struct ArenaTelemetry
    {
        /**
         * @brief Default construction of arena telemetry with no size is strictly prohibited.
         *
         * @note Use @ref Arena(std::size_t) to construct an ArenaTelemetry with an arena size.
         */
        ArenaTelemetry() = delete;


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
         * @brief Update the current minimum usage with @p usage if it is lower than the recorded minimum.
         *
         * @note STUB Method. Telemetry is disabled.
         */
        constexpr void updateMinUsage(std::size_t) noexcept {}


        /**
         * @brief Update the current peak usage with @p usage if it is higher than the recorded peak.
         *
         * @note STUB Method. Telemetry is disabled.
         */
        constexpr void updatePeakUsage(std::size_t) noexcept {}


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


        /**
         * @brief Get the size of the arena that the telemetry is attached to.
         * @note STUB Method. Telemetry is disabled.
         *
         * @return 0.
         */
        [[nodiscard]] constexpr std::size_t getArenaSize() const noexcept
        {
            return 0;
        }

        /**
         * @brief Get the current memory usage of the arena, the telemetry is attached to.
         * @note STUB Method. Telemetry is disabled.
         *
         * @return 0.
         */
        [[nodiscard]] constexpr std::size_t getCurrentUsage() const noexcept
        {
            return 0;
        }

        /**
         * @brief Get the all-time minimum of memory used in the arena, that the telemetry is attached to.
         * @note STUB Method. Telemetry is disabled.
         *
         * @return 0.
         */
        [[nodiscard]] constexpr std::size_t getMinUsage() const noexcept
        {
            return 0;
        }

        /**
         * @brief Get the all-time maximum of memory used in the arena, that the telemetry is attached to.
         * @note STUB Method. Telemetry is disabled.
         *
         * @return 0.
         */
        [[nodiscard]] constexpr std::size_t getPeakUsage() const noexcept
        {
            return 0;
        }
    };

#endif

    /** @} */

} // namespace pmm


#include "ArenaTelemetry.tpp"