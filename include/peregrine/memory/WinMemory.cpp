/**
 * @file WinMemory.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Implementation of memory managemenet functions defined in Memory.h specific to Windows.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#ifdef _WIN32

    #define PMM_WINDOWS

    #include "Memory.h"

    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>


namespace pmm
{
    void* malloc(const std::size_t byteSize)
    {
        // TODO: Add checks for page size alignment
        return VirtualAlloc(nullptr, byteSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }

    bool mfree(void* start, std::size_t)
    {
        // Windows requires a dwSize of 0 to free the entire memory block reserved with VirtualAlloc
        return VirtualFree(start, 0, MEM_RELEASE);
    }
} // namespace pmm

#endif
