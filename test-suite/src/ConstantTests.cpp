#include <gtest/gtest.h>

#include "Utils.h"

#include <utils/Constants.h>


/** Verify that the @ref pmm::constant primitives are initialized to the correct size. */
namespace
{
	static_assert(pmm::constants::BYTE == 1);
	static_assert(pmm::constants::WORD == 2);
	static_assert(pmm::constants::DWORD == 4);
	static_assert(pmm::constants::QWORD == 8);
	static_assert(pmm::constants::KB == 1024);
	static_assert(pmm::constants::MB == 1048576);
	static_assert(pmm::constants::GB == 1073741824);
	static_assert(pmm::constants::TB == 1099511627776);
}
