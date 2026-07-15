#pragma once
/**
 * @file Stack.h
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Stack allocator following LIFO (Last In, First Out) memory management strategy.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "StoragePolicy.h"
#include "peregrine/utils/Preprocessors.h"

#include <cstddef>
#include <cstdint>


namespace pmm
{
    /**
     * @addtogroup PMM_Stack
     * @{
     */

    // TODO: Use policy based allocation
    // TODO: Add a custom sizer that resizes footprint based on user preference. PAllocSize16?
    /**
     * @brief Header for storing *minimal* information about a stack entry.
     *
     * @details Minimizes footprint only allocating space for padding.
     * Uses std::size_t on the premise that default alignment is 8-bytes(enough padding to hold the header)
     * on a 64-bit system and SIMD uses at least 16-byte padding(SSE).
     *
     * @relatedalso SafeStackHeader
     */
    struct MinStackHeader
    {
        std::size_t padding;
    };


    /**
     * @brief Header for storing information about a stack entry.
     *
     * @details
     * Prioritizes stack behavior over memory footprint, consuming around 16-bytes(on a 64-bit system) per allocation.
     *
     * @relatedalso MinStackHeader
     */
    struct SafeStackHeader
    {
        std::size_t previousOffset{}; /// Offset of previous allocated block.
        std::size_t blockSize{};      /// Target allocation's block size.
    };

    template <StoragePolicy Policy = MinimalStackPolicy>
    class Stack
    {
    public:
        /**
         * @brief Allocate a new physical memory vault from the Operating System.
         *
         * @note TODO: When telemetry is enabled, allocates a Telemetry instance on the **Heap**.
         *
         * @param[in] sizeInBytes The total capacity of the stack in bytes.
         *
         * @warning The memory block is NOT zero-initialized.
         * @warning This allocator is Linear and is NOT thread-safe by default.
         */
        [[nodiscard]] explicit Stack(std::size_t sizeInBytes) noexcept;

        /**
         * @brief Get the total capacity in bytes of the stack.
         * @return The total capacity of the stack.
         */
        [[nodiscard]] constexpr std::size_t size() const noexcept;


        /**
         * @brief Allocate @p size bytes of memory on the stack.
         *
         * @param[in] size      Number of bytes to allocate.
         * @param[in] alignment Base alignment of the allocation.
         *                       Default: 8-bytes on 64-bit machine.
         *
         * @return A `void pointer` to starting memory address of the allocation.
         */
        [[nodiscard]] void* alloc(std::size_t size, std::size_t alignment = sizeof(void*)) noexcept
            requires std::same_as<Policy, MinimalStackPolicy>;

        // TODO: Add test
        // TODO: Add implementation
        // TODO: Add warning to alloc
        /**
         * @brief Free memory from the stack to the @p ptr marker.
         *
         * @note The function will mark the passed-in pointer as `nullptr`,
         *       but will not mark any intermediate pointers as `nullptr`.
         *
         * @warning Does not check for invalid states including out-of-bounds and `nullptr` free in *Release Mode*.
         *
         * @param[in,out] ptr The pointer to free upto.
         */
        void free(void* ptr) noexcept
            requires std::same_as<Policy, MinimalStackPolicy>;


        // TODO: Implementation (SPLIT OUT to Deque)
        // /**
        //  * @brief Allocate @p size bytes of memory at the back of the stack.
        //  *
        //  * @param[in] size: Number of bytes to allocate.
        //  * @param[in] alignment: Base alignment of the allocation.
        //  *
        //  * @return A `void pointer` to starting memory address of the allocation.
        //  */
        // [[nodiscard]] constexpr void* allocBack(std::size_t size, std::size_t alignment = sizeof(void*));

        /**
         * @brief Stack Destructor. Frees all memory held.
         *
         * @note For clearing the Arena, use @ref freeAll, or to move free individual frames use @ref free.
         */
        ~Stack() noexcept;


    private:
        /**
         * @brief Calculate the padding required for a given alignment for rebasing the offset.
         *
         * @param alignment The alignment to snap the ptr to.
         *
         * @return The padding required for alignment.
         */
        std::size_t _calcAlignment(std::size_t alignment) noexcept;

    private:
        uint8_t* _buffer;
        std::size_t _size, _offset;



#ifdef ENABLE_PMM_TESTS
    // FRIEND TEST macros for verifying internal states
    #include <gtest/gtest_prod.h>




        FRIEND_TEST(StackInitialization, InitializesDefaultStateAndBuffer);
        FRIEND_TEST(StackAllocation, MovesOffsetAtleastByAllocationSize);
#endif
    };

    /** @} */

} // namespace pmm


#include "Stack.tpp"
