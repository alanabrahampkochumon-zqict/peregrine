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
         *
         * @relatedalso decStackUsage()
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
         *
         * @relatedalso incStackUsage()
         */
        constexpr void decStackUsage(std::size_t size, std::size_t padding) noexcept;


        /**
         * @brief Update the current minimum memory usage(non-padded) with @p usage
         *        if it is lower than the recorded minimum.
         *
         * @param[in] usage The new memory allocation size.
         *
         * @relatedalso updatePeakPaddingUsage()
         * @relatedalso updateMinPaddingUsage()
         * @relatedalso updatePeakMemoryUsage()
         */
        constexpr void updateMinMemoryUsage(std::size_t usage) noexcept;


        /**
         * @brief Update the current peak memory usage(non-padded) with @p usage if it is higher than the recorded peak.
         *
         * @param[in] usage The new memory allocation size.
         *
         * @relatedalso updatePeakPaddingUsage()
         * @relatedalso updateMinPaddingUsage()
         * @relatedalso updateMinMemoryUsage()
         */
        constexpr void updatePeakMemoryUsage(std::size_t usage) noexcept;


        /**
         * @brief Update the current minimum padding usage with @p usage if it is lower than the recorded minimum.
         *
         * @param[in] usage The new padding.
         *
         * @relatedalso updatePeakPaddingUsage()
         * @relatedalso updatePeakMemoryUsage()
         * @relatedalso updateMinMemoryUsage()
         */
        constexpr void updateMinPaddingUsage(std::size_t usage) noexcept;


        /**
         * @brief Update the current peak usage with @p usage if it is higher than the recorded peak.
         *
         * @param[in] usage The new padding.
         *
         * @relatedalso updateMinPaddingUsage()
         * @relatedalso updatePeakMemoryUsage()
         * @relatedalso updateMinMemoryUsage()
         */
        constexpr void updatePeakPaddingUsage(std::size_t usage) noexcept;


        /**
         * @brief Reset the current memory usage (including padding), while preserving the peak and minimum usage.
         *
         * @relatedalso resetTelemetry()
         */
        constexpr void resetCurrentUsage() noexcept;


        /**
         * @brief Update the telemetry usage statistics to zero.
         *
         * @relatedalso resetCurrentUsage()
         */
        constexpr void resetTelemetry() noexcept;


        /**
         * @brief Get the size of the stack.
         */
        [[nodiscard]] constexpr std::size_t getStackSize() const noexcept;


        /**
         * @brief Get the current memory usage (excluding padding) of the stack.
         *
         * @relatedalso getMinPadding()
         * @relatedalso getPeakPadding()
         * @relatedalso getCurrentPadding()
         *
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr std::size_t getCurrentMemoryUsage() const noexcept;


        /**
         * @brief Get the all-time minimum memory usage (excluding padding) of the stack.
         *
         * @relatedalso getMinPadding()
         * @relatedalso getPeakPadding()
         * @relatedalso getCurrentPadding()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr std::size_t getMinMemoryUsage() const noexcept;


        /**
         * @brief Get the all-time peak memory usage (excluding padding) of the stack.
         *
         * @relatedalso getMinPadding()
         * @relatedalso getPeakPadding()
         * @relatedalso getCurrentPadding()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr std::size_t getPeakMemoryUsage() const noexcept;


        /**
         * @brief Get the padding used the by the stack across all allocations.
         *
         * @note The value is not preserved if the stack is reset.
         *
         * @relatedalso getMinPadding()
         * @relatedalso getPeakPadding()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr std::size_t getCurrentPadding() const noexcept;


        /**
         * @brief Get the maximum padding used across all allocations.
         *
         * @relatedalso getCurrentPadding()
         * @relatedalso getMinPadding()
         */
        [[nodiscard]] constexpr std::size_t getPeakPadding() const noexcept;


        /**
         * @brief Get the maximum padding used across all allocations.
         *
         * @relatedalso getCurrentPadding()
         * @relatedalso getPeakPadding()
         */
        [[nodiscard]] constexpr std::size_t getMinPadding() const noexcept;


        /**
         * @brief Get the total buffer usage inclusive of padding.
         */
        [[nodiscard]] constexpr std::size_t getTotalUsage() const noexcept;

    private:
        std::size_t _currentUsage, _minMemoryUsage, _peakMemoryUsage;
        std::size_t _totalPadding, _minPaddingUsage, _peakPaddingUsage;
        std::size_t _size;
    };

    /** @} */

} // namespace pmm


#include "StackTelemetry.tpp"
