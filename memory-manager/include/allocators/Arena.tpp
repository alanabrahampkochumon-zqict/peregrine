#pragma once
/**
 * @file Arena.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Implementation for Linear/Arena allocator member functions defined in
 *        Arena.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <utility>
#include <bit>
#include <cstring>
#include <cassert>

namespace pmm {
    /**************************************
     *                                    *
     *           INITIALIZERS             *
     *                                    *
     **************************************/

    Arena::Arena(const std::size_t bytes) noexcept {
        _buffer = new uint8_t[bytes];
        _sizeInBytes = bytes;
        _offset = 0;
    }


    Arena::~Arena() noexcept {
        delete[] _buffer;
    }


    inline Arena::Arena(Arena &&arena) noexcept {
        // Move the data members and null-out the moved data members.
        _buffer = std::exchange(arena._buffer, nullptr);
        _offset = std::exchange(arena._offset, 0);
        _sizeInBytes = std::exchange(arena._sizeInBytes, 0);
    }


    inline Arena &Arena::operator=(Arena &&arena) noexcept {
        // Release the buffer held by the current arena
        delete _buffer; // TODO: Update as we move to HAL

        // Move the data members and null-out the moved data members.
        _buffer = std::exchange(arena._buffer, nullptr);
        _offset = std::exchange(arena._offset, 0);
        _sizeInBytes = std::exchange(arena._sizeInBytes, 0);

        return *this;
    }


    constexpr std::size_t Arena::freeSize() const noexcept {
        return _sizeInBytes - _offset;
    }


    constexpr std::size_t Arena::usedSize() const noexcept {
        return _offset;
    }


    constexpr std::size_t Arena::size() const noexcept {
        return _sizeInBytes;
    }

    constexpr void alignForward(const std::size_t alignment) {
        assert(std::has_single_bit(alignment));
    }

    constexpr void *Arena::allocBytes(const std::size_t bytes, const std::size_t alignment) {
        // Bytes = 13 -> 1101
        // Alignment = 4 -> 0100
        // Modulo = 1 -> 1101 & 0011 (4 - 1 = 3) = 01
        // const auto modulo = bytes & (alignment - 1);
        //
        // if (const auto alignedSize = bytes + (alignment - modulo); _offset + alignedSize <= _sizeInBytes) {
        //     void* ptr = &_buffer[_offset];
        //     _offset += alignedSize;
        //
        //     memset(ptr, 0, alignedSize); // TODO: Remove when moving to HAL
        //     return ptr;
        // }

        // Return nullptr if the requested memory cannot be allocated
        return nullptr;
    }
} // namespace pmm
