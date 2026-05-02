#pragma once
/**
 * @file Constants.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Definition and implementation of memory constant literals.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <cstddef>


namespace pmm::constants
{
	inline constexpr std::size_t BYTE = 1; ///< Byte Size of a byte.
	inline constexpr std::size_t WORD = 2; ///< Byte Size of a Word.
	inline constexpr std::size_t DWORD = 4; ///< Byte size of a DoubleWord.
	inline constexpr std::size_t QWORD = 8; ///< Byte size of a QuadWord.
	inline constexpr std::size_t KB = 1024; ///< Byte size of 1 Kilobyte.
	inline constexpr std::size_t MB = KB * KB; ///< Byte size of 1 Megabyte.
	inline constexpr std::size_t GB = KB * MB; ///< Byte size of 1 Gigabyte.
	inline constexpr std::size_t TB = KB * GB; ///< Byte size of 1 Terabyte.

	//constexpr std::size_t operator""MB(const std::size_t byte) noexcept
	//{
	//	return M
	//}
}

