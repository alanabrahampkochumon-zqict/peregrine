/**
 * @file LinuxMemory.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Implementation of memory management functions defined in Memory.h specific to macOS.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "../utils/Preprocessors.h"
#include "Memory.h"


#ifdef PMM_PLATFORM_MACOS

namespace pmm
{
    #include <sys/mman.h>
    #include <unistd.h>

    void* memAlloc(const std::size_t byteSize) noexcept // NOLINT(bugprone-exception-escape)
    {
        PMM_ASSERT_MSG(byteSize > 0, "Cannot allocate 0 bytes!");
        auto ptr = mmap(nullptr, byteSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        return ptr == MAP_FAILED ? nullptr : ptr;
    }


    bool memFree(void* start, std::size_t size) noexcept // NOLINT(bugprone-exception-escape)
    {
        if (start == nullptr || size == 0)
        {
            return false;
        }
        return munmap(start, size) == 0;
    }

    MemoryDetails queryMemoryDetails() noexcept
    {
        // On MacOS page size and granularity are the same so we return the same value.
        auto pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
        return MemoryDetails{ .pageSize = pageSize, .granularity = pageSize };
    }

} // namespace pmm

#endif
