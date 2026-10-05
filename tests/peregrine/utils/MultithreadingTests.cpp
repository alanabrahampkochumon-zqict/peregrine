/**
 * @file MultithreadingTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 05, 2026
 *
 * @brief Verifies multithreading primitives and functions.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "peregrine/utils/Multithreading.h"

#include <gtest/gtest.h>
#include <peregrine/utils/Multithreading.h>
#include <thread>
#include <vector>


/**
 * @addtogroup T_PMM_Helpers
 * @{
 */


/**************************************
 *          TEST SETUP                *
 **************************************/

namespace
{
    /**************************************
     *          STATIC TESTS              *
     **************************************/
    namespace static_tests
    {
        using namespace pmm::mt;
        /// @test Verify that @ref Mutex_t returns a @ref SpinLock type with an MTPolicy of SpinLock.
        static_assert(std::is_same_v<Mutex_t<MTPolicy::SpinLock>, SpinLock>);

        /// @test Verify that @ref Mutex_t returns a @ref DummySpinLock type with an MTPolicy of NoMT.
        static_assert(std::is_same_v<Mutex_t<MTPolicy::NoMT>, DummySpinLock>);

    } // namespace static_tests
} // namespace


/// @ref Verify that @ref pmm::mt::SpinLock is able to keep mutual exclusivity of resources.
TEST(PeregrineMultithreadindTests, SpinLockKeepsMutualExclusivity)
{
    constexpr size_t numTotalIterations = 10000000; // 10MIL
    const size_t numThreads = std::thread::hardware_concurrency() == 0 ? 4 : std::thread::hardware_concurrency();
    const size_t numIterationsPerThread = numTotalIterations / numThreads;
    pmm::mt::SpinLock mutex;
    std::vector<std::thread> threads{};
    size_t runningCounter = 0; // Shared resource

    // Helper function for incrementing the running counter for each thread
    const auto incrementCounter = [&]() {
        for (size_t j{ 0 }; j < numIterationsPerThread; ++j)
        {
            mutex.lock();     // Lock runningCounter
            ++runningCounter; // Mutate the value
            mutex.unlock();   // Unlock runningCounter
        }
    };

    // Loop and dispatch the incrementCounter on each thread.
    for (size_t i{ 0 }; i < numThreads; ++i)
    {
        threads.emplace_back(std::thread(incrementCounter));
    }

    // Join the threads so the program will wait for the threads to finish before exiting
    for (size_t i{ 0 }; i < numThreads; ++i)
    {
        threads[i].join();
    }

    // Assert the equality of the expected and the real cumulative sum.
    EXPECT_EQ(numThreads * numIterationsPerThread, runningCounter);
}


/** @} */
