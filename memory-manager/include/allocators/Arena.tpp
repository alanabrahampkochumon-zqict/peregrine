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
#include <new>


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
        delete[] _buffer; // TODO: Update as we move to HAL

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


    inline void Arena::_alignForward(const std::size_t alignment) noexcept {
        // To make sure alignment is the power of 2
        assert(std::has_single_bit(alignment));

        // Base address of the pointer
        const auto absoluteBaseAddress = reinterpret_cast<uintptr_t>(_buffer);
        // Current memory address
        const auto absoluteAddress = absoluteBaseAddress + _offset;

        // Modulo arithmetic for numbers that are power of two
        // Bytes = 13 -> 1101
        // Alignment = 4 -> 0100
        // Modulo = 1 -> 1101 & 0011 (4 - 1 = 3) = 01
        if (const auto modulo = absoluteAddress & (alignment - 1); modulo != 0) {
            const uintptr_t alignedAddress = absoluteAddress + (alignment - modulo);
            // Convert the offset back to a relative offset
            _offset = alignedAddress - absoluteBaseAddress;
        }

    }


    inline void* Arena::allocBytes(const std::size_t bytes, const std::size_t alignment) {
        _alignForward(alignment);

        if (_sizeInBytes >= _offset + bytes) {
            void* ptr = &_buffer[_offset];

            _offset += bytes;

            memset(ptr, 0, bytes); // TODO: Remove when moving to HAL
            return ptr;
        }

        // Return nullptr if the requested memory cannot be allocated
        return nullptr;
    }

    template <typename T, typename... Args>
    constexpr T* Arena::alloc(Args... args)
    {
        if (constexpr auto objectSize = sizeof(T); _sizeInBytes >= _offset + objectSize)
        {
            // Allocate memory in the arena.
            void* raw = &_buffer[_offset];
            _offset += objectSize;

            // Instantiate the object with arguments.
            T* object = new (raw) T(std::forward<Args>(args)...);
            return object;
        }

        // Return nullptr if the requested memory cannot be allocated
        return nullptr;
    }

} // namespace pmm
