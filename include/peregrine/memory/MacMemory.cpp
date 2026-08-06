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

    void* malloc(const std::size_t byteSize) noexcept // NOLINT(bugprone-exception-escape)
    {
        PMM_ASSERT_MSG(byteSize > 0, "Cannot allocate 0 bytes of memory!");

        auto ptr = mmap(nullptr, byteSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        PMM_ASSERT_MSG(ptr != MAP_FAILED, "Failed to allocate virtual memory!");

        return ptr;
    }


    bool mfree(void* start, std::size_t size) noexcept // NOLINT(bugprone-exception-escape)
    {
        PMM_ASSERT_MSG(start != nullptr, "Cannot free a nullptr!");
        PMM_ASSERT_MSG(size > 0, "Cannot free 0 bytes of memory!");
        return munmap(start, size) == 0;
    }

    MemoryDetails queryMemoryDetails() noexcept
    { // TODO: Implementation
        return MemoryDetails{ .pageSize = 4096, .granularity = 4096 };
    }

} // namespace pmm

#endif
