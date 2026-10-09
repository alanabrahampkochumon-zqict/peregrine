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


#include <concepts>
#include <mutex>

namespace pmm
{
    /**
     * @addtogroup PMM_Policy
     * @{
     */

    // TODO: Update stack to enum type

    /**************************************
     *            STACK TYPES             *
     **************************************/

    namespace stack
    {
        /**
         * @brief Concept defining requirements for different stack types.
         */
        template <typename T>
        concept StackType = requires(T policy) {
            // Member variables
            { T::getTypeIndex() } -> std::same_as<std::size_t>;
        };


        /**
         * @brief Stack with minimal header footprint, but it may not define absolute stack behavior.
         *
         * @relatedalso Strict
         */
        struct Loose
        {
            static std::size_t getTypeIndex() { return 1; }
        };


        /**
         * @brief Stack with full LIFO compliance, using more memory than @ref stack::Loose.
         *
         * @note The foolproofness of the policy are implementation dependant.
         *       For example: free(void*) may or may not check for `nullptr` depending on whether
         *       assertions or conditionals are used.
         *
         * @relatedalso Loose
         */
        struct Strict
        {
            static std::size_t getTypeIndex() { return 2; }
        };

    } // namespace stack


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
