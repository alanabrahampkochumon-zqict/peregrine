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


namespace pmm
{
    /**
     * @addtogroup PMM_Policy
     * @{
     */


    /**************************************
     *                                    *
     *            STACK TYPES             *
     *                                    *
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



    /**************************************
     *                                    *
     *          MEMORY OWNERSHIP          *
     *                                    *
     **************************************/

    /**
     * @brief Concept defining the requirements for a memory ownership strategy.
     *
     * @relatedalso ManagedMemory
     * @relatedalso UnmanagedMemory
     */
    template <typename Strategy>
    concept MemoryStrategy = requires {
        { Strategy::getId() } -> std::same_as<std::size_t>;
    };


    /**
     * @brief Memory strategy indicating that the allocator owns the memory.
     *        The allocator handles allocation/deallocation; users cannot directly free it.
     *
     * @relatedalso UnmanagedMemory
     */
    struct ManagedMemory
    {
        static constexpr std::size_t getId() noexcept { return 1; }
    };


    /**
     * @brief Memory strategy indicating that the memory is shared with or managed by the user.
     *        The allocator does not own the memory life cycle; the user must ensure its validity.
     *
     * @relatedalso ManagedMemory
     */
    struct UnmanagedMemory
    {
        static constexpr std::size_t getId() noexcept { return 2; }
    };



    /**************************************
     *                                    *
     *             TELEMETRY              *
     *                                    *
     **************************************/

    namespace telemetry
    {

        /**
         * @brief Concept defining the requirements for telemetry.
         *
         * @relatedalso Managed
         * @relatedalso Unmanaged
         * @relatedalso Disabled
         */
        template <typename T>
        concept TelemetryPolicy = requires(T) {
            { T::getPolicyName() } -> std::same_as<std::string>;
        };


        /**
         * @brief Telemetry policy indicating that the telemetry is managed by the allocator.
         *
         * @relatedalso Unmanaged
         * @relatedalso Disabled
         */
        struct Managed
        {
            static constexpr std::string getPolicyName() noexcept { return "Managed Telemetry"; }
        };

        /**
         * @brief Telemetry policy indicating that the telemetry is managed by the user.
         *        The user must provide a telemetry instance to the allocator.
         *
         * @relatedalso Managed
         * @relatedalso Disabled
         */
        struct Unmanaged
        {
            static constexpr std::string getPolicyName() noexcept { return "Unmanaged Telemetry"; }
        };


        /**
         * @brief Telemetry policy indicating that telemetry is disabled.
         *
         * @relatedalso Managed
         * @relatedalso Unmanaged
         */
        struct Disabled
        {
            static constexpr std::string getPolicyName() noexcept { return "No Telemetry"; }
        };

    } // namespace telemetry

    /** @} */

} // namespace pmm
