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


#include <cstddef>
#include <cstdint>


namespace pmm
{
    /**
     * @addtogroup PMM_Stack
     * @{
     */

    /**
     * @brief Header that stores information about each entry in the stack.
     */
    struct StackHeader
    {
        uint8_t padding;
    };


    struct Stack
    {

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
        inline explicit Stack(std::size_t sizeInBytes) noexcept;

        /**
         * @brief Get the total capacity in bytes of the stack.
         * @return The total capacity of the stack.
         */
        [[nodiscard]] constexpr std::size_t size() const noexcept;

    private:
        uint8_t* _buffer;
        std::size_t _size, _offset;



#ifdef ENABLE_PMM_TESTS
    // FRIEND TEST macros for verifying internal states
    #include <gtest/gtest_prod.h>

        FRIEND_TEST(StackInitialization, InitializesDefaultStateAndBuffer);
#endif
    };

    /** @} */

} // namespace pmm


#include "Stack.tpp"
