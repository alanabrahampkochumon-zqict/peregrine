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


#include <cstddef>
#include <cstdint>

#include "gtest/gtest_prod.h"


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
         * @brief Move the current Arena instance to a new object.
         *
         * @param[in/out] arena The arena to move into the new object.
         */
        Arena(Arena&& arena) noexcept;


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

        //constexpr void* allocBytes(std::size_t bytes, std::size_t alignment);

        //constexpr void* allocBytes(std::size_t bytes);

        //template <typename T, typename... Args>
        //constexpr T* alloc(Args... args);

        //template<typename T, typename... Args>
        //constexpr T* alloc(std::size_t alignment, Args... args);

        //template <typename T>
        //constexpr T* alloc();

        //template <typename T>
        //constexpr T* alloc(std::size_t alignment);

        //template <typename T>
        //constexpr void free(T* ptr) {}

        //constexpr void free(void* ptr) {}

        //constexpr void freeAll();

        // TODO: Add resize
        // TODO: Add temp arena

    private:
        uint8_t* _buffer;
        uint64_t _sizeInBytes, _offset;

        // FRIEND TEST macros for verifying internal states
        FRIEND_TEST(ArenaMoveConstructor, NullsOutInternalBuffer);
        FRIEND_TEST(ArenaMoveConstructor, MovesBufferIntoNewObject);
    };


    
} // namespace pmm

#include "Arena.tpp"