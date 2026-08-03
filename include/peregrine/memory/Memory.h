#pragma once
/**
 * @file Memory.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Defines OS agnostic memory management operations.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstddef>


namespace pmm
{

    /**
     * @brief Allocate a memory block from the operating system vault.
     *
     * @note The @p byteSize must align with the target operating system page size.
     *
     * @param byteSize The amount of memory to allocate.
     *
     * @return A void* to the base address of the allocation or nullptr if allocation fails.
     */
    void* malloc(std::size_t byteSize);

    /**
     * @brief Free a memory block allocated using @ref pmm::malloc.
     * @param start The start address of the allocated memory.
     * @param size The size of the allocated memory.
     */
    void mfree(void* start, std::size_t size);

} // namespace pmm
