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
	inline constexpr std::size_t BYTE = 1;
	inline constexpr std::size_t WORD = 2;
	inline constexpr std::size_t DWORD = 4;
	inline constexpr std::size_t QWORD = 8;
	inline constexpr std::size_t KB = 1024;
	inline constexpr std::size_t MB = KB * KB;
	inline constexpr std::size_t GB = KB * MB;
	inline constexpr std::size_t TB = KB * GB;

	//constexpr std::size_t operator""MB(const std::size_t byte) noexcept
	//{
	//	return M
	//}
}

