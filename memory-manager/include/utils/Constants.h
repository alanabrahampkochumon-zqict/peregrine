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


#include <cstdint>
// TODO: Add custom assert

namespace pmm::constants
{

	inline constexpr uint64_t BYTE = 1;         ///< Size of a single byte.
	inline constexpr uint64_t WORD = 2;         ///< Size of a Word (2 bytes).
	inline constexpr uint64_t DWORD = 4;         ///< Size of a DoubleWord (4 bytes).
	inline constexpr uint64_t QWORD = 8;         ///< Size of a QuadWord (8 bytes).

	inline constexpr uint64_t KB = 1024;         ///< Size of 1 Kilobyte in bytes.
	inline constexpr uint64_t MB = KB * KB;      ///< Size of 1 Megabyte in bytes.
	inline constexpr uint64_t GB = KB * MB;      ///< Size of 1 Gigabyte in bytes.
	inline constexpr uint64_t TB = KB * GB;      ///< Size of 1 Terabyte in bytes.


	/**
	 * @brief User-defined literal for Byte assignment.
	 * @param[in] byte The amount of bytes.
	 * @return The equivalent size in bytes.
	 */
	constexpr uint64_t operator""_B(const unsigned long long byte) noexcept
	{
		return byte;
	}


	/**
	 * @brief User-defined literal for Kilobyte to byte conversion.
	 * @param[in] byte The amount of Kilobytes.
	 * @return The equivalent size in bytes.
	 */
	constexpr uint64_t operator""_KB(const unsigned long long byte) noexcept
	{
		return byte * KB;
	}


	/**
	 * @brief User-defined literal for Megabyte to byte conversion.
	 * @param[in] byte The amount of Megabytes.
	 * @return The equivalent size in bytes.
	 */
	constexpr uint64_t operator""_MB(const unsigned long long byte) noexcept
	{
		return byte * MB;
	}


	/**
	 * @brief User-defined literal for Gigabyte to byte conversion.
	 * @param[in] byte The amount of Gigabytes.
	 * @return The equivalent size in bytes.
	 */
	constexpr uint64_t operator""_GB(const unsigned long long byte) noexcept
	{
		return byte * GB;
	}


	/**
	 * @brief User-defined literal for Terabyte to byte conversion.
	 * @param[in] byte The amount of Terabytes.
	 * @return The equivalent size in bytes.
	 */
	constexpr uint64_t operator""_TB(const unsigned long long byte) noexcept
	{
		return byte * TB;
	}
}

