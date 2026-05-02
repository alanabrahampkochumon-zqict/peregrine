#include <gtest/gtest.h>

#include "Utils.h"

#include <utils/Constants.h>
#include <cstdint>



template <typename T>
class UserDefinedLiteralParam
{
public:
	T size, expectedBytes;
};
/** Test fixture for @ref pmm::constant user-defined literals, parameterized by @ref UserDefinedLiteralParam. */
class UserDefinedLiteralTest : public ::testing::TestWithParam<UserDefinedLiteralParam<uint64_t>> {};



/** Verify that the @ref pmm::constant primitives are initialized to the correct size. */
namespace
{
	using namespace pmm::constants;
	static_assert(BYTE == 1);
	static_assert(WORD == 2);
	static_assert(DWORD == 4);
	static_assert(QWORD == 8);
	static_assert(KB == 1024);
	static_assert(MB == 1048576);
	static_assert(GB == 1073741824);
	static_assert(TB == 1099511627776);

	// User defined literals
	static_assert(5_B == 5);
	static_assert(10_KB == 10240);
	static_assert(10_MB == 10485760);
	static_assert(10_GB == 10737418240);
	static_assert(10_TB == 10995116277760);
}



TEST_P()