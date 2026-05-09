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


    constexpr Arena::Arena(Arena &&arena) noexcept {
        // Transfer the resources to the current instance
        _buffer = arena._buffer;
        _offset = arena._offset;
        _sizeInBytes = arena._sizeInBytes;

        // Nullify the source buffer to prevent double-free.
        arena._buffer = nullptr;
        arena._offset = 0;
        arena._sizeInBytes = 0;
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
} // namespace pmm
