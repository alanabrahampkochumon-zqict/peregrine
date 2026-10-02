#pragma once
/**
 * @file Multithreading.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 01, 2026
 *
 * @brief Multithreading primitives.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <atomic>

namespace pmm::mt
{

#if defined(__i386__) || deifned(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
    #include <immintrin.h>
    #define _PMM_MT_YIELD() _mm_pause()
#elif defined(__arm64__) || defined(_M_ARM64)
    #define _PMM_MT_YIELD() __asm__ __volatile__("isb sy" : : : "memory")
#else
    #define _PMM_MT_YIELD()
#endif



    /// @brief Standard SpinLock with a spin-wait loop hint on supported platforms.
    struct SpinLock
    {
        std::atomic_flag flag = ATOMIC_FLAG_INIT;

        void lock() noexcept
        {
            while (flag.test_and_set(std::memory_order_acquire))
            {
                _PMM_MT_YIELD();
            }
        }

        void unlock() { flag.clear(std::memory_order_release); }
    };

    /// @brief Dummy SpinLock to be used when MTPolicy is non-thread safe.
    struct DummySpinLock
    {
        void lock() noexcept {}
        void unlock() noexcept {}
    };


} // namespace pmm::mt
