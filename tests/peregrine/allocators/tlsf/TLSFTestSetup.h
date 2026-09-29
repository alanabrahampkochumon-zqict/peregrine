#pragma once
/**
 * @file TLSFTestSetup.h
 * @author Alan Abraham P Kochumon
 * @date Created on: September 07, 2026
 *
 * @brief Contains all test fixtures used by all TLSF allocator tests.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Mocks.h"
#include "Utils.h"

#include <gtest/gtest.h>
#include <peregrine/allocators/TLSF.h>
#include <peregrine/utils/Constants.h>


using namespace pmm::constants;

/**
 * @brief Test fixture for internally memory managed @ref pmm::TLSF.
 */
class InternallyManagedTLSFTests: public testing::Test
{
public:
    static constexpr size_t tlsfSize{ 2_MB };
    pmm::TLSF<pmm::MemPolicy::Internal> tlsf{ tlsfSize };
    using Header   = pmm::TLSF<pmm::MemPolicy::Internal>::Header;
    using Offset_t = pmm::TLSF<pmm::MemPolicy::Internal>::HeaderOffset_t;
};


/**
 * @brief Test fixture for externally memory managed @ref pmm::TLSF.
 */
class ExternallyManagedTLSFTests: public testing::Test
{
public:
    static constexpr size_t tlsfSize{ 2_MB };
    uint8_t* buffer = new uint8_t[tlsfSize]();
    pmm::TLSF<pmm::MemPolicy::External> tlsf{ buffer, tlsfSize };
    using Header   = pmm::TLSF<pmm::MemPolicy::External>::Header;
    using Offset_t = pmm::TLSF<pmm::MemPolicy::External>::HeaderOffset_t;

protected:
    ~ExternallyManagedTLSFTests() override { delete[] buffer; }
};
