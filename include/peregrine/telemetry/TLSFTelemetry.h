#pragma once
/**
 * @file TLSFTelemetry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: September 22, 2026
 *
 * @brief Define structures and methods for pool allocator's telemetry collection.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "../allocators/Policy.h"

#include <cstddef>


namespace pmm
{

    /**
     * @addtogroup PMM_Telemetry
     * @{
     */

    struct TLSFTelemetry
    {

        /**
         * @brief Initialize a TLSF Telemetry instance with the current tlsf size.
         *
         * @param[in] size The size of the TLSF allocator.
         */
        [[nodiscard]] explicit constexpr TLSFTelemetry(size_t size) noexcept;


        /**
         * @brief Increment the current TLSF usage.
         *
         * @note The function expects the size of current allocation, not the size of the TLSF's buffer,
         *       or the current offset.
         *
         * @param[in] reqSize       The size requested by the user.
         * @param[in] allocatedSize The size of the allocated memory (including padding and header size).
         *
         * @relatedalso decUsage()
         */
        constexpr void incUsage(size_t reqSize, size_t allocatedSize) noexcept;


        /**
         * @brief Decrement the current TLSF usage.
         *
         * @note The function expects the size of current allocation, not the size of the TLSF's buffer,
         *       or the current offset.
         * @note This will not reset the recorded peak or minimum usage.
         *
         * @param[in] reqSize       The size of the allocation that was accessible by the user.
         * @param[in] allocatedSize The total size freed by the allocator.
         *
         * @relatedalso incUsage()
         */
        constexpr void decUsage(size_t reqSize, size_t allocatedSize) noexcept;


        /**
         * @brief Update the current minimum memory usage(non-padded) with @p usage
         *        if it is lower than the recorded minimum.
         *
         * @param[in] usage The new memory allocation size.
         *
         * @relatedalso updatePeakMemoryUsage()
         * @relatedalso updatePeakMetadataUsage()
         * @relatedalso updateMinMetadataUsage()
         */
        constexpr void updateMinMemoryUsage(size_t usage) noexcept;


        /**
         * @brief Update the current peak memory usage(non-padded) with @p usage if it is higher than the recorded peak.
         *
         * @param[in] usage The new memory allocation size.
         *
         * @relatedalso updateMinMemoryUsage()
         * @relatedalso updatePeakMetadataUsage()
         * @relatedalso updateMinMetadataUsage()
         */
        constexpr void updatePeakMemoryUsage(size_t usage) noexcept;


        /**
         * @brief Update the current minimum metadata usage with @p usage if it is lower than the recorded minimum.
         *
         * @param[in] usage The new metadata usage.
         *
         * @relatedalso updatePeakMemoryUsage()
         * @relatedalso updateMinMemoryUsage()
         * @relatedalso updatePeakMetadataUsage()
         */
        constexpr void updateMinMetadataUsage(size_t usage) noexcept;


        /**
         * @brief Update the current peak metadata usage with @p usage if it is higher than the recorded peak.
         *
         * @param[in] usage The new metadata usage.
         *
         * @relatedalso updatePeakMemoryUsage()
         * @relatedalso updateMinMemoryUsage()
         * @relatedalso updateMinMetadataUsage()
         */
        constexpr void updatePeakMetadataUsage(size_t usage) noexcept;


        /**
         * @brief Reset the current memory usage (including metadata usage),
         *        while preserving the peak and minimum usage.
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
         * @brief Get the size of the tlsf.
         */
        [[nodiscard]] constexpr size_t getTLSFSize() const noexcept;


        /**
         * @brief Get the current memory usage (excluding metadata size of allocations) of the tlsf.
         *
         * @relatedalso getMinMetadataUsage()
         * @relatedalso getPeakMetadataUsage()
         * @relatedalso getMetadataUsage()
         *
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr size_t getCurrentMemoryUsage() const noexcept;


        /**
         * @brief Get the all-time minimum memory usage (excluding metadata) of the tlsf.
         *
         * @relatedalso getMinMetadataUsage()
         * @relatedalso getPeakMetadataUsage()
         * @relatedalso getMetadataUsage()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr size_t getMinMemoryUsage() const noexcept;


        /**
         * @brief Get the all-time peak memory usage (excluding metadata) of the tlsf.
         *
         * @relatedalso getMinMetadataUsage()
         * @relatedalso getPeakMetadataUsage()
         * @relatedalso getMetadataUsage()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr size_t getPeakMemoryUsage() const noexcept;


        /**
         * @brief Get the metadata size used the by the tlsf across all allocations.
         *
         * @note The value is not preserved if the tlsf is reset.
         *
         * @relatedalso getMinMetadataUsage()
         * @relatedalso getPeakMetadataUsage()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr size_t getMetadataUsage() const noexcept;


        /**
         * @brief Get the maximum metadata size used across all allocations.
         *
         * @relatedalso getMetadataUsage()
         * @relatedalso getMinMetadataUsage()
         */
        [[nodiscard]] constexpr size_t getPeakMetadataUsage() const noexcept;


        /**
         * @brief Get the maximum metadata size used across all allocations.
         *
         * @relatedalso getMetadataUsage()
         * @relatedalso getPeakMetadataUsage()
         */
        [[nodiscard]] constexpr size_t getMinMetadataUsage() const noexcept;


        /**
         * @brief Get the total buffer usage including memory used for metadata.
         */
        [[nodiscard]] constexpr size_t getTotalUsage() const noexcept;

    private:
        size_t _totalMemoryUsage, _minTotalMemoryUsage, _peakTotalMemoryUsage;
        size_t _metadataUsage, _minMetadataUsage, _peakMetadataUsage;
        size_t _size;
    };


    /**
     * @brief Dummy telemetry used when telemetry policy is set to @p pmm::telemetry::Disabled.
     */
    struct DummyTLSFTelemetry
    {
        explicit constexpr DummyTLSFTelemetry(size_t) noexcept {}

        constexpr void incUsage(size_t, size_t) noexcept {}
        constexpr void decUsage(size_t, size_t) noexcept {}
        constexpr void updateMinMemoryUsage(size_t) noexcept {}
        constexpr void updatePeakMemoryUsage(size_t) noexcept {}
        constexpr void updateMinMetadataUsage(size_t) noexcept {}
        constexpr void updatePeakMetadataUsage(size_t) noexcept {}
        constexpr void resetCurrentUsage() noexcept {}
        constexpr void resetTelemetry() noexcept {}

        [[nodiscard]] constexpr size_t getTLSFSize() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getCurrentMemoryUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMinMemoryUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getPeakMemoryUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMetadataUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getPeakMetadataUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMinMetadataUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getTotalUsage() const noexcept { return 0; }
    };

    /**
     * @brief Define the type of tlsf telemetry based on the current telemetry policy.
     *
     * @tparam Policy Telemetry Policy used by the TLSF.
     */
    template <TelPolicy Policy>
    using TLSFTelemetryType = std::conditional_t<Policy == TelPolicy::Enabled, TLSFTelemetry, DummyTLSFTelemetry>;



    /**
     * @brief Get a telemetry instance depending the telemetry policy in use by the target tlsf.
     *
     * @tparam Policy The Telemetry policy in use by the target tlsf.
     *
     * @param tlsfSize The size of the tlsf
     *
     * @return A tlsf telemetry instance suited for the telemetry policy.
     */
    template <TelPolicy Policy>
    constexpr TLSFTelemetryType<Policy> getTelemetryInstance(const size_t tlsfSize) noexcept
    {
        if constexpr (Policy == TelPolicy::Enabled)
        {
            return TLSFTelemetry(tlsfSize);
        }
        else
        {
            return DummyTLSFTelemetry(tlsfSize);
        }
    }

    /** @} */

} // namespace pmm


#include "TLSFTelemetry.tpp"
