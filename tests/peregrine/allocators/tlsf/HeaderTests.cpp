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


TEST_F(TLSFHeaderTests, SetSize_DoesNotAlterFlags)
{
    // Set the both the flags as free
    header.markFree();
    header.markPrevFree();

    constexpr size_t newSize = 2_KB;
    header.setSize(newSize);

    EXPECT_TRUE(header.isFree());
    EXPECT_TRUE(header.isPrevFree());
}


TEST_F(TLSFHeaderTests, MarkFree_MarksTheHeaderAsFree)
{
    header.markFree();
    EXPECT_TRUE(header.isFree());
}


TEST_F(TLSFHeaderTests, MarkFree_DoesNotAlterSize)
{
    header.markFree();
    EXPECT_EQ(blockSize, header.getSize());
}


TEST_F(TLSFHeaderTests, MarkUsed_MarksTheHeaderAsUsed)
{
    header.markUsed();
    EXPECT_FALSE(header.isFree());
}


TEST_F(TLSFHeaderTests, MarkUsed_DoesNotAlterSize)
{
    header.markUsed();
    EXPECT_EQ(blockSize, header.getSize());
}

TEST_F(TLSFHeaderTests, MarkPrevFree_MarksTheHeadersPreviousAsFree)
{
    header.markPrevFree();
    EXPECT_TRUE(header.isPrevFree());
}


TEST_F(TLSFHeaderTests, MarkPrevFree_DoesNotAlterSize)
{
    header.markPrevFree();
    EXPECT_EQ(blockSize, header.getSize());
}


TEST_F(TLSFHeaderTests, MarkPrevUsed_MarksTheHeadersPreviousAsUsed)
{
    header.markPrevUsed();
    EXPECT_FALSE(header.isPrevFree());
}


TEST_F(TLSFHeaderTests, MarkPrevUsed_DoesNotAlterSize)
{
    header.markPrevUsed();
    EXPECT_EQ(blockSize, header.getSize());
}

// TODO: Add tests


/** @} */
