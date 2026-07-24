#pragma once
/**
 * @file StackTelemetry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: July 24, 2026
 *
 * @brief Define structures and methods for Stack allocators telemetry collection.
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

    struct StackTelemetry
    {

        /**
         * @brief Initialize a Stack Telemetry instance with the current stack size.
         *
         * @param[in] size The size of the Stack.
         */
        [[nodiscard]] explicit constexpr StackTelemetry(std::size_t size) noexcept;


        /**
         * @brief Update the current stack usage by updating with the current allocation byte size.
         *
         * @note The function expects the size of current allocation, not the size of the stack's buffer,
         *       or the current offset.
         *
         * @param[in] allocatedByteSize The byte allocated in the current allocation call of the stack.
         */
        constexpr void updateAllocationUsage(std::size_t allocatedByteSize) noexcept;


        /**
         * @brief Update the current minimum usage with @p usage if it is lower than the recorded minimum.
         *
         * @param[in] usage The new minimum usage.
         */
        constexpr void updateMinUsage(std::size_t usage) noexcept;


        /**
         * @brief Update the current peak usage with @p usage if it is higher than the recorded peak.
         * @param[in] usage The new peak usage.
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
         * @brief Get the size of the stack.
         */
        [[nodiscard]] constexpr std::size_t getStackSize() const noexcept;

        /**
         * @brief Get the current memory usage of the stack.
         */
        [[nodiscard]] constexpr std::size_t getCurrentUsage() const noexcept;

        /**
         * @brief Get the all-time minimum memory usage of the stack.
         */
        [[nodiscard]] constexpr std::size_t getMinUsage() const noexcept;

        /**
         * @brief Get the all-time peak memory usage of the stack.
         */
        [[nodiscard]] constexpr std::size_t getPeakUsage() const noexcept;

        /**
         * @brief Get the padding used the by the stack across all allocations.
         */
        [[nodiscard]] constexpr std::size_t getPadding() const noexcept;

        bool enabled{ true };

    private:
        std::size_t _currentUsage;
        std::size_t _peakUsage;
        std::size_t _padding;
        std::size_t _minUsage;
        std::size_t _size;
    };

    /** @} */

} // namespace pmm


#include "StackTelemetry.tpp"
