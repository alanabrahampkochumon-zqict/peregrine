#pragma once
/**
 * @file StackTelemetry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 03, 2026
 *
 * @brief Define structures and methods for pool allocator's telemetry collection.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "peregrine/allocators/Policy.h"

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


    /**
     * @brief Dummy telemetry used when telemetry policy is set to @p pmm::telemetry::Disabled.
     */
    struct DummyStackTelemetry
    {

        [[nodiscard]] explicit constexpr DummyStackTelemetry(std::size_t) noexcept {}

        constexpr void incStackUsage(std::size_t, std::size_t) noexcept {}

        constexpr void decStackUsage(std::size_t, std::size_t) noexcept {}

        constexpr void updateMinMemoryUsage(std::size_t) noexcept {}

        constexpr void updatePeakMemoryUsage(std::size_t) noexcept {}

        constexpr void updateMinPaddingUsage(std::size_t) noexcept {}

        constexpr void updatePeakPaddingUsage(std::size_t) noexcept {}

        constexpr void resetCurrentUsage() noexcept {}

        constexpr void resetTelemetry() noexcept {}

        [[nodiscard]] constexpr std::size_t getStackSize() const noexcept { return 0; }

        [[nodiscard]] constexpr std::size_t getCurrentMemoryUsage() const noexcept { return 0; }

        [[nodiscard]] constexpr std::size_t getMinMemoryUsage() const noexcept { return 0; }

        [[nodiscard]] constexpr std::size_t getPeakMemoryUsage() const noexcept { return 0; }

        [[nodiscard]] constexpr std::size_t getCurrentPadding() const noexcept { return 0; }

        [[nodiscard]] constexpr std::size_t getPeakPadding() const noexcept { return 0; }

        [[nodiscard]] constexpr std::size_t getMinPadding() const noexcept { return 0; }

        [[nodiscard]] constexpr std::size_t getTotalUsage() const noexcept { return 0; }
    };

    /**
     * @brief Define the type of stack telemetry based on the current telemetry policy.
     *
     * @tparam Policy Telemetry Policy used by the stack.
     */
    template <telemetry::TelemetryPolicy Policy>
    using StackTelemetryType =
        std::conditional_t<std::is_same_v<Policy, telemetry::Disabled>, DummyStackTelemetry, StackTelemetry>;



    /**
     * @brief Get a telemetry instance depending the telemetry policy in use by the target stack.
     *
     * @tparam Policy The Telemetry policy in use by the target stack.
     *
     * @param stackSize The size of the stack
     *
     * @return A stack telemetry instance suited for the telemetry policy.
     */
    template <telemetry::TelemetryPolicy Policy>
    constexpr StackTelemetryType<Policy> getTelemetryInstance(const std::size_t stackSize) noexcept
    {
        if constexpr (std::same_as<Policy, telemetry::Disabled>)
        {
            return DummyStackTelemetry(stackSize);
        }
        else
        {
            return StackTelemetry(stackSize);
        }
    }

    /** @} */

} // namespace pmm


#include "StackTelemetry.tpp"
