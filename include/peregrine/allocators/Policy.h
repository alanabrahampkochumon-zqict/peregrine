#pragma once
/**
 * @file Policy.h
 * @author Alan Abraham P Kochumon
 * @date Created on: July 11, 2026
 *
 * @brief Ownership policies and strategies used by allocators for changing internal behavior.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


namespace pmm
{
    /**
     * @addtogroup PMM_Policy
     * @{
     */

    /**************************************
     *            STACK TYPES             *
     **************************************/

    /// @brief Stack allocator type.
    enum class StackPolicy : uint8_t
    {
        Loose, /// Stack with minimal memory footprint.
        Strict /// Stack with full LIFO compliance. Uses more memory than @ref StackPolicy::Loose.
    };


    /// @brief Allocator Telemetry configuration.
    enum class TelPolicy : uint8_t
    {
        Enabled,
        Disabled
    };


    /// @brief Allocator memory ownership configuration.
    enum class MemPolicy : uint8_t
    {
        Internal, ///< Memory buffer is owned by the allocator. Lifecycle is managed by allocator.
        External  ///< Memory buffer needs to be provided by used.
    };


    /// @brief Allocator safety policy configuration.
    enum class SafeModePolicy : uint8_t
    {
        Safe,  ///< Policy for safe allocator. Recommended for allocations/deallocation where validation is required.
        Unsafe ///< Policy for unsafe allocator. Faster due to lack of safe guardrails like nullptr checking.
    };

    /** @} */

} // namespace pmm
