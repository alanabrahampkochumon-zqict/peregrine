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
         * @note This function must be called only once per allocation for logging accuracy.
         *
         * @param[in] reqSize  The size requested by the user.
         * @param[in] overhead The size used for metadata (padding, offsets, and header size).
         *
         * @relatedalso decUsage()
         */
        constexpr void incUsage(size_t reqSize, size_t overhead) noexcept;

        /**
         *
         * @brief Decrement the current TLSF usage.
         *
         * @note The function expects the size of current allocation, not the size of the TLSF's buffer,
         *       or the current offset.
         * @note This function must be called only once per allocation for logging accuracy.
         *
         * @param[in] reqSize       The size of the allocation that was accessible by the user.
         * @param[in] overhead The size used for metadata (padding, offsets, and header size).
         *
         * @relatedalso incUsage()
         */
        constexpr void decUsage(size_t reqSize, size_t overhead) noexcept;

        /// @ref Update the largest free block's size.
        /// @note The calculations needs to be done by the allocator as this a dummy setter.
        constexpr void updateLargestFreeBlockSize(size_t newSize) noexcept;

        /// @brief Increment the number of free blocks.
        constexpr void incFreeBlockCount() noexcept;

        /// @brief Decrement the number of free blocks.
        constexpr void decFreeBlockCount() noexcept;


        /// @brief Get the total capacity of the allocator in bytes.
        [[nodiscard]] constexpr size_t getTotalCapacity() const noexcept;

        /// @brief Get the total memory used by the allocator, including metadata/overhead size.
        [[nodiscard]] constexpr size_t getCurrentBufferUsage() const noexcept;

        /// @brief Get the minimum memory allocated, including metadata/overhead size.
        /// @note This value can only be cleared by calling @ref resetTelemetry.
        [[nodiscard]] constexpr size_t getMinBufferUsage() const noexcept;

        /// @brief Get the peak memory allocated, including metadata/overhead size.
        /// @note This value can only be cleared by calling @ref resetTelemetry.
        [[nodiscard]] constexpr size_t getPeakBufferUsage() const noexcept;

        /// @brief Get the total memory used by user payload (exclusive of metadata).
        [[nodiscard]] constexpr size_t getCurrentPayloadUsage() const noexcept;

        /// @brief Get the minimum memory allocated for user payload, excluding metadata/overhead size.
        /// @note This value can only be cleared by calling @ref resetTelemetry.
        [[nodiscard]] constexpr size_t getMinPayloadUsage() const noexcept;

        /// @brief Get the peak memory allocated for user payload, excluding metadata/overhead size.
        /// @note This value can only be cleared by calling @ref resetTelemetry.
        [[nodiscard]] constexpr size_t getPeakPayloadUsage() const noexcept;

        /// @brief Get the total memory used for internal metadata.
        [[nodiscard]] constexpr size_t getCurrentMetadataUsage() const noexcept;

        /// @brief Get the total memory used for internal metadata.
        [[nodiscard]] constexpr size_t getMinMetadataUsage() const noexcept;

        /// @brief Get the total memory used for internal metadata.
        [[nodiscard]] constexpr size_t getPeakMetadataUsage() const noexcept;

        /// @brief Get the largest free block in the allocator.
        [[nodiscard]] constexpr size_t getLargestFreeBlockSize() const noexcept;

        /// @brief Get the number of free blocks in the allocator.
        [[nodiscard]] constexpr size_t getFreeBlockCount() const noexcept;

        /// @brief Get the total active allocations in the allocator.
        [[nodiscard]] constexpr size_t getActiveAllocations() const noexcept;

        /// @brief Get the total number of allocations done in the lifetime of the allocator.
        [[nodiscard]] constexpr size_t getLifetimeAllocations() const noexcept;

        /// @brief Get the total number of frees lifetime during the allocator.
        [[nodiscard]] constexpr size_t getLifetimeFrees() const noexcept;

        /// @brief Get a hint if there is memory leak in the allocator.
        [[nodiscard]] constexpr bool hasMemoryLeak() const noexcept;

        /// @brief Reset the entire telemetry.
        constexpr void resetTelemetry() noexcept;

    private:
        size_t _currentBufferUsage, _minBufferUsage, _peakBufferUsage;
        size_t _currentMetadataUsage, _minMetadataUsage, _peakMetadataUsage;
        size_t _currentPayloadUsage, _minPayloadUsage, _peakPayloadUsage;
        size_t _allocatorSize;
        size_t _largestFreeBlockSize;
        size_t _freeBlockCount;
        size_t _activeAllocationCount, _lifetimeAllocationCount, _lifetimeFreeCount;
    };


    /**
     * @brief Dummy telemetry used when telemetry policy is set to @p pmm::telemetry::Disabled.
     */
    struct DummyTLSFTelemetry
    {
        explicit constexpr DummyTLSFTelemetry(size_t) noexcept {}

        constexpr void incUsage(size_t, size_t) noexcept {}
        constexpr void decUsage(size_t, size_t) noexcept {}
        constexpr void updateLargestFreeBlockSize(size_t) noexcept {}
        constexpr void incFreeBlockCount() noexcept {}
        constexpr void decFreeBlockCount() noexcept {}

        [[nodiscard]] constexpr size_t getTotalCapacity() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getCurrentBufferUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMinBufferUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getPeakBufferUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getCurrentPayloadUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMinPayloadUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getPeakPayloadUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getCurrentMetadataUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMinMetadataUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getPeakMetadataUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getLargestFreeBlockSize() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getFreeBlockCount() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getActiveAllocations() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getLifetimeAllocations() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getLifetimeFrees() const noexcept { return 0; }
        [[nodiscard]] constexpr bool hasMemoryLeak() const noexcept { return 0; }

        constexpr void resetTelemetry() noexcept {};
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
