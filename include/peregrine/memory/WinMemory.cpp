/**
 * @file WinMemory.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Implementation of memory managemenet functions defined in Memory.h specific to Windows.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

// TODO: REMOVE
#define _WIN32

#ifdef _WIN32

    #include "Memory.h"

    #include <memoryapi.h>


namespace pmm
{
    void* malloc(std::size_t byteSize)
    {
        // TODO: Implementation
        return nullptr;
    }

    void mfree(void* start, std::size_t size)
    {
        // TODO: Implementation
    }
} // namespace pmm

#endif
