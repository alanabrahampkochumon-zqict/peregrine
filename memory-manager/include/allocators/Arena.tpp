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


#include <bit>
#include <cassert>
#include <cstring>
#include <new>
#include <utility>

// TODO: Add tests to verify that the ownership of passed in arena telemetry is upto the class

namespace pmm
{
    /**************************************
     *                                    *
     *           CONSTRUCTORS             *
     *                                    *
     **************************************/

    Arena::Arena(const std::size_t bytes) noexcept
        : _buffer(new uint8_t[bytes]),
          _sizeInBytes(bytes),
          _offset(0),
          _prevOffset(0),
          _defaultAlignment(0),
          _telemetry{ new ArenaTelemetry(bytes) }
    {}

    inline Arena::Arena(const std::size_t bytes, ArenaTelemetry* telemetry) noexcept
        : _buffer(new uint8_t[bytes]),
          _sizeInBytes(bytes),
          _offset(0),
          _prevOffset(0),
          _defaultAlignment(0),
          _telemetry(telemetry)
    {}


    Arena::Arena(const std::size_t bytes, const std::size_t alignment) noexcept
        : _buffer(new uint8_t[bytes]),
          _sizeInBytes(bytes),
          _offset(0),
          _prevOffset(0),
          _defaultAlignment(alignment),
          _telemetry{ new ArenaTelemetry(bytes) }
    {
        // Sets the offset based on alignment
        if (alignment > 0)
        {
            _alignForward(alignment);
            _prevOffset = _offset;
        }
    }


    inline Arena::Arena(const std::size_t bytes, const std::size_t alignment, ArenaTelemetry* telemetry) noexcept
        : _buffer(new uint8_t[bytes]),
          _sizeInBytes(bytes),
          _offset(0),
          _prevOffset(0),
          _defaultAlignment(alignment),
          _telemetry(telemetry)
    {
        // Sets the offset based on alignment
        if (alignment > 0)
        {
            _alignForward(alignment);
            _prevOffset = _offset;
        }
    }


    Arena::~Arena() noexcept
    {
        delete[] _buffer;
    }


    inline Arena::Arena(Arena&& arena) noexcept
    {
        // TODO: Move to value init
        // Move the data members and null-out the moved data members.
        _buffer = std::exchange(arena._buffer, nullptr);
        _offset = std::exchange(arena._offset, 0);
        _prevOffset = std::exchange(arena._prevOffset, 0);
        _sizeInBytes = std::exchange(arena._sizeInBytes, 0);
        _defaultAlignment = std::exchange(arena._defaultAlignment, 0);
        _telemetry = std::exchange(arena._telemetry, nullptr);
    }


    inline Arena& Arena::operator=(Arena&& arena) noexcept
    {
        // For self assignment return the current arena.
        if (this == &arena)
        {
            return *this;
        }

        // Release the buffer held by the current arena
        delete[] _buffer; // TODO: Update as we move to HAL

        // Move the data members and null-out the moved data members.
        _buffer = std::exchange(arena._buffer, nullptr);
        _offset = std::exchange(arena._offset, 0);
        _prevOffset = std::exchange(arena._prevOffset, 0);
        _sizeInBytes = std::exchange(arena._sizeInBytes, 0);
        _defaultAlignment = std::exchange(arena._defaultAlignment, 0);
        _telemetry = std::exchange(arena._telemetry, nullptr);

        return *this;
    }


    constexpr std::size_t Arena::freeSize() const noexcept
    {
        return _sizeInBytes - _offset;
    }


    constexpr std::size_t Arena::usedSize() const noexcept
    {
        return _offset;
    }


    constexpr std::size_t Arena::size() const noexcept
    {
        return _sizeInBytes;
    }

    /**
     * Align the "base address" of the arena's next allocation to @p alignment.
     * @param alignment The alignment to which the offset + base address is aligned to.
     */
    inline void Arena::_alignForward(const std::size_t alignment) noexcept
    {
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


    inline void* Arena::allocBytes(const std::size_t bytes, const std::size_t alignment) noexcept
    {
        _alignForward(alignment);

        // Check if the arena has enough memory for allocation
        if (_sizeInBytes >= _offset + bytes)
        {
            void* ptr = &_buffer[_offset];

            _prevOffset = _offset;
            _offset += bytes;

            memset(ptr, 0, bytes); // TODO: Remove when moving to HAL

            // Update the telemetry usage
            _telemetry->updateAllocationUsage(bytes);

            return ptr;
        }

        // Return nullptr if the requested memory cannot be allocated
        return nullptr;
    }


    template <typename T, typename... Args>
    constexpr T* Arena::alloc(Args... args) noexcept
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

            // Update the telemetry usage
            _telemetry->updateAllocationUsage(objectSize);

            return object;
        }

        // Return nullptr if the requested memory cannot be allocated
        return nullptr;
    }


    template <typename T, typename... Args>
    constexpr T* Arena::allocAs(const std::size_t alignment, Args... args) noexcept
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

            // Update the telemetry usage
            _telemetry->updateAllocationUsage(objectSize);

            return object;
        }

        // Return nullptr if the requested memory cannot be allocated
        return nullptr;
    }


    template <typename T>
    constexpr std::span<T> Arena::allocV(std::size_t count) noexcept
    {
        // Allocate the raw memory and wrap it in a span
        if (T* rawPointer = static_cast<T*>(allocBytes(sizeof(T) * count, alignof(T))))
            return std::span(rawPointer, count);
        return std::span<T>();
    }


    constexpr void Arena::freeAll() noexcept
    {
        _offset = 0;

        // Forward align by default alignment (if specified during construction)
        if (_defaultAlignment > 0)
            _alignForward(_defaultAlignment);

        _prevOffset = _offset;
    }


    constexpr void* Arena::resize(void* oldMemory, const std::size_t oldSize, const std::size_t newSize,
                                  const std::size_t alignment) noexcept
    {
        // The allocation is new
        if (oldMemory == nullptr || oldSize == 0)
            return allocBytes(newSize, alignment);

        // If the new size is smaller than the old size
        // No resizing required
        if (oldSize >= newSize)
            return oldMemory;

        // Check whether the old memory is the last allocation we made
        const auto allocationAddress = reinterpret_cast<uintptr_t>(oldMemory);
        const auto lastAllocatedAddress = reinterpret_cast<uintptr_t>(_buffer) + _prevOffset;
        const auto offsetDiff = newSize - oldSize;
        // If there is enough memory in the arena to "expand" last allocation
        // expand the offset to the difference between new size and old size
        if (allocationAddress == lastAllocatedAddress && _sizeInBytes >= _offset + offsetDiff)
        {
            _offset += offsetDiff;
            return oldMemory;
        }

        // The memory exists else where in the arena, so create a new byte chunk
        // copy the existing data and return it
        // @note: This leaves a "hole" where the previous allocation was
        if (_sizeInBytes >= _offset + newSize)
        {
            void* newLocation = allocBytes(newSize, alignment);
            memmove(newLocation, oldMemory, oldSize);
            return newLocation;
        }

        // Allocation not possible due to lack of memory in arena
        // So return a nullptr.
        return nullptr;
    }


    constexpr ArenaTelemetry Arena::getTelemetry() const noexcept
    {
        return *_telemetry;
    }

} // namespace pmm
