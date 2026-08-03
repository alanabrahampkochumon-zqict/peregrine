#pragma once
/**
 * @file PoolTelemetry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: July 24, 2026
 *
 * @brief Define structures and methods for stack allocator's telemetry collection.
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

    struct PoolTelemetry
    {

        /**
         * @brief Initialize a Pool Telemetry instance.
         *
         * @param[in] poolSize  The size of the pool.
         * @param[in] chunkSize The size of the chunk prior to alignment.
         * @param[in] alignment The byte alignment of the pool.
         *
         */
        [[nodiscard]] explicit constexpr PoolTelemetry(std::size_t poolSize, std::size_t chunkSize,
                                                       std::size_t alignment) noexcept;


        /**
         * @brief Get the size of the pool allocator.
         */
        [[nodiscard]] constexpr std::size_t getPoolSize() const noexcept;


        /**
         * @brief Get the unaligned chunk/fragment size of each pool allocation.
         * @note This is the raw values requested by the user. For chunkSize aligned with pool's default alignment
         *       use @ref getAlignedChunkSize()
         *
         * @relatedalso getAlignedChunkSize()
         */
        [[nodiscard]] constexpr std::size_t getChunkSize() const noexcept;


        /**
         * @brief Get the aligned chunk/fragment size of each pool allocation.
         *
         * @relatedalso getAlignedChunkSize()
         */
        [[nodiscard]] constexpr std::size_t getAlignedChunkSize() const noexcept;


        /**
         * @brief Get the maximum number of chunks/fragments in the pool.
         */
        [[nodiscard]] constexpr std::size_t getMaxAllocationCount() const noexcept;


        /**
         * @brief Get the number of used chunks/fragments in the pool.
         */
        [[nodiscard]] constexpr std::size_t getUsedAllocationCount() const noexcept;


        /**
         * @brief Get the number of free chunks/fragments in the pool.
         */
        [[nodiscard]] constexpr std::size_t getFreeAllocationCount() const noexcept;


        /**
         * @brief Get the pool's base alignment.
         */
        [[nodiscard]] constexpr std::size_t getAlignment() const noexcept;


        /**
         * @brief Get the padding applied to the pool buffer to ensure alignment.
         */
        [[nodiscard]] constexpr std::size_t getPadding() const noexcept;


        /**
         * @brief Update the telemetry's padding with the new @p padding.
         * @param padding The padding to update with.
         */
        constexpr void setPadding(std::size_t padding) noexcept;


        /**
         * @brief Update the telemetry's aligned chunk size with @p chunkSize.
         *
         * @note This will not update the real chunk size, as telemetry considers them as two different property.
         *
         * @param chunkSize The aligned chunk size.
         */
        constexpr void setAlignedChunkSize(std::size_t chunkSize) noexcept;



    private:
        std::size_t _poolSize, _realChunkSize, _alignedChunkSize, _basePadding, _alignment;
        std::size_t _maxAllocationCount, _usedAllocationCount;
    };





    /**
     * @brief Dummy telemetry used when telemetry policy is set to @p pmm::telemetry::Disabled.
     */
    struct DummyPoolTelemetry
    {
        [[nodiscard]] explicit constexpr DummyPoolTelemetry(std::size_t, std::size_t, std::size_t) noexcept {}
    };

    /**
     * @brief Define the type of stack telemetry based on the current telemetry policy.
     *
     * @tparam Policy Telemetry Policy used by the stack.
     */
    template <telemetry::TelemetryPolicy Policy>
    using PoolTelemetryType =
        std::conditional_t<std::is_same_v<Policy, telemetry::Disabled>, DummyPoolTelemetry, PoolTelemetry>;



    /**
     * @brief Get a telemetry instance depending the telemetry policy in use by the target stack.
     *
     * @tparam Policy The Telemetry policy in use by the target stack.
     *
     * @param[in] poolSize  The size of the Pool.
     * @param[in] chunkSize The size of the chunk prior to alignment.
     * @param[in] alignment The byte alignment of the pool.
     *
     * @return A stack telemetry instance suited for the telemetry policy.
     */
    template <telemetry::TelemetryPolicy Policy>
    constexpr PoolTelemetryType<Policy> getTelemetryInstance(const std::size_t poolSize, const std::size_t chunkSize,
                                                             const std::size_t alignment) noexcept
    {
        if constexpr (std::same_as<Policy, telemetry::Disabled>)
        {
            return DummyPoolTelemetry(poolSize, chunkSize, alignment);
        }
        else
        {
            return PoolTelemetry(poolSize, chunkSize, alignment);
        }
    }

    /** @} */

} // namespace pmm


#include "PoolTelemetry.tpp"
