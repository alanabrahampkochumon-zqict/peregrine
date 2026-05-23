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


#include "gtest/gtest_prod.h"
#include <cstddef>
#include <cstdint>


namespace pmm
{
    struct Arena
    {
        /**
         * @brief Allocate a new physical memory vault from the Operating System.
         *
         * @param[in] bytes The total capacity of the arena in bytes.
         *
         * @warning The memory block is NOT zero-initialized.
         * @warning This allocator is Linear and is NOT thread-safe by default.
         */
        inline explicit Arena(std::size_t bytes) noexcept;


        /**
         * @brief Allocate a new physical memory vault from the Operating System with a base alignment of @p alignment.
         *
         * @param[in] bytes     The total capacity of the arena in bytes.
         * @param[in] alignment The base alignment of the arena.
         *                      Must be a power of 2.
         *
         * @warning The memory block is NOT zero-initialized.
         * @warning This allocator is Linear and is NOT thread-safe by default.
         */
        inline explicit Arena(std::size_t bytes, std::size_t alignment) noexcept;


        /**
         * @brief Destroy Arena, freeing up any memory it holds.
         * @note For clearing the Arena, use @ref freeAll.
         */
        inline ~Arena() noexcept;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the arena.
         */
        Arena(const Arena&) = delete;


        /**
         * @brief Copying is strictly prohibited to prevent double-free crashes.
         * @note Use std::move() to transfer ownership of the arena.
         */
        Arena& operator=(const Arena&) = delete;


        /**
         * @brief Transfer the current Arena's memory to a new object.
         *
         * @param[in/out] arena The arena to move into the new object.
         */
        Arena(Arena&& arena) noexcept;


        /**
         * @brief Transfer the current Arena's memory to another object.
         *
         * @warning This will delete any buffers held by the LHS object.
         *
         * @param[in/out] arena The arena to move into the object.
         *
         * @return The current arena instance.
         */
        Arena& operator=(Arena&& arena) noexcept;


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
         * @param bytes     The memory in bytes to allocate from the Arena.
         * @param alignment The alignment to use when allocating memory (in bytes).
         *                  Defaults to sizeof(void*) which is 8 bytes in 64-bit machines.
         *
         * @warning Can cause internal fragmentation, when aligning ill-aligned values.
         *
         * @return A void pointer to the start of allocated memory or
         *         `nullptr` if the arena cannot allocate memory of requested size.
         */
        [[nodiscard]] void* allocBytes(std::size_t bytes, std::size_t alignment = sizeof(void*)) noexcept;


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
        [[nodiscard]] constexpr T* alloc(Args... args) noexcept;


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
        [[nodiscard]] constexpr T* allocAs(std::size_t alignment, Args... args) noexcept;


        /**
         * @brief [NO-OP] Objects cannot be individually freed in an arena.
         *
         * @tparam T  The type of allocated object.
         *
         * @param ptr The pointer to the allocated object.
         */
        template <typename T>
        constexpr void free(T* ptr) = delete;


        /**
         * @brief [NO-OP] Byte memory cannot be individually freed in an arena.
         *
         * @tparam T  The type of allocated object.
         *
         * @param ptr The pointer to the allocated object.
         */
        template <typename T>
        constexpr void free(void* ptr) = delete;


        /**
         * @brief Free the entire arena.
         *
         * @note This is not a hard reset.
         *       All memory states may/may not get erased.
         *
         */
        constexpr void freeAll() noexcept;


        /**
         * @brief Resize @p oldMemory block from @p oldSize to @p newSize.
         *
         * @note This does not resize the arena.
         *
         * @param oldMemory The pointer to the memory to resize.
         * @param oldSize   The current size of the @p oldMemory.
         * @param newSize   The size of resize @p oldMemory to.
         * @param alignment The byte alignment of the @p oldMemory.
         *
         * @return A reference to the new memory location in arena or nullptr if allocation fails.
         */
        [[nodiscard]] constexpr void* resize(void* oldMemory, std::size_t oldSize, std::size_t newSize,
                              std::size_t alignment) noexcept;

        // TODO: Add resize
        // TODO: Add temp arena
        // TODO: Add namespace based new allocation eg: namespace arena { Mat3 mat = new Mat3(); // Uses arena new not
        // C++ heap}

    private:
        uint8_t* _buffer;
        uint64_t _sizeInBytes, _offset, _prevOffset, _defaultAlignment;

        /**
         * @brief Align the internal buffer to @p alignment.
         * @param alignment The memory alignment to align the buffer to.
         */
        void _alignForward(std::size_t alignment) noexcept;

        // FRIEND TEST macros for verifying internal states
        FRIEND_TEST(AlignedArenaInitialization, InternalState_AlignBaseOffset);
        FRIEND_TEST(ArenaMoveConstructor, NullsOutInternalBuffer);
        FRIEND_TEST(ArenaMoveConstructor, MovesBufferIntoNewObject);
        FRIEND_TEST(ArenaMoveAssignment, NullsOutInternalBuffer);
        FRIEND_TEST(ArenaMoveAssignment, MovesBufferIntoNewObject);
        FRIEND_TEST(ArenaMoveAssignment, SelfAssignmentReturnsTheSameArena);
        FRIEND_TEST(ArenaMoveConstructor, AlignedArena_MovesBufferIntoNewObject);
        FRIEND_TEST(ArenaMoveAssignment, DeletingOriginalArenaDoNotDeleteTheNewArenasMemory);
        FRIEND_TEST(ArenaAllocBytes, OffsetMinusPrevOffsetGivesObjectSize);
        FRIEND_TEST(ArenaAlloc, AlignsToTargetAlignment);
        FRIEND_TEST(ArenaAlloc, OffsetMinusPrevOffsetGivesObjectSize);
        FRIEND_TEST(ArenaAllocAs, AlignsToGivenAlignment);
        FRIEND_TEST(ArenaAllocAs, OffsetMinusPrevOffsetGivesObjectSize);
        FRIEND_TEST(ArenaFreeAll, ResetsOffsetToZero);
        FRIEND_TEST(ArenaFreeAll, AlignedArena_ResetsOffsetToAlignedAddress);
        FRIEND_TEST(ArenaResize, LatestAllocationResizeBuffer);
        FRIEND_TEST(ArenaResize, LatestAllocationOnlyResizeByOffsetDifference);
        FRIEND_TEST(ArenaResize, AllocationBeforePriorAllocationReturnNewBuffer);
    };

} // namespace pmm


#include "Arena.tpp"
