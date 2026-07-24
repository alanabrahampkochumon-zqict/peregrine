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
         * @brief Increment the current stack usage.
         *
         * @note The function expects the size of current allocation, not the size of the stack's buffer,
         *       or the current offset.
         *
         * @param[in] size    The size of the allocated memory (exclusive of padding) in bytes.
         * @param[in] padding The padding used the by the allocation in bytes.
         */
        constexpr void incStackUsage(std::size_t size, std::size_t padding) noexcept;


        /**
         * @brief Decrement the current stack usage.
         *
         * @note The function expects the size of current allocation, not the size of the stack's buffer,
         *       or the current offset.
         * @note This will not reset the recorded peak or minimum usage.
         *
         * @param[in] size    The size of the freed memory (exclusive of padding) in bytes.
         * @param[in] padding The padding used the by the allocation in bytes.
         */
        constexpr void decStackUsage(std::size_t size, std::size_t padding) noexcept;


        /**
         * @brief Update the current minimum memory usage with @p usage if it is lower than the recorded minimum.
         *
         * @param[in] usage The new memory allocation size.
         */
        constexpr void updateMinUsage(std::size_t usage) noexcept;


        /**
         * @brief Update the current peak usage memory with @p usage if it is higher than the recorded peak.
         *
         * @param[in] usage The new memory allocation size.
         */
        constexpr void updatePeakUsage(std::size_t usage) noexcept;


        /**
         * @brief Update the current minimum padding usage with @p usage if it is lower than the recorded minimum.
         *
         * @param[in] usage The new padding.
         */
        constexpr void updateMinPaddingUsage(std::size_t usage) noexcept;


        /**
         * @brief Update the current peak usage with @p usage if it is higher than the recorded peak.
         *
         * @param[in] usage The new padding.
         */
        constexpr void updatePeakPaddingUsage(std::size_t usage) noexcept;


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
         *
         * @relatedalso getMinPaddingUsage()
         * @relatedalso getPeakPaddingUsage()
         */
        [[nodiscard]] constexpr std::size_t getTotalPadding() const noexcept;


        /**
         * @brief Get the maximum padding used across all allocations.
         *
         * @relatedalso getTotalPadding()
         * @relatedalso getMinPaddingUsage()
         */
        [[nodiscard]] constexpr std::size_t getPeakPaddingUsage() const noexcept;


        /**
         * @brief Get the maximum padding used across all allocations.
         *
         * @relatedalso getTotalPadding()
         * @relatedalso getPeakPaddingUsage()
         */
        [[nodiscard]] constexpr std::size_t getMinPaddingUsage() const noexcept;

        bool enabled{ true };

    private:
        std::size_t _currentUsage;
        std::size_t _peakUsage;
        std::size_t _padding;
        std::size_t _totalPadding, _peakPaddingUsage, _minPaddingUsage;
        std::size_t _minUsage;
        std::size_t _size;
    };

    /** @} */

} // namespace pmm


#include "StackTelemetry.tpp"
