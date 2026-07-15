#pragma once
/**
 * @file StoragePolicy.h
 * @author Alan Abraham P Kochumon
 * @date Created on: July 11, 2026
 *
 * @brief Storage policies used by allocators for changing internal behavior.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <concepts>


namespace pmm
{
    /**
     * @addtogroup PMM_Storage_Policy
     * @{
     */
    // TODO:

    template <typename T>
    concept StoragePolicy = requires(T policy) {
        // Member variables
        { policy.getPolicyIndex() } -> std::same_as<std::size_t>;
    };


    // TODO: Add docs
    struct MinimalStackPolicy
    {
        std::size_t getPolicyIndex() { return _policyIndex; }

    private:
        std::size_t _policyIndex = 1;
    };


    struct SafeStackPolicy
    {
        std::size_t getPolicyIndex() { return _policyIndex; }

    private:
        std::size_t _policyIndex = 2;
    };



    /** @} */

} // namespace pmm
