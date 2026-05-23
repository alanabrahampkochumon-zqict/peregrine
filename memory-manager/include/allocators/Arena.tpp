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
        _prevOffset = 0;
    }

    Arena::Arena(const std::size_t bytes, const std::size_t alignment) noexcept
    {
        _buffer = new uint8_t[bytes];
        _sizeInBytes = bytes;
        // Sets the offset
        _offset = 0;
        _alignForward(alignment);

        _prevOffset= _offset;
    }


    Arena::~Arena() noexcept {
        delete[] _buffer;
    }


    inline Arena::Arena(Arena &&arena) noexcept {
        // Move the data members and null-out the moved data members.
        _buffer = std::exchange(arena._buffer, nullptr);
        _offset = std::exchange(arena._offset, 0);
        _prevOffset = std::exchange(arena._prevOffset, 0);
        _sizeInBytes = std::exchange(arena._sizeInBytes, 0);
    }


    inline Arena &Arena::operator=(Arena &&arena) noexcept {
        // Release the buffer held by the current arena
        delete[] _buffer; // TODO: Update as we move to HAL

        // Move the data members and null-out the moved data members.
        _buffer = std::exchange(arena._buffer, nullptr);
        _offset = std::exchange(arena._offset, 0);
        _prevOffset = std::exchange(arena._prevOffset, 0);
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
        const auto misalignment = absoluteAddress & (alignment - 1);
        // & (alignment - 1) skips the need for branching since
        // we are masking-off bits greater than or equal to alignment.
        // So, when we hit a misalignment of 0, and the result of subtraction go to alignment,
        // essentially cancelling it out to 0
        // E.g: 64 -> 1 0 0 0 0 0 0 (Alignment)
        //      63 -> 0 1 1 1 1 1 1 (Mask)
        //
        //      24 -> 0 0 1 1 0 0 0 (Padding Required)
        //      24 -> 0 0 1 1 0 0 0 (Offset Increment)
        //
        //      64 -> 1 0 0 0 0 0 0 (Padding Required)
        //       0 -> 0 0 0 0 0 0 0 (Offset Increment)
        _offset += (alignment - misalignment) & (alignment - 1);
    }


    inline void* Arena::allocBytes(const std::size_t bytes, const std::size_t alignment) {
        _alignForward(alignment);

        // Check if the arena has enough memory for allocation
        if (_sizeInBytes >= _offset + bytes) {
            void* ptr = &_buffer[_offset];

            _prevOffset = _offset;
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
        // Forward align the memory by the types default alignment
        _alignForward(alignof(T));

        if (constexpr auto objectSize = sizeof(T); _sizeInBytes >= _offset + objectSize)
        {
            // Allocate memory in the arena.
            void* raw = &_buffer[_offset];
            _prevOffset = _offset;
            _offset += objectSize;

            // Instantiate the object with arguments.
            T* object = new (raw) T(std::forward<Args>(args)...);
            return object;
        }

        // Return nullptr if the requested memory cannot be allocated
        return nullptr;
    }


    template <typename T, typename... Args>
    constexpr T* Arena::allocAs(std::size_t alignment, Args... args)
    {
        // Forward align the memory by the alignment
        _alignForward(alignment);

        if (constexpr auto objectSize = sizeof(T); _sizeInBytes >= _offset + objectSize)
        {
            // Allocate memory in the arena.
            void* raw = &_buffer[_offset];
            _prevOffset = _offset;
            _offset += objectSize;

            // Instantiate the object with arguments.
            T* object = new (raw) T(std::forward<Args>(args)...);
            return object;
        }

        // Return nullptr if the requested memory cannot be allocated
        return nullptr;

    }

} // namespace pmm
