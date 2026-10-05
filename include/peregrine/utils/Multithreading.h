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


/**
 * @addtogroup PMM_MT
 * @{
 */

#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
    #include <immintrin.h>
    #define _PMM_MT_YIELD() _mm_pause()
#elif defined(__arm64__) || defined(_M_ARM64)
    #define _PMM_MT_YIELD() __asm__ __volatile__("isb sy" : : : "memory")
#else
    #define _PMM_MT_YIELD()
#endif

namespace pmm::mt
{
    /// @brief Multithreading policy.
    enum class MTPolicy : uint8_t
    {
        NoMT,    /// Not safe for multithreaded environments.
        SpinLock /// Multithreading support with spin locks.
    };


    /// @brief Standard SpinLock with a spin-wait loop hint on supported platforms.
    /// @note Direct use discouraged. Use @ref Mutex_t to switch based on the current MT Policy.
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


    /// @brief Dummy SpinLock used when MTPolicy is disable.
    /// @note Direct use discouraged. Use @ref Mutex_t to switch based on the current MT Policy.
    struct DummySpinLock
    {
        void lock() noexcept {}
        void unlock() noexcept {}
    };


    /// @brief Type of Mutex available based on @ref MTPolicy.
    template <MTPolicy Policy>
    using Mutex_t = std::conditional_t<Policy == MTPolicy::SpinLock, SpinLock, DummySpinLock>;


    /// @brief Concept defining a simple mutex with functionalities for locking and unlocking.
    template <typename T>
    concept SimpleMutex = requires(T t) {
        t.lock();
        t.unlock();
    };


    /// @brief Mutex wrapper providing RAII mechanism for the duration of the scoped block.
    template <SimpleMutex T>
    struct LockGuard
    {
        explicit constexpr LockGuard(T& mutex) noexcept: _mutex{ mutex } { _mutex.lock(); }
        constexpr ~LockGuard() noexcept { _mutex.unlock(); }

    private:
        T& _mutex;
    };

} // namespace pmm::mt


/** @} */
