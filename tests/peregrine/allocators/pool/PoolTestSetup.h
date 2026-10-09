#pragma once
/**
 * @file PoolTestSetup.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 3, 2026
 *
 * @brief Contains all test fixtures used by all pool allocator tests.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"
#include "Utils.h"

#include <gtest/gtest.h>
#include <peregrine/allocators/Pool.h>
#include <peregrine/utils/Constants.h>


using namespace pmm::constants;

/// @brief Parameters for Pool allocator's alignment testing.
struct PoolAllocatorAlignmentParams
{
    size_t poolSize, chunkSize, alignment;

    friend void PrintTo(const PoolAllocatorAlignmentParams& param, std::ostream* os)
    {
        *os << "Pool Size: " << param.poolSize << ", Alignment: " << param.alignment
            << ", Chunk Size: " << param.chunkSize;
    }
};


/// @brief Test fixture for @ref pmm::Pool<pmm::MemPolicy::Internal> tests.
struct ManagedPoolAllocatorTests: public testing::Test
{
    size_t poolSize{ 2_KB }, chunkSize{ 8 }, alignment{ 8 };
    pmm::Pool<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled> pool{ poolSize, chunkSize, alignment };

    [[maybe_unused]] friend void PrintTo(const ManagedPoolAllocatorTests& param, std::ostream* os)
    {
        *os << "Managed Pool Allocator (Pool Size: " << param.poolSize << ", Alignment: " << param.alignment
            << ", Chunk Size: " << param.chunkSize << ")";
    }
};


/// @brief Test fixture for @ref pmm::Pool<pmm::MemPolicy::External> tests.
struct UnmanagedPoolAllocatorTests: public testing::Test
{
    size_t bufferSize{ 2_KB }, chunkSize{ 8 }, alignment{ 8 };
    uint8_t* buffer = new uint8_t[bufferSize];
    pmm::Pool<pmm::MemPolicy::External, pmm::TelPolicy::Enabled> pool{ buffer, bufferSize, chunkSize, alignment };

    [[maybe_unused]] friend void PrintTo(const UnmanagedPoolAllocatorTests& param, std::ostream* os)
    {
        *os << "Managed Pool Allocator (Pool Size: " << param.bufferSize << ", Alignment: " << param.alignment
            << ", Chunk Size: " << param.chunkSize << ", Buffer: " << reinterpret_cast<uintptr_t>(param.buffer) << ")";
    }
protected:
    void TearDown() override { delete[] buffer; }
};
