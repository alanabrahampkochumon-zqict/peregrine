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
        void* allocBytes(std::size_t bytes, std::size_t alignment = sizeof(void*));


        /**
         * @brief Allocate an object of type @p T in the arena.
         * @note The memory alignment is dictated by the arena's alignment which can be set upon instantiation.
         *
         * @tparam T    The type of object to allocate.
         * @tparam Args The type of arguments to instantiate the object.
         *
         * @param args The arguments to instantiate the object.
         *
         * @return A reference to the allocated memory.
         */
        template <typename T, typename... Args>
        constexpr T* alloc(Args... args);

        // template <typename T>
        // constexpr T* alloc();

        // template <typename T>
        // constexpr T* alloc(std::size_t alignment);

        // template <typename T>
        // constexpr void free(T* ptr) {}

        // constexpr void free(void* ptr) {}

        // constexpr void freeAll();

        // TODO: Add resize
        // TODO: Add temp arena
        // TODO: Add namespace based new allocation eg: namespace arena { Mat3 mat = new Mat3(); // Uses arena new not
        // C++ heap}

    private:
        uint8_t* _buffer;
        uint64_t _sizeInBytes, _offset;

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
        FRIEND_TEST(ArenaMoveAssignment, DeletingOriginalArenaDoNotDeleteTheNewArenasMemory);
    };

} // namespace pmm


#include "Arena.tpp"
