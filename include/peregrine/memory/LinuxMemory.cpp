/**
 * @file LinuxMemory.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Implementation of memory managemenet functions defined in Memory.h specific to Windows.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

// TODO: Update Implementation
#include "../utils/Preprocessors.h"
#include "Memory.h"
#include <cstdint>

#ifdef PMM_PLATFORM_LINUX

namespace pmm
{
    void* malloc(const std::size_t byteSize) noexcept // NOLINT(bugprone-exception-escape)
    {
        PMM_ASSERT_MSG(byteSize > 1, "Cannot allocate less than 1 byte!");
        return new uint8_t[byteSize]; // TODO: Replace
    }


    bool mfree(void* start, std::size_t) noexcept // NOLINT(bugprone-exception-escape)
    {
        PMM_ASSERT_MSG(start != nullptr, "Cannot free a nullptr!");
        // Windows requires a dwSize of 0 to free the entire memory block reserved with VirtualAlloc
        // TODO: Implementation
        return true;
    }

    MemoryDetails queryMemoryDetails() noexcept
    { // TODO: Implementation
        return MemoryDetails{ .pageSize = 4096, .granularity = 4096 };
    }

} // namespace pmm

#endif
