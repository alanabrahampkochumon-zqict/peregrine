/**
 * @file WinMemory.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Implementation of memory management functions defined in Memory.h specific to Windows.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "../utils/Preprocessors.h"
#include "Memory.h"

#ifdef PMM_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX // Prevent windows from hijacking min and max functions
    #include <Windows.h>

namespace pmm
{

    void* memAlloc(const std::size_t byteSize) noexcept // NOLINT(bugprone-exception-escape)
    {
        PMM_ASSERT_MSG(byteSize > 0, "Cannot allocate 0 bytes!");
        return VirtualAlloc(nullptr, byteSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }


    bool memFree(void* start, std::size_t) noexcept // NOLINT(bugprone-exception-escape)
    {
        if (start == nullptr)
        {
            return false;
        }
        // Windows requires a dwSize of 0 to free the entire memory block reserved with VirtualAlloc
        return VirtualFree(start, 0, MEM_RELEASE);
    }

    MemoryDetails queryMemoryDetails() noexcept
    {
        SYSTEM_INFO info{};
        GetSystemInfo(&info);

        return MemoryDetails{ .pageSize = info.dwPageSize, .granularity = info.dwAllocationGranularity };
    }


} // namespace pmm

#endif
