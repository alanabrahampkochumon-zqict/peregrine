/**
 * @file StackMemoryManagementTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 22, 2026
 *
 * @brief Verifies stack's memory managment policy.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */



#include <gtest/gtest.h>
#include <peregrine/allocators/Stack.h>


/** @brief Verify that managed stack does not free any memory. */
namespace
{
    // Since we cant really confirm is a buffer is freed
    // And we only delete[] buffer in the dtor of Stack, we can check if its trivially destructible
    // to ensure memory is freed in the stack in unmanaged mode and opposite otherwise
    static_assert(std::is_trivially_destructible_v<pmm::Stack<pmm::stack::Loose, pmm::UnmanagedMemory>> == true);
    static_assert(std::is_trivially_destructible_v<pmm::Stack<pmm::stack::Strict, pmm::UnmanagedMemory>> == true);

    static_assert(std::is_trivially_destructible_v<pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory>> == false);
    static_assert(std::is_trivially_destructible_v<pmm::Stack<pmm::stack::Strict, pmm::ManagedMemory>> == false);

} // namespace



/** @brief Verify that managed stack maintains its own internal buffer. */
TEST(StackMemoryManagement, ManagedStack_MaintainsInternalBuffer)
{
    pmm::Stack<pmm::stack::Loose, pmm::ManagedMemory> stack{ 4096 };

    constexpr auto count = 128;
    const auto buffer    = static_cast<int*>(stack.allocBytes(count * sizeof(int)));

    for (std::size_t i = 0; i < count; ++i)
    {
        buffer[i] = static_cast<int>(316 + i);
    }

    for (std::size_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(static_cast<int>(316 + i), buffer[i]);
    }
}


namespace pmm
{
    /** @brief Verify that unmanaged loose stack uses the external buffer. */
    TEST(StackMemoryManagement, LooseUnmanagedStack_UsesExternalBuffer)
    {
        constexpr auto size = 4096;
        auto* buffer        = new uint8_t[size];
        const Stack<stack::Loose, UnmanagedMemory> stack{ size, buffer };

        EXPECT_EQ(buffer, stack._buffer);
    }

    /** @brief Verify that unmanaged strict stack uses the external buffer. */
    TEST(StackMemoryManagement, StrictUnmanagedStack_UsesExternalBuffer)
    {
        constexpr auto size = 4096;
        auto* buffer        = new uint8_t[size];
        const Stack<stack::Loose, UnmanagedMemory> stack{ size, buffer };

        EXPECT_EQ(buffer, stack._buffer);
    }

} // namespace pmm
