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
     * @brief Structure holding all memory details of the current system, including page size, granularity, etc.
     */
    struct MemoryDetails
    {
        size_t pageSize;    ///> The memory allocation page size.
        size_t granularity; ///> The granularity/alignment
    };


    /**
     * @brief Allocate a memory block from the operating system vault.
     *
     * @note The @p byteSize must be a power of 2.
     * @note Although not re-enforced, it is recommended to request memory in multiples of Page Size.
     *       Use @ref queryMemoryDetails to get memory information like page size per platform.
     *
     * @param byteSize The amount of memory to allocate.
     *
     * @return A void* to the base address of the allocation or nullptr if allocation fails.
     */
    [[nodiscard]] void* malloc(std::size_t byteSize) noexcept;


    /**
     * @brief Free a memory block allocated using @ref pmm::malloc.
     *
     * @param start The start address of the allocated memory.
     * @param size The size of the allocated memory.
     * 
     * @return A boolean indicating whether the memory was freed.
     */
    bool mfree(void* start, std::size_t size) noexcept;


    /**
     * @brief Query the system for memory details in a platform agnostic manner.
     * @return A @ref MemoryDetails struct containing all memory details.
     */
    MemoryDetails queryMemoryDetails() noexcept;

} // namespace pmm
