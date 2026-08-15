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


#include "Policy.h"
#include "peregrine/telemetry/StackTelemetry.h"
#include "peregrine/utils/Helpers.h"
#include "peregrine/utils/Preprocessors.h"

#include <cstdint>

// TODO: Add stack safe mode
namespace pmm
{
    /**
     * @addtogroup PMM_Stack
     * @{
     */

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
        std::size_t prevOffset{}; /// Offset of previous allocated block.
        std::size_t padding{};    /// Target allocation's block size.
    };

    /**
     * @brief Linear memory allocator following LIFO principle.
     *
     *
     * @tparam MemStrategy Memory management type. See @ref pmm::MemoryStrategy.
     * @tparam TelPolicy   Flag indicating whether or not telemetry is enabled for this arena. See @ref pmm::telemetry.
     * @tparam Safe        Flags an arena as safe, implying certain operations like resizing a `nullptr` are handled
     *                     gracefully when assertions are disabled. `False` by default to prevent any performance
     *                     stalls incurred by conditional checks.
     */
    template <stack::StackType Type = stack::Loose, MemoryStrategy MemStrategy = ManagedMemory,
              telemetry::TelemetryPolicy TelPolicy = telemetry::Enabled, bool Safe = false>
    class Stack
    {
    public:
        /**
         * @brief Allocate a new physical memory vault from the Operating System.
         *
         * @note The memory block is zero-initialized.
         *
         * @param[in] stackSize The total capacity of the stack in bytes.
         *
         * @remarks API specialized for @ref pmm::ManagedMemory.
         *
         * @warning This allocator is Linear and is NOT thread-safe by default.
         */
        [[nodiscard]] explicit constexpr Stack(std::size_t stackSize) noexcept
            requires std::same_as<MemStrategy, ManagedMemory>;


        /**
         * @brief Allocate a new physical memory vault from the Operating System.
         *
         * @param[in,out] buffer The starting address to the backing buffer.
         * @param[in] bufferSize The size of the backing buffer in bytes, which will directly
         *                       translate into the stack size.
         *
         * @remarks API specialized for @ref pmm::UnmanagedMemory.
         *
         * @warning This allocator is Linear and is NOT thread-safe by default.
         */
        [[nodiscard]] explicit constexpr Stack(uint8_t* buffer, std::size_t bufferSize) noexcept
            requires std::same_as<MemStrategy, UnmanagedMemory>;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the stack.
         */
        constexpr Stack(const Stack&) = delete;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the stack.
         */
        constexpr Stack& operator=(const Stack&) = delete;


        /**
         * @brief Transfer the current Stack's memory to a new object.
         *
         * @param[in,out] stack The stack to move into the new object.
         */
        constexpr Stack(Stack&& stack) noexcept;


        /**
         * @brief Transfer the current Stack's memory to another object.
         *
         * @warning This will delete any buffers held by the LHS object.
         *
         * @param[in,out] stack The stack to move into the object.
         *
         * @return The current stack instance.
         */
        constexpr Stack& operator=(Stack&& stack) noexcept;


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
         * @brief Return whether telemetry is enabled for this stack.
         */
        [[nodiscard]] static constexpr bool isTelemetryEnabled() noexcept;


        /**
         * @brief Get the telemetry instance.
         *
         * @return A telemetry instance if telemetry policy is not Disabled, else an empty struct.
         */
        [[nodiscard]] constexpr const StackTelemetryType<TelPolicy>& getTelemetry() const noexcept;


        /**
         * @brief Allocate @p size bytes of memory on the stack.
         *
         * @warning Does not check for invalid states in *Release Mode*.
         * @warning The memory block may NOT be zero-initialized.
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
         * @warning The memory block may NOT be zero-initialized.
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
         *         Returns an empty span (`.empty() == true`) if the Stack lacks sufficient capacity.
         *
         * @relatedalso allocBytes
         * @relatedalso alloc
         */
        template <typename T>
        [[nodiscard]] std::span<T> allocV(std::size_t count) noexcept;


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
         *                      Default: 8-bytes on 64-bit machine.
         *
         * @return A reference to the new memory location in stack.
         *
         * @remarks API specialized for @ref pmm::stack::Strict.
         *
         * @relatedalso resizeFast
         * @relatedalso resizeLast
         */
        [[nodiscard]] void* resize(void* oldMemory, std::size_t oldSize, std::size_t newSize,
                                   std::size_t alignment = sizeof(void*))
            requires std::same_as<Type, stack::Loose>;


        /**
         * @brief Resize @p oldMemory block from @p oldSize to @p newSize while minimizing fragmentation.
         *
         * @note Stack will not be resized.
         * @note Passing `0` as @p newSize will not deallocate memory, and is undefined behavior in release mode.
         * @note Allocations are categorized(Latest, Resizing to smaller size, etc.) to minimize internal fragmentation
         *       but can result in slower resizes.
         *
         * @warning In **Release Mode** safety checks for nullptr, and 0 sizes are disabled.
         *
         * @param[in] oldMemory The pointer to the memory to resize.
         * @param[in] oldSize   The current size of @p oldMemory.
         * @param[in] newSize   The size to resize @p oldMemory to.
         * @param[in] alignment The required alignment.
         *                      Default: 8-bytes on 64-bit machine.
         *
         * @return A reference to the new memory location in stack.
         *
         * @remarks API specialized for @ref pmm::stack::Strict.
         *
         * @relatedalso resizeFast
         * @relatedalso resizeLast
         */
        [[nodiscard]] void* resize(void* oldMemory, std::size_t oldSize, std::size_t newSize,
                                   std::size_t alignment = sizeof(void*))
            requires std::same_as<Type, stack::Strict>;


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
         * @warning Use only on latest allocations.
         *
         * @param[in] oldMemory The pointer to the memory to resize.
         * @param[in] oldSize   The current size of @p oldMemory.
         * @param[in] newSize   The size to resize @p oldMemory to.
         *
         * @return A reference to the passed-in memory location.
         *
         * @remarks API specialized for @ref pmm::stack::Loose.
         *
         * @relatedalso resize
         * @relatedalso resizeFast
         */
        [[nodiscard]] void* resizeLast(void* oldMemory, std::size_t oldSize, std::size_t newSize)
            requires std::same_as<Type, stack::Loose>;


        /**
         * @brief Resize the latest allocation, @p oldMemory from @p oldSize to @p newSize.
         *
         * @note Stack will not be resized.
         * @note Passing `0` as @p newSize will not deallocate memory, and is undefined behavior in release mode.
         * @note Performs assertion in *Debug Mode* to ensure that the latest allocation is being resized.
         *
         * @warning In **Release Mode** safety checks for `nullptr`, and 0 sizes are disabled.
         * @warning Use only on latest allocations, otherwise memory corruption can occur.
         *
         * @param[in] oldMemory The pointer to the memory to resize.
         * @param[in] oldSize   The current size of @p oldMemory.
         * @param[in] newSize   The size to resize @p oldMemory to.
         *
         * @return A reference to the passed-in memory location.
         *
         * @remarks API specialized for @ref pmm::stack::Strict.
         *
         * @relatedalso resize
         * @relatedalso resizeFast
         */
        [[nodiscard]] void* resizeLast(void* oldMemory, std::size_t oldSize, std::size_t newSize)
            requires std::same_as<Type, stack::Strict>;


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
         * @remarks API specialized for @ref pmm::stack::Loose.
         *
         * @relatedalso free
         * @relatedalso freeV
         * @relatedalso clear
         */
        void freeBytes(void* ptr) noexcept
            requires std::same_as<Type, stack::Loose>;


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
         * @remarks API specialized for @ref pmm::stack::Strict.
         *
         * @relatedalso free
         * @relatedalso freeV
         * @relatedalso clear
         */
        void freeBytes(void* ptr) noexcept
            requires std::same_as<Type, stack::Strict>;


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
         * @relatedalso clear
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
         * @relatedalso clear
         */
        template <typename T>
        void freeV(std::span<T> vector) noexcept;


        /**
         * @brief Free the entire stack, resetting to a fresh state.
         *
         * @relatedalso  free
         * @relatedalso  freeBytes
         */
        void clear();


        /**
         * @brief Zero out the stack's buffer.
         *
         * @warning This will completely overwrite the stack entire buffer with zeroes. Only call this method if you
         *          want a zero-ed out stack after clearing.
         */
        void zeroOut() const noexcept;


        /**
         * @brief Stack Destructor. Free the internal buffer.
         *
         * @note For clearing the Stack, use @ref clear, or to move free individual frames use @ref free.
         *
         * @remarks API specialized for @ref pmm::ManagedMemory.
         */
        ~Stack() noexcept
            requires std::same_as<MemStrategy, ManagedMemory>;


        /**
         * @brief Stack Destructor.
         *
         * @note For clearing the Stack, use @ref clear, or to move free individual frames use @ref free.
         *
         * @warning Will not clear free the backing buffer since its managed by the user.
         *
         * @remarks API specialized for @ref pmm::UnmanagedMemory.
         */
        ~Stack() noexcept
            requires std::same_as<MemStrategy, UnmanagedMemory>
        = default;


    private:
        /**
         * @brief Calculate the padding required for a given alignment for rebasing the offset.
         *
         * @param alignment The alignment to snap the ptr to.
         *
         * @return The padding required for alignment.
         */
        constexpr std::size_t _calcAlignment(std::size_t alignment) noexcept;


        /// Custom Types
        using PreviousOffsetType = std::conditional_t<std::is_same_v<Type, stack::Strict>, std::size_t, EmptyMember>;

        /// Member Variables
        uint8_t* _buffer;
        std::size_t _stackSize, _offset{ 0 };
        PMM_NO_UNIQUE_ADDR PreviousOffsetType _prevOffset;
        PMM_NO_UNIQUE_ADDR StackTelemetryType<TelPolicy> _telemetry;



#ifdef ENABLE_PMM_TESTS
    // FRIEND TEST macros for verifying internal states
    #include <gtest/gtest_prod.h>




        FRIEND_TEST(StackMemoryManagementTests, LooseUnmanagedStack_UsesExternalBuffer);
        FRIEND_TEST(StackMemoryManagementTests, StrictUnmanagedStack_UsesExternalBuffer);

        FRIEND_TEST(StrictStackInitializationTests, InitializesDefaultStateAndBuffer);
        FRIEND_TEST(StrictStackTests, Initialization_MovesOffsetAtleastByAllocationSize);
        FRIEND_TEST(StrictStackTests, Clear_MovesOffsetAndPreviousOffsetToZero);
        FRIEND_TEST(StrictStackResizeLastTests, ResizeLast_MovesOffsetInCorrectDirection);
        FRIEND_TEST(StrictStackTests, MoveCtor_NullsOutInternalBuffer);
        FRIEND_TEST(StrictStackTests, MoveCtor_MovesBufferIntoNewObject);
        FRIEND_TEST(StrictStackTests, MoveOperator_NullsOutInternalBuffer);
        FRIEND_TEST(StrictStackTests, MoveOperator_MovesBufferIntoNewObject);
        FRIEND_TEST(StrictStackTests, MoveOperator_SelfAssignmentReturnsTheSameStack);
        FRIEND_TEST(StrictStackTests, MoveOperator_DeletingOriginalStackDoNotDeleteTheNewStacksMemory);
        FRIEND_TEST(StrictStackTests, ZeroOut_ZeroesOutTheInternalBuffer);


        FRIEND_TEST(LooseStackInitializationTests, InitializesDefaultStateAndBuffer);
        FRIEND_TEST(LooseStackTests, Initialization_MovesOffsetAtleastByAllocationSize);
        FRIEND_TEST(LooseStackTests, Clear_MovesOffsetToZero);
        FRIEND_TEST(LooseStackResizeLastTests, ResizeLast_MovesOffsetInCorrectDirection);
        FRIEND_TEST(LooseStackTests, MoveCtor_NullsOutInternalBuffer);
        FRIEND_TEST(LooseStackTests, MoveCtor_MovesBufferIntoNewObject);
        FRIEND_TEST(LooseStackTests, MoveOperator_NullsOutInternalBuffer);
        FRIEND_TEST(LooseStackTests, MoveOperator_MovesBufferIntoNewObject);
        FRIEND_TEST(LooseStackTests, MoveOperator_SelfAssignmentReturnsTheSameStack);
        FRIEND_TEST(LooseStackTests, MoveOperator_DeletingOriginalStackDoNotDeleteTheNewStacksMemory);
        FRIEND_TEST(LooseStackTests, ZeroOut_ZeroesOutTheInternalBuffer);
#endif
    };
} // namespace pmm


#include "Stack.tpp"
