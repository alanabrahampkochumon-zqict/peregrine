/**
 * @file SafeModeTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 29, 2026
 *
 * @brief Verifies tlsf allocators's returns safer fallbacks for allocations and frees with SafeMode
 *        and in Release Mode.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "TLSFTestSetup.h"


#ifndef ENABLE_PMM_DEATH_TESTS

namespace
{
    class TLSFAlignmentNonPowersOfTwo: public testing::TestWithParam<size_t>
    {};
    INSTANTIATE_TEST_SUITE_P(TLSFAllocationAlignmentNonPowerOfTwo, TLSFAlignmentNonPowersOfTwo,
                             ::testing::Values(0, 1, 3, 5, 111));

} // namespace



/**************************************
 *      INTERNALLY MANAGED TLSF       *
 **************************************/


TEST_F(InternallyManagedTLSFTests, MAlloc_ZeroSizeReturnsNullptrInSafeMode) { EXPECT_EQ(nullptr, tlsf.malloc(0)); }

TEST_F(InternallyManagedTLSFTests, MAlloc_NearTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize - 1)); }

TEST_F(InternallyManagedTLSFTests, MAlloc_EqualToTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize)); }

TEST_F(InternallyManagedTLSFTests, MAlloc_GreaterThanTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize + 1)); }

TEST_F(InternallyManagedTLSFTests, Malloc_OutofMemoryAllocation_ReturnsNullptrInSafeMode)
{
    static_cast<void>(tlsf.malloc(tlsfSize - 128));
    EXPECT_EQ(nullptr, tlsf.malloc(1_KB));
}

TEST_F(InternallyManagedTLSFTests, Malloc_OutofMemoryDueToFragmentation_ReturnsNullptrInSafeMode)
{
    const auto mem1                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem2 = tlsf.malloc(500_KB);
    const auto mem3                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(250_KB);
    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    EXPECT_EQ(nullptr, tlsf.malloc(1_MB));
}


TEST_P(TLSFAlignmentNonPowersOfTwo, InternallyManagedTLSF_MAlloc_NonPowerOfTwoAlignmentReturnsNullptrInSafeMode)
{
    pmm::TLSF<pmm::MemPolicy::Internal> tlsf{ 1_KB };
    EXPECT_EQ(nullptr, tlsf.malloc(24, this->GetParam()));
}


TEST_F(InternallyManagedTLSFTests, Resize_ToZeroSizeReturnsNullptrInSafeMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 128, 0));
}


TEST_F(TLSFAlignmentNonPowersOfTwo, InternallyManagedTLSF_Resize_NonPowerOfTwoAlignmentReturnsNullptrInSafeMode)
{
    pmm::TLSF<pmm::MemPolicy::Internal> tlsf{ 1_KB };
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 128, 256, this->GetParam()));
}


TEST_F(InternallyManagedTLSFTests, Resize_NullptrReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.resize(nullptr, 64, 128)); }


TEST_F(InternallyManagedTLSFTests, Resize_FromZeroSizeReturnsNullptrInSafeMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 0, 128));
}


TEST_F(InternallyManagedTLSFTests, Resize_GreaterThanTLSFSizeReturnsNullptrInSafeMode)
{
    const auto mem1 = tlsf.malloc(1500_KB);
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 1500_KB, 2048_KB));
}


TEST_F(InternallyManagedTLSFTests, Resize_OutofMemoryAllocation_ReturnsNullptrInSafeMode)
{
    const auto mem1 = tlsf.malloc(400_KB);
    static_cast<void>(tlsf.malloc(1400_KB));
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 400_KB, 800_KB));
}

TEST_F(InternallyManagedTLSFTests, Resize_OutofMemoryDueToFragmentation_ReturnsNullptrInSafeMode)
{
    const auto mem1                  = tlsf.malloc(300_KB);
    const auto mem2                  = tlsf.malloc(300_KB);
    [[maybe_unused]] const auto mem3 = tlsf.malloc(300_KB);
    const auto mem4                  = tlsf.malloc(300_KB);
    [[maybe_unused]] const auto mem5 = tlsf.malloc(300_KB);
    const auto mem6                  = tlsf.malloc(300_KB);
    tlsf.mfree(mem2);
    tlsf.mfree(mem4);
    tlsf.mfree(mem6);
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 300_KB, 650_KB));
}


/**************************************
 *      EXTERNALLY MANAGED TLSF       *
 **************************************/


TEST(ExternallyManagedTLSFCtorTests, NullptrForBackingBuffer_ReturnsNullptrInSafeMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::TLSF<pmm::MemPolicy::External>(nullptr, 512)), ""); }

TEST(ExternallyManagedTLSFCtorTests, ZeroSize_ReturnsNullptrInSafeMode)
{
    const auto buffer = new uint8_t[1_KB];
    EXPECT_DEBUG_DEATH(static_cast<void>(pmm::TLSF<pmm::MemPolicy::External>(buffer, 0)), "");
    delete[] buffer;
}

TEST_F(ExternallyManagedTLSFTests, MAlloc_ZeroSizeReturnsNullptrInSafeMode) { EXPECT_EQ(nullptr, tlsf.malloc(0)); }

TEST_F(ExternallyManagedTLSFTests, MAlloc_NearTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize - 1)); }

TEST_F(ExternallyManagedTLSFTests, MAlloc_EqualToTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize)); }

TEST_F(ExternallyManagedTLSFTests, MAlloc_GreaterThanTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize + 1)); }

TEST_F(ExternallyManagedTLSFTests, Malloc_OutofMemoryAllocation_ReturnsNullptrInSafeMode)
{
    static_cast<void>(tlsf.malloc(tlsfSize - 128));
    EXPECT_EQ(nullptr, tlsf.malloc(1_KB));
}

TEST_F(ExternallyManagedTLSFTests, Malloc_OutofMemoryDueToFragmentation_ReturnsNullptrInSafeMode)
{
    const auto mem1                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem2 = tlsf.malloc(500_KB);
    const auto mem3                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(250_KB);
    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    EXPECT_EQ(nullptr, tlsf.malloc(1_MB));
}


TEST_P(TLSFAlignmentNonPowersOfTwo, ExternallyManagedTLSF_MAlloc_NonPowerOfTwoAlignmentReturnsNullptrInSafeMode)
{
    const auto buffer = new uint8_t[1_KB];
    pmm::TLSF<pmm::MemPolicy::External> tlsf{ buffer, 1_KB };
    EXPECT_EQ(nullptr, tlsf.malloc(24, this->GetParam()));
    delete[] buffer;
}


TEST_F(ExternallyManagedTLSFTests, Resize_ToZeroSizeReturnsNullptrInSafeMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 128, 0));
}


TEST_P(TLSFAlignmentNonPowersOfTwo, ExternallyManagedTLSF_Resize_NonPowerOfTwoAlignmentReturnsNullptrInSafeMode)
{
    const auto buffer = new uint8_t[1_KB];
    pmm::TLSF<pmm::MemPolicy::External> tlsf{ buffer, 1_KB };
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 128, 256, this->GetParam()));
    delete[] buffer;
}


TEST_F(ExternallyManagedTLSFTests, Resize_NullptrReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.resize(nullptr, 64, 128)); }


TEST_F(ExternallyManagedTLSFTests, Resize_FromZeroSizeReturnsNullptrInSafeMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 0, 128));
}


TEST_F(ExternallyManagedTLSFTests, Resize_GreaterThanTLSFSizeReturnsNullptrInSafeMode)
{
    const auto mem1 = tlsf.malloc(1500_KB);
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 1500_KB, 2048_KB));
}


TEST_F(ExternallyManagedTLSFTests, Resize_OutofMemoryAllocation_ReturnsNullptrInSafeMode)
{
    const auto mem1 = tlsf.malloc(400_KB);
    static_cast<void>(tlsf.malloc(1400_KB));
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 400_KB, 800_KB));
}

TEST_F(ExternallyManagedTLSFTests, Resize_OutofMemoryDueToFragmentation_ReturnsNullptrInSafeMode)
{
    const auto mem1                  = tlsf.malloc(300_KB);
    const auto mem2                  = tlsf.malloc(300_KB);
    [[maybe_unused]] const auto mem3 = tlsf.malloc(300_KB);
    const auto mem4                  = tlsf.malloc(300_KB);
    [[maybe_unused]] const auto mem5 = tlsf.malloc(300_KB);
    const auto mem6                  = tlsf.malloc(300_KB);
    tlsf.mfree(mem2);
    tlsf.mfree(mem4);
    tlsf.mfree(mem6);
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 300_KB, 650_KB));
}


TEST_F(ExternallyManagedTLSFTests, MFree_NullptrReturnsNullptrInSafeMode)
{ EXPECT_DEBUG_DEATH(tlsf.mfree(nullptr), ""); }


#endif
