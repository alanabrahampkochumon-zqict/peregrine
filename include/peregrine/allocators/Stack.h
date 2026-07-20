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


#include "StackType.h"
#include "peregrine/utils/Utilities.h"
#include "peregrine/utils/Preprocessors.h"

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
     *          Uses std::size_t on the premise that default alignment is 8-bytes(enough padding to hold the header)
     *          on a 64-bit system and SIMD uses at least 16-byte padding(SSE).
     *
     * @relatedalso StrictStackHeader
     */
    struct LooseStackHeader
    {
        std::size_t padding;
    };


    /**
     * @brief Header for storing information about a stack entry.
     *
     * @details
     * Prioritizes stack behavior over memory footprint, consuming around 16-bytes(on a 64-bit system) per allocation.
     *
     * @relatedalso LooseStackHeader
     */
    struct StrictStackHeader
    {
        std::size_t previousOffset{}; /// Offset of previous allocated block.
        std::size_t padding{};        /// Target allocation's block size.
    };


    template <stack::StackType Type = stack::Loose>
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
         * @brief Get the free capacity in bytes of the stack.
         * @return The free capacity of the stack in bytes.
         */
        [[nodiscard]] constexpr std::size_t freeSize() const noexcept;


        /**
         * @brief Get the used capacity in bytes of the stack.
         * @return The used capacity of the stack in bytes.
         */
        [[nodiscard]] constexpr std::size_t usedSize() const noexcept;



        /**
         * @brief Allocate @p size bytes of memory on the stack.
         *
         * @warning Does not check for invalid states in *Release Mode*.
         *
         * @param[in] size      Number of bytes to allocate.
         * @param[in] alignment Base alignment of the allocation.
         *                      Default: 8-bytes on 64-bit machine.
         *
         * @return A `void pointer` to starting memory address of the allocation.
         *
         * @remarks API specialized for @ref pmm::stack::Loose.
         *
         * @relatedalso alloc
         * @relatedalso allocV
         */
        [[nodiscard]] void* allocBytes(std::size_t size, std::size_t alignment = sizeof(void*)) noexcept
            requires std::same_as<Type, stack::Loose>;

        /**
         * @brief Allocate @p size bytes of memory on the stack.
         *
         * @warning Does not check for invalid states in *Release Mode*.
         *
         * @param[in] size      Number of bytes to allocate.
         * @param[in] alignment Base alignment of the allocation.
         *                      Default: 8-bytes on 64-bit machine.
         *
         * @return A `void pointer` to starting memory address of the allocation.
         *
         * @remarks API specialized for @ref pmm::stack::Strict.
         *
         * @relatedalso alloc
         * @relatedalso allocV
         */
        [[nodiscard]] void* allocBytes(std::size_t size, std::size_t alignment = sizeof(void*)) noexcept
            requires std::same_as<Type, stack::Strict>;


        /**
         * @brief Allocate an object of type @p T in the stack and initialize it with @p args.
         *
         * @note The object will be aligned to the default alignment of @p T.
         * @note T must be aligned to a power of 2.
         *
         * @tparam T    The type of object to allocate.
         * @tparam Args The type of arguments to instantiate the object.
         *
         * @param[in] args      The arguments to instantiate the object.
         *
         * @return A reference to the allocated memory.
         *
         * @relatedalso allocBytes
         * @relatedalso allocV
         */
        template <typename T, typename... Args>
        [[nodiscard]] T* alloc(Args... args) noexcept;


        /**
         * @brief Allocate a contiguous memory block for an array of @p count objects.
         *
         * @note This function allocates raw, uninitialized memory aligned to type @p T.
         *       Object constructors are NOT called automatically. You must manually construct
         *       the objects in the returned memory (e.g., using placement-new or `std::uninitialized_fill`).
         * @note T must be aligned to a power of 2.
         *
         * @tparam T The type of object to allocate.
         *
         * @param[in] count The total number of contiguous elements requested.
         *
         * @return A `std::span<T>` viewing the allocated memory block.
         *         Returns an empty span (`.empty() == true`) if the Arena lacks sufficient capacity.
         *
         * @relatedalso allocBytes
         * @relatedalso alloc
         */
        template <typename T>
        [[nodiscard]] constexpr std::span<T> allocV(std::size_t count) noexcept;


        /**
         * @brief Resize @p oldMemory block from @p oldSize to @p newSize while minimizing fragmentation.
         *
         * @note Stack will not be resized.
         * @note Passing `0` as @p newSize will not deallocate memory, and is undefined behavior in release mode.
         * @note Slower compared to @ref resizeFast(which doesn't optimize memory footprint) and @ref resizeLast,
         *       which should only be used for latest allocations.
         *
         * @warning In **Release Mode** safety checks for nullptr, and 0 sizes are disabled.
         *
         * @param[in] oldMemory The pointer to the memory to resize.
         * @param[in] oldSize   The current size of @p oldMemory.
         * @param[in] newSize   The size to resize @p oldMemory to.
         * @param[in] alignment The required alignment.
         *                  Default: 8-bytes on 64-bit machine.
         *
         * @return A reference to the new memory location in stack.
         *
         * @relatedalso resizeFast
         * @relatedalso resizeLast
         */
        [[nodiscard]] void* resize(void* oldMemory, std::size_t oldSize, std::size_t newSize,
                                   std::size_t alignment = sizeof(void*));


        /**
         * @brief Resize @p oldMemory block from @p oldSize to @p newSize.
         *
         * @note Stack will not be resized.
         * @note Passing `0` as @p newSize will not deallocate memory, and is undefined behavior in release mode.
         * @note Doesn't optimize memory footprint, as allocation always reserves new memory.
         *       If you want optimal memory usage use @ref resize, or for resizing lastest allocations
         *       without overheads use @ref resizeLast
         *
         * @warning In **Release Mode** safety checks for `nullptr`, and 0 sizes are disabled.
         * @warning Never use this for the latest allocations, as this method bypasses all checks and
         *          allocate a new buffer.
         *
         * @param[in] oldMemory The pointer to the memory to resize.
         * @param[in] oldSize   The current size of @p oldMemory.
         * @param[in] newSize   The size to resize @p oldMemory to.
         * @param[in] alignment The required alignment.
         *                      Default: 8-bytes on 64-bit machine.
         *
         * @return A reference to the new memory location in stack.
         *
         * @relatedalso resize
         * @relatedalso resizeLast
         */
        [[nodiscard]] void* resizeFast(const void* oldMemory, std::size_t oldSize, std::size_t newSize,
                                       std::size_t alignment = sizeof(void*));


        /**
         * @brief Resize the latest allocation, @p oldMemory from @p oldSize to @p newSize.
         *
         * @note Stack will not be resized.
         * @note Passing `0` as @p newSize will not deallocate memory, and is undefined behavior in release mode.
         *
         * @warning In **Release Mode** safety checks for `nullptr`, and 0 sizes are disabled.
         * @warning Recommended to use for latest allocations, otherwise memory corruption can occur.
         *
         * @param[in] oldMemory The pointer to the memory to resize.
         * @param[in] oldSize   The current size of @p oldMemory.
         * @param[in] newSize   The size to resize @p oldMemory to.
         *
         * @return A reference to the new memory location in stack.
         *
         * @relatedalso resize
         * @relatedalso resizeFast
         */
        [[nodiscard]] void* resizeLast(void* oldMemory, std::size_t oldSize, std::size_t newSize);


        /**
         * @brief Free memory from the stack to the @p ptr marker.
         *
         * @note If you are freeing data allocated using @ref alloc or @ref allocV, use @ref alloc and @ref allocV
         *       respectively, as they will call the class destructor for non-trivial types.
         *
         * @warning Does not check for invalid states including out-of-bounds and `nullptr` free in *Release Mode*.
         *
         * @param[in] ptr The pointer to free upto.
         *
         * @relatedalso free
         * @relatedalso freeV
         * @relatedalso freeAll
         */
        void freeBytes(void* ptr) noexcept
            requires std::same_as<Type, stack::Loose>;


        /**
         * @brief Safely free memory allocated with @ref alloc<T>.
         *
         * @note Destructor is called for non-trivially destructible types.
         *
         * @tparam T  The data type of the memory pointer.
         *
         * @param[in] ptr The object pointer to free.
         *
         * @relatedalso freeV
         * @relatedalso freeBytes
         * @relatedalso freeAll
         */
        template <typename T>
        void free(T* ptr) noexcept;


        /**
         * @brief Safely free memory allocated with @ref allocV<T>.
         *
         * @note Destructor is called for non-trivially destructible types.
         *
         * @tparam T  The data type of the memory pointer.
         *
         * @param[in] vector The collection of objects to free.
         *
         * @relatedalso free
         * @relatedalso freeBytes
         * @relatedalso freeAll
         */
        template <typename T>
        void freeV(std::span<T> vector) noexcept;


        /**
         * @brief Free the entire stack, resetting to a fresh state.
         *
         * @relatedalso  free
         * @relatedalso  freeBytes
         */
        void freeAll();


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

        /// Member Variables
        uint8_t* _buffer;
        std::size_t _size, _offset{ 0 };

        using PreviousOffsetType = std::conditional_t<std::is_same_v<Type, stack::Strict>, std::size_t, EmptyMember>;
        PMM_NO_UNIQUE_ADDR PreviousOffsetType _prevOffset{ 0 };



#ifdef ENABLE_PMM_TESTS
    // FRIEND TEST macros for verifying internal states
    #include <gtest/gtest_prod.h>




        FRIEND_TEST(LooseStackInitialization, InitializesDefaultStateAndBuffer);
        FRIEND_TEST(LooseStackTests, Initialization_MovesOffsetAtleastByAllocationSize);
        FRIEND_TEST(LooseStackTests, FreeAll_MovesOffsetToZero);
        FRIEND_TEST(LooseStackResizeLast, ResizeLast_MovesOffsetInCorrectDirection);
#endif
    };

    /** @} */

} // namespace pmm


#include "Stack.tpp"
