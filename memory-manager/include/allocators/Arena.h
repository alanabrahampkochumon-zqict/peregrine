#pragma once
/**
 * @file Arena.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Arena/Linear Memory Allocator
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <cstddef>
#include <cstdint>

namespace pmm
{

    struct Arena
    {
        /**
         * @brief Instantiate an Arena allocator of the given size.
         * @param[in] bytes The byte size of the arena.
         */
        constexpr Arena(std::size_t bytes) noexcept;


        /**
         * @brief Destroy Arena, freeing up any memory it holds.
         * @note For clearing the Arena, use @ref freeAll.
         */
        constexpr ~Arena() noexcept;


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
    };


    
} // namespace pmm

#include "Arena.tpp"