/**
 * @file BitTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 09, 2026
 *
 * @brief Verifies bit manipulation functions defined in utils/Bits.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Types.h"

#include <gtest/gtest.h>
#include <limits>
#include <peregrine/utils/Bit.h>


/**
 * @addtogroup T_PMM_Helpers
 * @{
 */


/**************************************
 *          TEST SETUP                *
 **************************************/

namespace
{

    /**
     * @brief Test fixture for bit manipulation methods across different types.
     * @tparam T The numeric type to bit values.
     */
    template <typename T>
    class BitUtilsTypedTests: public testing::Test
    {
    public:
        T value = std::numeric_limits<T>::max();
        // Max value will have the MSB set to 1 if unsigned and to second bit otherwise
        // (since first bit is used for sign)
        // fls = sizeof(T) * 8 (MSB) - (1 if T is signed or 0) - 1(since we are looking for a zero indexed value).
        T fls = sizeof(T) * 8 - (std::is_signed_v<T> ? 1 : 0) - 1;
        // Max value will also have the LSB set to 1.
        T ffs = 0;
    };
    TYPED_TEST_SUITE(BitUtilsTypedTests, TestIntegralTypes);


    /// Parameterized test fixture for FFS. The pair values indicate <input, expected_index> pairs.
    class BitUtilsFFSTest: public testing::TestWithParam<std::pair<uint64_t, size_t>>
    {};
    INSTANTIATE_TEST_SUITE_P(BitUtilsFFSTests, BitUtilsFFSTest,
                             testing::Values(std::make_pair(0x1ULL, 0), std::make_pair(0x0000000100000000ULL, 32),
                                             std::make_pair(0x8000000000000000ULL, 63),
                                             std::make_pair(0xFFFFFFFFFFFFFFFFULL, 0), std::make_pair(0x2ULL, 1),
                                             std::make_pair(0x4ULL, 2), std::make_pair(0x8ULL, 3),
                                             std::make_pair(0xFFFFFFFF00000000ULL, 32)));

    /// Parameterized test fixture for FLS. The pair values indicate <input, expected_index> pairs.
    class BitUtilsFLSTest: public testing::TestWithParam<std::pair<uint64_t, size_t>>
    {};
    INSTANTIATE_TEST_SUITE_P(BitUtilsFLSTests, BitUtilsFLSTest,
                             testing::Values(std::make_pair(0x1ULL, 0), std::make_pair(0x0000000100000000ULL, 32),
                                             std::make_pair(0x8000000000000000ULL, 63),
                                             std::make_pair(0xFFFFFFFFFFFFFFFFULL, 63), std::make_pair(0x2ULL, 1),
                                             std::make_pair(0x4ULL, 2), std::make_pair(0x8ULL, 3),
                                             std::make_pair(0xFFFFFFFF00000000ULL, 63)));

    /**************************************
     *          STATIC TESTS              *
     **************************************/
    namespace static_tests
    {}
} // namespace


TYPED_TEST(BitUtilsTypedTests, FFS_WorksAcrossDifferentIntegralTypes)
{ EXPECT_EQ(this->ffs, pmm::utils::ffs(this->value)); }


TYPED_TEST(BitUtilsTypedTests, FLS_WorksAcrossDifferentIntegralTypes)
{ EXPECT_EQ(this->fls, pmm::utils::fls(this->value)); }


TEST_P(BitUtilsFFSTest, ReturnsAValidIndex)
{
    const auto [num, expectedIndex] = GetParam();
    EXPECT_EQ(expectedIndex, pmm::utils::ffs(num));
}


TEST_P(BitUtilsFLSTest, ReturnsAValidIndex)
{
    const auto [num, expectedIndex] = GetParam();
    EXPECT_EQ(expectedIndex, pmm::utils::fls(num));
}


/** @} */
