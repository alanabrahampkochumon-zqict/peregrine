/**
 * @file DeathTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 28, 2026
 *
 * @brief Verifies tlsf allocators's assertions trigger correctly in DEBUG MODE.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "TLSFTestSetup.h"


#ifdef ENABLE_PMM_DEATH_TESTS

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

TEST(InternallyManagedTLSFCtorTests, ZeroSize_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::TLSF<pmm::MemPolicy::Internal>(0)), ""); }

TEST_F(InternallyManagedTLSFTests, MAlloc_ZeroSizeTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(0)), ""); }

TEST_F(InternallyManagedTLSFTests, MAlloc_NearTLSFSizeTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(tlsfSize - 1)), ""); }

TEST_F(InternallyManagedTLSFTests, MAlloc_EqualToTLSFSizeTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(tlsfSize)), ""); }

TEST_F(InternallyManagedTLSFTests, MAlloc_GreaterThanTLSFSizeTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(tlsfSize + 1)), ""); }

TEST_F(InternallyManagedTLSFTests, Malloc_OutofMemoryAllocation_TriggersAssertionInDebugMode)
{
    static_cast<void>(tlsf.malloc(tlsfSize - 128));
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(1_KB)), "");
}

TEST_F(InternallyManagedTLSFTests, Malloc_OutofMemoryDueToFragmentation_TriggersAssertionInDebugMode)
{
    const auto mem1                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem2 = tlsf.malloc(500_KB);
    const auto mem3                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(250_KB);
    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(1_MB)), "");
}


TEST_P(TLSFAlignmentNonPowersOfTwo, InternallyManagedTLSF_MAlloc_NonPowerOfTwoAlignmentTriggersAssertionInDebugMode)
{
    pmm::TLSF<pmm::MemPolicy::Internal> tlsf{ 1_KB };
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(24, this->GetParam())), "");
}


TEST_F(InternallyManagedTLSFTests, Resize_ToZeroSizeTriggersAssertionInDebugMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem, 128, 0)), "");
}


TEST_F(TLSFAlignmentNonPowersOfTwo, InternallyManagedTLSF_Resize_NonPowerOfTwoAlignmentTriggersAssertionInDebugMode)
{
    pmm::TLSF<pmm::MemPolicy::Internal> tlsf{ 1_KB };
    const auto mem = tlsf.malloc(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem, 128, 256, this->GetParam())), "");
}


TEST_F(InternallyManagedTLSFTests, Resize_NullptrTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(nullptr, 64, 128)), ""); }


TEST_F(InternallyManagedTLSFTests, Resize_FromZeroSizeTriggersAssertionInDebugMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem, 0, 128)), "");
}


TEST_F(InternallyManagedTLSFTests, Resize_GreaterThanTLSFSizeTriggersAssertionInDebugMode)
{
    const auto mem1 = tlsf.malloc(1500_KB);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem1, 1500_KB, 2048_KB)), "");
}


TEST_F(InternallyManagedTLSFTests, Resize_OutofMemoryAllocation_TriggersAssertionInDebugMode)
{
    const auto mem1 = tlsf.malloc(400_KB);
    static_cast<void>(tlsf.malloc(1400_KB));
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem1, 400_KB, 800_KB)), "");
}

TEST_F(InternallyManagedTLSFTests, Resize_OutofMemoryDueToFragmentation_TriggersAssertionInDebugMode)
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
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem1, 300_KB, 650_KB)), "");
}


TEST_F(InternallyManagedTLSFTests, MFree_NullptrTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(tlsf.mfree(nullptr), ""); }



/**************************************
 *      EXTERNALLY MANAGED TLSF       *
 **************************************/


TEST(ExternallyManagedTLSFCtorTests, NullptrForBackingBuffer_TriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(pmm::TLSF<pmm::MemPolicy::External>(nullptr, 512)), ""); }

TEST(ExternallyManagedTLSFCtorTests, ZeroSize_TriggersAssertionInDebugMode)
{
    const auto buffer = new uint8_t[1_KB];
    EXPECT_DEBUG_DEATH(static_cast<void>(pmm::TLSF<pmm::MemPolicy::External>(buffer, 0)), "");
    delete[] buffer;
}

TEST_F(ExternallyManagedTLSFTests, MAlloc_ZeroSizeTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(0)), ""); }

TEST_F(ExternallyManagedTLSFTests, MAlloc_NearTLSFSizeTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(tlsfSize - 1)), ""); }

TEST_F(ExternallyManagedTLSFTests, MAlloc_EqualToTLSFSizeTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(tlsfSize)), ""); }

TEST_F(ExternallyManagedTLSFTests, MAlloc_GreaterThanTLSFSizeTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(tlsfSize + 1)), ""); }

TEST_F(ExternallyManagedTLSFTests, Malloc_OutofMemoryAllocation_TriggersAssertionInDebugMode)
{
    static_cast<void>(tlsf.malloc(tlsfSize - 128));
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(1_KB)), "");
}

TEST_F(ExternallyManagedTLSFTests, Malloc_OutofMemoryDueToFragmentation_TriggersAssertionInDebugMode)
{
    const auto mem1                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem2 = tlsf.malloc(500_KB);
    const auto mem3                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(250_KB);
    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(1_MB)), "");
}


TEST_P(TLSFAlignmentNonPowersOfTwo, ExternallyManagedTLSF_MAlloc_NonPowerOfTwoAlignmentTriggersAssertionInDebugMode)
{
    const auto buffer = new uint8_t[1_KB];
    pmm::TLSF<pmm::MemPolicy::External> tlsf{ buffer, 1_KB };
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.malloc(24, this->GetParam())), "");
    delete[] buffer;
}


TEST_F(ExternallyManagedTLSFTests, Resize_ToZeroSizeTriggersAssertionInDebugMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem, 128, 0)), "");
}


TEST_P(TLSFAlignmentNonPowersOfTwo, ExternallyManagedTLSF_Resize_NonPowerOfTwoAlignmentTriggersAssertionInDebugMode)
{
    const auto buffer = new uint8_t[1_KB];
    pmm::TLSF<pmm::MemPolicy::External> tlsf{ buffer, 1_KB };
    const auto mem = tlsf.malloc(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem, 128, 256, this->GetParam())), "");
    delete[] buffer;
}


TEST_F(ExternallyManagedTLSFTests, Resize_NullptrTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(nullptr, 64, 128)), ""); }


TEST_F(ExternallyManagedTLSFTests, Resize_FromZeroSizeTriggersAssertionInDebugMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem, 0, 128)), "");
}


TEST_F(ExternallyManagedTLSFTests, Resize_GreaterThanTLSFSizeTriggersAssertionInDebugMode)
{
    const auto mem1 = tlsf.malloc(1500_KB);
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem1, 1500_KB, 2048_KB)), "");
}


TEST_F(ExternallyManagedTLSFTests, Resize_OutofMemoryAllocation_TriggersAssertionInDebugMode)
{
    const auto mem1 = tlsf.malloc(400_KB);
    static_cast<void>(tlsf.malloc(1400_KB));
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem1, 400_KB, 800_KB)), "");
}

TEST_F(ExternallyManagedTLSFTests, Resize_OutofMemoryDueToFragmentation_TriggersAssertionInDebugMode)
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
    EXPECT_DEBUG_DEATH(static_cast<void>(tlsf.resize(mem1, 300_KB, 650_KB)), "");
}


TEST_F(ExternallyManagedTLSFTests, MFree_NullptrTriggersAssertionInDebugMode)
{ EXPECT_DEBUG_DEATH(tlsf.mfree(nullptr), ""); }


#endif
