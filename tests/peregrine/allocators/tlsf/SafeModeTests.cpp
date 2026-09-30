/**
 * @file SafeModeTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 29, 2026
 *
 * @brief Verifies TLSF allocators's returns safer fallbacks for allocations and frees with SafeMode
 *        and in Release Mode.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "TLSFTestSetup.h"


#ifndef ENABLE_PMM_DEATH_TESTS

namespace
{
    class SafeTLSFAlignmentNonPowersOfTwo: public testing::TestWithParam<size_t>
    {};
    INSTANTIATE_TEST_SUITE_P(TLSFAllocationAlignmentNonPowerOfTwo, SafeTLSFAlignmentNonPowersOfTwo,
                             ::testing::Values(0, 1, 3, 5, 111));



    /**
     * @brief Test fixture for internally memory managed @ref pmm::TLSF.
     */
    class InternallyManagedSafeTLSFTests: public testing::Test
    {
    public:
        static constexpr size_t tlsfSize{ 2_MB };
        pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe> tlsf{ tlsfSize };
        using Header = pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe>::Header;
        using Offset_t =
            pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe>::HeaderOffset_t;
    };


    /**
     * @brief Test fixture for externally memory managed @ref pmm::TLSF.
     */
    class ExternallyManagedSafeTLSFTests: public testing::Test
    {
    public:
        static constexpr size_t tlsfSize{ 2_MB };
        uint8_t* buffer = new uint8_t[tlsfSize]();
        pmm::TLSF<pmm::MemPolicy::External, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe> tlsf{ buffer,
                                                                                                       tlsfSize };
        using Header = pmm::TLSF<pmm::MemPolicy::External, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe>::Header;
        using Offset_t =
            pmm::TLSF<pmm::MemPolicy::External, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe>::HeaderOffset_t;

    protected:
        ~ExternallyManagedSafeTLSFTests() override { delete[] buffer; }
    };


} // namespace



/**************************************
 *      INTERNALLY MANAGED TLSF       *
 **************************************/


TEST_F(InternallyManagedSafeTLSFTests, MAlloc_ZeroSizeReturnsNullptrInSafeMode) { EXPECT_EQ(nullptr, tlsf.malloc(0)); }

TEST_F(InternallyManagedSafeTLSFTests, MAlloc_NearTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize - 1)); }

TEST_F(InternallyManagedSafeTLSFTests, MAlloc_EqualToTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize)); }

TEST_F(InternallyManagedSafeTLSFTests, MAlloc_GreaterThanTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize + 1)); }

TEST_F(InternallyManagedSafeTLSFTests, Malloc_OutofMemoryAllocation_ReturnsNullptrInSafeMode)
{
    static_cast<void>(tlsf.malloc(tlsfSize - 128));
    EXPECT_EQ(nullptr, tlsf.malloc(1_KB));
}

TEST_F(InternallyManagedSafeTLSFTests, Malloc_OutofMemoryDueToFragmentation_ReturnsNullptrInSafeMode)
{
    const auto mem1                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem2 = tlsf.malloc(500_KB);
    const auto mem3                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(250_KB);
    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    EXPECT_EQ(nullptr, tlsf.malloc(1_MB));
}


TEST_P(SafeTLSFAlignmentNonPowersOfTwo, InternallyManagedTLSF_MAlloc_NonPowerOfTwoAlignmentReturnsNullptrInSafeMode)
{
    pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe> tlsf{ 1_KB };
    EXPECT_EQ(nullptr, tlsf.malloc(24, this->GetParam()));
}


TEST_F(InternallyManagedSafeTLSFTests, Resize_ToZeroSizeReturnsNullptrInSafeMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 128, 0));
}


TEST_P(SafeTLSFAlignmentNonPowersOfTwo, InternallyManagedTLSF_Resize_NonPowerOfTwoAlignmentReturnsNullptrInSafeMode)
{
    pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe> tlsf{ 1_KB };
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 128, 256, this->GetParam()));
}


TEST_F(InternallyManagedSafeTLSFTests, Resize_NullptrReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.resize(nullptr, 64, 128)); }


TEST_F(InternallyManagedSafeTLSFTests, Resize_FromZeroSizeReturnsNullptrInSafeMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 0, 128));
}


TEST_F(InternallyManagedSafeTLSFTests, Resize_GreaterThanTLSFSizeReturnsNullptrInSafeMode)
{
    const auto mem1 = tlsf.malloc(1500_KB);
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 1500_KB, 2048_KB));
}


TEST_F(InternallyManagedSafeTLSFTests, Resize_OutofMemoryAllocation_ReturnsNullptrInSafeMode)
{
    const auto mem1 = tlsf.malloc(400_KB);
    static_cast<void>(tlsf.malloc(1400_KB));
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 400_KB, 800_KB));
}

TEST_F(InternallyManagedSafeTLSFTests, Resize_OutofMemoryDueToFragmentation_ReturnsNullptrInSafeMode)
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

TEST_F(ExternallyManagedSafeTLSFTests, MAlloc_ZeroSizeReturnsNullptrInSafeMode) { EXPECT_EQ(nullptr, tlsf.malloc(0)); }

TEST_F(ExternallyManagedSafeTLSFTests, MAlloc_NearTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize - 1)); }

TEST_F(ExternallyManagedSafeTLSFTests, MAlloc_EqualToTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize)); }

TEST_F(ExternallyManagedSafeTLSFTests, MAlloc_GreaterThanTLSFSizeReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.malloc(tlsfSize + 1)); }

TEST_F(ExternallyManagedSafeTLSFTests, Malloc_OutofMemoryAllocation_ReturnsNullptrInSafeMode)
{
    static_cast<void>(tlsf.malloc(tlsfSize - 128));
    EXPECT_EQ(nullptr, tlsf.malloc(1_KB));
}

TEST_F(ExternallyManagedSafeTLSFTests, Malloc_OutofMemoryDueToFragmentation_ReturnsNullptrInSafeMode)
{
    const auto mem1                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem2 = tlsf.malloc(500_KB);
    const auto mem3                  = tlsf.malloc(500_KB);
    [[maybe_unused]] const auto mem4 = tlsf.malloc(250_KB);
    tlsf.mfree(mem1);
    tlsf.mfree(mem3);
    EXPECT_EQ(nullptr, tlsf.malloc(1_MB));
}


TEST_P(SafeTLSFAlignmentNonPowersOfTwo, ExternallyManagedTLSF_MAlloc_NonPowerOfTwoAlignmentReturnsNullptrInSafeMode)
{
    const auto buffer = new uint8_t[1_KB];
    pmm::TLSF<pmm::MemPolicy::External, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe> tlsf{ buffer, 1_KB };
    EXPECT_EQ(nullptr, tlsf.malloc(24, this->GetParam()));
    delete[] buffer;
}


TEST_F(ExternallyManagedSafeTLSFTests, Resize_ToZeroSizeReturnsNullptrInSafeMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 128, 0));
}


TEST_P(SafeTLSFAlignmentNonPowersOfTwo, ExternallyManagedTLSF_Resize_NonPowerOfTwoAlignmentReturnsNullptrInSafeMode)
{
    const auto buffer = new uint8_t[1_KB];
    pmm::TLSF<pmm::MemPolicy::External, pmm::TelPolicy::Disabled, pmm::SafeModePolicy::Safe> tlsf{ buffer, 1_KB };
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 128, 256, this->GetParam()));
    delete[] buffer;
}


TEST_F(ExternallyManagedSafeTLSFTests, Resize_NullptrReturnsNullptrInSafeMode)
{ EXPECT_EQ(nullptr, tlsf.resize(nullptr, 64, 128)); }


TEST_F(ExternallyManagedSafeTLSFTests, Resize_FromZeroSizeReturnsNullptrInSafeMode)
{
    const auto mem = tlsf.malloc(128);
    EXPECT_EQ(nullptr, tlsf.resize(mem, 0, 128));
}


TEST_F(ExternallyManagedSafeTLSFTests, Resize_GreaterThanTLSFSizeReturnsNullptrInSafeMode)
{
    const auto mem1 = tlsf.malloc(1500_KB);
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 1500_KB, 2048_KB));
}


TEST_F(ExternallyManagedSafeTLSFTests, Resize_OutofMemoryAllocation_ReturnsNullptrInSafeMode)
{
    const auto mem1 = tlsf.malloc(400_KB);
    static_cast<void>(tlsf.malloc(1400_KB));
    EXPECT_EQ(nullptr, tlsf.resize(mem1, 400_KB, 800_KB));
}

TEST_F(ExternallyManagedSafeTLSFTests, Resize_OutofMemoryDueToFragmentation_ReturnsNullptrInSafeMode)
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


TEST_F(ExternallyManagedSafeTLSFTests, MFree_NullptrReturnsNullptrInSafeMode)
{ EXPECT_DEBUG_DEATH(tlsf.mfree(nullptr), ""); }


#endif
