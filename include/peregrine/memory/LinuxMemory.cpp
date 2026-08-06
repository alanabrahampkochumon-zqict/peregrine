/**
 * @file LinuxMemory.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Implementation of memory managemenet functions defined in Memory.h specific to Windows.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "../utils/Preprocessors.h"
#include "Memory.h"

#ifdef PMM_PLATFORM_LINUX

namespace pmm
{
    #include <sys/mman.h>
    #include <unistd.h>

    void* malloc(const std::size_t byteSize) noexcept // NOLINT(bugprone-exception-escape)
    {
        PMM_ASSERT_MSG(byteSize > 1, "Cannot allocate less than 1 byte!");
        return mmap(nullptr, byteSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    }


    bool mfree(void* start, std::size_t size) noexcept // NOLINT(bugprone-exception-escape)
    {
        PMM_ASSERT_MSG(start != nullptr, "Cannot free a nullptr!");
        PMM_ASSERT_MSG(size > 0, "Cannot free 0 bytes of memory!");
        return munmap(start, size) == 0;
    }

    MemoryDetails queryMemoryDetails() noexcept
    {
        // On Linux page size and granularity are the same so we return the same value.
        auto pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
        return MemoryDetails{ .pageSize = pageSize, .granularity = pageSize };
    }

} // namespace pmm

#endif
