#pragma once
/**
 * @file StackType.h
 * @author Alan Abraham P Kochumon
 * @date Created on: July 11, 2026
 *
 * @brief Storage policies used by allocators for changing internal behavior.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <concepts>


namespace pmm::stack
{
    /**
     * @addtogroup PMM_Storage_Policy
     * @{
     */

    /**
     * @brief Constraint used by different stack types.
     */
    template <typename T>
    concept StackType = requires(T policy) {
        // Member variables
        { T::getTypeIndex() } -> std::same_as<std::size_t>;
    };


    /**
     * @brief Define a stack with minimal header footprint, but it may not define absolute stack behavior.
     *
     * @relatedalso Strict
     */
    struct Loose
    {
        static std::size_t getTypeIndex() { return 1; }
    };


    /**
     * @brief Define a stack with full stack compliance, but using utmost twice the memory
     *        as when using @ref Loose.
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



    /** @} */

} // namespace pmm
