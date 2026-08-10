#pragma once
/**
 * @file Arena.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Arena/Linear Memory Allocator.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Policy.h"
#include "peregrine/telemetry/ArenaTelemetry.h"
#include "peregrine/utils/Preprocessors.h"

#include <cstdint>
#include <span>


namespace pmm
{

    /**
     * @addtogroup PMM_Arena
     * @{
     */

    /**
     *  @brief Linear memory allocator.
     *
     *  @tparam MemStrategy Memory management type. See @ref pmm::MemoryStrategy.
     *  @tparam TelPolicy   Flag indicating whether or not telemetry is enabled for this arena. See @ref pmm::telemetry.
     *  @tparam Safe        Flags an arena as safe, implying certain operations like resizing a `nullptr` are handled
     *                      gracefully when assertions are disabled. `False` by default to prevent any performance
     *                      stalls incurred by conditional checks.
     */
    template <MemoryStrategy MemStrategy = ManagedMemory, telemetry::TelemetryPolicy TelPolicy = telemetry::Enabled,
              bool Safe = false>
    struct Arena
    {

        /**
         * @brief Allocate a new physical memory vault from the Operating System.
         *
         * @note When telemetry is enabled, allocates a Telemetry instance on the **Heap**.
         * @note The memory block is zero-initialized.
         *
         * @param[in] arenaSize The total capacity of the arena in bytes.
         *
         * @warning This allocator is Linear and is NOT thread-safe by default.
         *
         * @remarks API specialized for @ref pmm::ManagedMemory.
         */
        constexpr explicit Arena(size_t arenaSize) noexcept
            requires std::same_as<MemStrategy, ManagedMemory>;


        /**
         * @brief Allocate a new physical memory vault from the Operating System.
         *
         * @note When telemetry is enabled, allocates a Telemetry instance on the **Heap**.
         *
         * @param[in,out] backingBuffer The memory buffer to be used by the allocator.
         * @param[in] arenaSize         The total capacity of the arena in bytes.
         *
         * @warning This allocator is Linear and is NOT thread-safe by default.
         *
         * @remarks API specialized for @ref pmm::UnmanagedMemory.
         */
        constexpr explicit Arena(void* backingBuffer, size_t arenaSize) noexcept
            requires std::same_as<MemStrategy, UnmanagedMemory>;


        /**
         * @brief Destroy Arena, freeing up any memory it holds.
         *
         * @note For clearing the Arena, use @ref clear.
         *
         * @remarks API specialized for @ref pmm::ManagedMemory.
         */
        constexpr ~Arena() noexcept
            requires std::same_as<MemStrategy, ManagedMemory>;


        /**
         * @brief Arena Destructor.
         * @note User must free the buffer they provided.
         *
         * @remarks API specialized for @ref pmm::UnmanagedMemory.
         */
        constexpr ~Arena() noexcept
            requires std::same_as<MemStrategy, UnmanagedMemory>
        = default;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the arena.
         */
        constexpr Arena(const Arena&) = delete;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the arena.
         */
        constexpr Arena& operator=(const Arena&) = delete;


        /**
         * @brief Transfer the current Arena's memory to a new object.
         *
         * @param[in,out] arena The arena to move into the new object.
         */
        constexpr Arena(Arena&& arena) noexcept;


        /**
         * @brief Transfer the current Arena's memory to another object.
         *
         * @warning This will delete any buffers held by the LHS object.
         *
         * @param[in,out] arena The arena to move into the object.
         *
         * @return The current arena instance.
         */
        constexpr Arena& operator=(Arena&& arena) noexcept;


        /**
         * @brief Get the free capacity in bytes of the arena.
         * @return The free capacity of the arena in bytes.
         */
        [[nodiscard]] constexpr std::size_t freeSize() const noexcept;


        /**
         * @brief Get the used capacity in bytes of the arena.
         * @return The used capacity of the arena in bytes.
         */
        [[nodiscard]] constexpr std::size_t usedSize() const noexcept;


        /**
         * @brief Get the total capacity in bytes of the arena.
         * @return The total capacity of the arena.
         */
        [[nodiscard]] constexpr std::size_t size() const noexcept;


        /**
         * @brief Allocate @p bytes of memory.
         *
         * @param sizeInBytes     The memory in bytes to allocate from the Arena.
         * @param alignment The alignment to use when allocating memory (in bytes).
         *                  Defaults to sizeof(void*) which is 8 bytes in 64-bit machines.
         *
         * @warning Can cause internal fragmentation, when aligning ill-aligned values.
         * @warning Does not check for free space availability in *Release Mode*. // TODO
         * @warning The memory block may NOT be zero-initialized.
         *
         * @return A void pointer to the start of allocated memory or
         *         `nullptr` if the arena cannot allocate memory of requested size.
         */
        [[nodiscard]] void* allocBytes(std::size_t sizeInBytes, std::size_t alignment = sizeof(void*)) noexcept;


        /**
         * @brief Allocate an object of type @p T in the arena with natural alignment.
         *
         * @note Memory gets aligned to the default of alignment of @p T.
         *       For finer control use @ref allocBytes or @ref allocAs.
         *
         * @tparam T    The type of object to allocate.
         * @tparam Args The type of arguments to instantiate the object.
         *
         * @param args The arguments to instantiate the object.
         *
         * @return A reference to the allocated memory.
         */
        template <typename T, typename... Args>
        [[nodiscard]] T* alloc(Args... args) noexcept;


        /**
         * @brief Allocate an object of type @p T in the arena with @p alignment.
         *
         * @tparam T    The type of object to allocate.
         * @tparam Args The type of arguments to instantiate the object.
         *
         * @param alignment The byte alignment of the object.
         * @param args      The arguments to instantiate the object.
         *
         * @return A reference to the allocated memory.
         */
        template <typename T, typename... Args>
        [[nodiscard]] T* allocAs(std::size_t alignment, Args... args) noexcept;


        /**
         * @brief Allocate a contiguous memory block for an array of @p count objects.
         *
         * @note This function allocates raw, uninitialized memory aligned to type @p T.
         *       Object constructors are NOT called automatically. You must manually construct
         *       the objects in the returned memory (e.g., using placement-new or `std::uninitialized_fill`).
         *
         * @tparam T The type of object to allocate.
         *
         * @param count The total number of contiguous elements requested.
         *
         * @return A `std::span<T>` viewing the allocated memory block.
         *         Returns an empty span (`.empty() == true`) if the Arena lacks sufficient capacity.
         */
        template <typename T>
        [[nodiscard]] std::span<T> allocV(std::size_t count) noexcept;


        /**
         * @brief [NO-OP] Objects cannot be individually freed in an arena.
         *
         * @tparam T  The type of allocated object.
         *
         * @param ptr The pointer to the allocated object.
         */
        template <typename T>
        void free(T* ptr) = delete;


        /**
         * @brief [NO-OP] Byte memory cannot be individually freed in an arena.
         *
         * @tparam T  The type of allocated object.
         *
         * @param ptr The pointer to the allocated object.
         */
        template <typename T>
        void free(void* ptr) = delete;


        /**
         * @brief Reset the entire arena.
         *
         * @note This is not a hard reset.
         *       All memory states may/may not get erased.
         *
         */
        void clear() noexcept;


        /**
         * @brief Zero out the arena's buffer.
         *
         * @warning This will completely overwrite the arena entire buffer with zeroes. Only call this method if you
         *          want a zero-ed out arena after clearing.
         */
        void zeroOut() const noexcept;


        /**
         * @brief Resize @p oldMemory block from @p oldSize to @p newSize.
         *
         * @note This does not resize the arena.
         *
         * @param oldMemory The pointer to the memory to resize.
         * @param oldSize   The current size of the @p oldMemory.
         * @param newSize   The size to resize @p oldMemory to.
         * @param alignment The byte alignment of the @p oldMemory.
         *                  Default: 8 bytes on a 64-bit machine.
         *
         * @return A reference to the new memory location in arena or nullptr if allocation fails.
         */
        [[nodiscard]] void* resize(void* oldMemory, std::size_t oldSize, std::size_t newSize,
                                   std::size_t alignment = sizeof(void*)) noexcept;


        /**
         * @brief Get the telemetry instance associated with this arena.
         * @return A const reference to the @ref ArenaTelemetry instance.
         */
        [[nodiscard]] constexpr const ArenaTelemetryType<TelPolicy>& getTelemetry() const noexcept;


    private:
        uint8_t* _buffer;
        uint64_t _sizeInBytes, _offset, _prevOffset;
        PMM_NO_UNIQUE_ADDR ArenaTelemetryType<TelPolicy> _telemetry{ 0 };

        /**
         * @brief Align the internal buffer to @p alignment.
         * @param alignment The memory alignment to align the buffer to.
         */
        void _alignForward(std::size_t alignment) noexcept;

        // For internal variable access
        friend struct TempArena;

#ifdef ENABLE_PMM_TESTS
    // FRIEND TEST macros for verifying internal states
    #include <gtest/gtest_prod.h>




        FRIEND_TEST(ManagedArenaTests, MoveCtor_ClearsMovedArena);
        FRIEND_TEST(ManagedArenaTests, MoveCtor_MovesBufferIntoNewObject);
        FRIEND_TEST(ManagedArenaTests, MoveAssign_ClearsMovedArena);
        FRIEND_TEST(ManagedArenaTests, MoveAssign_MovesBufferIntoNewObject);
        FRIEND_TEST(ManagedArenaTests, MoveAssign_SelfAssignmentReturnsTheSameArena);
        FRIEND_TEST(ManagedArenaTests, MoveAssign_DeletingOriginalArenaDoNotDeleteTheNewArenasMemory);
        FRIEND_TEST(ManagedArenaTests, AllocBytes_MovesPrevOffset);
        FRIEND_TEST(ManagedArenaTests, Alloc_MovesPrevOffset);
        FRIEND_TEST(ManagedArenaTests, AllocBytes_UpdatesTelemetryPadding);
        FRIEND_TEST(ManagedArenaTests, Alloc_UpdatesTelemetryPadding);
        FRIEND_TEST(ManagedArenaTests, AllocV_UpdatesTelemetryPadding);
        FRIEND_TEST(ManagedArenaTests, Resize_LatestAllocationResizeBuffer);
        FRIEND_TEST(ManagedArenaTests, Resize_LatestAllocationOnlyResizeByOffsetDifference);
        FRIEND_TEST(ManagedArenaTests, Clear_ResetsOffsetToZero);
        FRIEND_TEST(ManagedArenaTests, ZeroOut_ZeroesOutTheInternalBuffer);

        FRIEND_TEST(UnmanagedArenaTests, MoveCtor_ClearsMovedArena);
        FRIEND_TEST(UnmanagedArenaTests, MoveCtor_MovesBufferIntoNewObject);
        FRIEND_TEST(UnmanagedArenaTests, MoveAssign_ClearsMovedArena);
        FRIEND_TEST(UnmanagedArenaTests, MoveAssign_MovesBufferIntoNewObject);
        FRIEND_TEST(UnmanagedArenaTests, MoveAssign_SelfAssignmentReturnsTheSameArena);
        FRIEND_TEST(UnmanagedArenaTests, MoveAssign_DeletingOriginalArenaDoNotDeleteTheNewArenasMemory);
        FRIEND_TEST(UnmanagedArenaTests, AllocBytes_MovesPrevOffset);
        FRIEND_TEST(UnmanagedArenaTests, Alloc_MovesPrevOffset);
        FRIEND_TEST(UnmanagedArenaTests, AllocBytes_UpdatesTelemetryPadding);
        FRIEND_TEST(UnmanagedArenaTests, Alloc_UpdatesTelemetryPadding);
        FRIEND_TEST(UnmanagedArenaTests, AllocV_UpdatesTelemetryPadding);
        FRIEND_TEST(UnmanagedArenaTests, Resize_LatestAllocationResizeBuffer);
        FRIEND_TEST(UnmanagedArenaTests, Resize_LatestAllocationOnlyResizeByOffsetDifference);
        FRIEND_TEST(UnmanagedArenaTests, Clear_ResetsOffsetToZero);
#endif
    };

    /** @} */

} // namespace pmm


#include "Arena.tpp"
