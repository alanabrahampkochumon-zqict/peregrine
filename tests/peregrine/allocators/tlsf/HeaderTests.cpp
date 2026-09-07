/**
 * @file HeaderTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 07, 2026
 *
 * @brief Verifies the member functions of TLSF header.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "TLSFTestSetup.h"

namespace
{
    class TLSFHeaderTests: public testing::Test
    {
    public:
        size_t blockSize = 2_MB;
        pmm::TLSF<>::Header header{ 2_MB };
    };
} // namespace



/**
 * @addtogroup T_PMM_TLSF_Header
 * @{
 */

TEST_F(TLSFHeaderTests, GetSize_ReturnsTheCorrectSize) { EXPECT_EQ(blockSize, header.getSize()); }

TEST_F(TLSFHeaderTests, SetSize_UpdatesTheSize)
{
    constexpr size_t newSize = 2_KB;
    header.setSize(newSize);
    EXPECT_EQ(newSize, header.getSize());
}

// TODO: Add tests


/** @} */
