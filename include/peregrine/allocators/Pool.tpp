#pragma once
/**
 * @file Pool.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: July 27, 2026
 *
 * @brief Implementation of member functions defined in Pool.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */



#include "peregrine/utils/Helpers.h"
#include "peregrine/utils/Preprocessors.h"

#include <format>
#include <utility>


namespace pmm
{

    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr Pool<MemStrategy, TelPolicy>::Pool(const size_t poolSize, const size_t chuckSize,
                                                            const size_t chunkAlignment) noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
        : _buffer{ new uint8_t[poolSize] },
          _poolSize{ poolSize },
          _chunkSize{ chuckSize },
          _chunkAlignment{ chunkAlignment },
          _head(nullptr),
          _telemetry{ getTelemetryInstance<TelPolicy>(poolSize, chuckSize, chunkAlignment) }
    {
        const auto baseAddress = reinterpret_cast<uintptr_t>(_buffer);

        // Align base address and chunk size to the target padding.
        _initialAlignmentPadding = calcAlignmentPadding(baseAddress, chunkAlignment);
        _chunkSize += calcAlignmentPadding(_chunkSize, chunkAlignment);
        _chunkCount = (poolSize - _initialAlignmentPadding) / _chunkSize;

        PMM_ASSERT_MSG(_chunkSize >= sizeof(PoolFreeNode), "Inadequate chunk size");
        PMM_ASSERT_MSG(_poolSize - _initialAlignmentPadding >= _chunkSize, "Backing buffer smaller than chunk size");

        clear();

        // Telemetry
        // Since we are incrementing the chunk size we can use the member variable here
        // instead of parameter
        _telemetry.setAlignedChunkSize(_chunkSize);
        _telemetry.setPadding(_initialAlignmentPadding);

        // TODO: Update buffer init to HAL
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr Pool<MemStrategy, TelPolicy>::Pool(uint8_t* backingBuffer, const size_t bufferSize,
                                                            const size_t chuckSize,
                                                            const size_t chunkAlignment) noexcept
        requires std::same_as<MemStrategy, UnmanagedMemory>
        : _buffer{ backingBuffer },
          _poolSize{ bufferSize },
          _chunkSize{ chuckSize },
          _chunkAlignment{ chunkAlignment },
          _telemetry{ getTelemetryInstance<TelPolicy>(bufferSize, chuckSize, chunkAlignment) }
    {
        const auto baseAddress = reinterpret_cast<uintptr_t>(_buffer);

        // Align base address and chunk size to the target padding.
        _initialAlignmentPadding = calcAlignmentPadding(baseAddress, chunkAlignment);
        _chunkSize += calcAlignmentPadding(_chunkSize, chunkAlignment);
        _chunkCount = (bufferSize - _initialAlignmentPadding) / _chunkSize;

        PMM_ASSERT_MSG(_chunkSize >= sizeof(PoolFreeNode), "Inadequate chunk size");
        PMM_ASSERT_MSG(_poolSize - _initialAlignmentPadding >= _chunkSize, "Backing buffer smaller than chunk size");

        clear();

        // Telemetry
        // Since we are incrementing the chunk size we can use the member variable here
        // instead of parameter
        _telemetry.setAlignedChunkSize(_chunkSize);
        _telemetry.setPadding(_initialAlignmentPadding);

        // TODO: Update buffer init to HAL
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr Pool<MemStrategy, TelPolicy>::Pool(Pool&& pool) noexcept
        : _buffer{ std::exchange(pool._buffer, nullptr) },
          _poolSize{ std::exchange(pool._poolSize, 0) },
          _chunkSize{ std::exchange(pool._chunkSize, 0) },
          _chunkAlignment{ std::exchange(pool._chunkAlignment, 0) },
          _initialAlignmentPadding{ std::exchange(pool._initialAlignmentPadding, 0) },
          _chunkCount{ std::exchange(pool._chunkCount, 0) },
          _head{ std::exchange(pool._head, nullptr) },
          _telemetry{ std::exchange(pool._telemetry,
                                    getTelemetryInstance<TelPolicy>(_poolSize, _chunkSize, _chunkAlignment)) }
    {}


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr Pool<MemStrategy, TelPolicy>& Pool<MemStrategy, TelPolicy>::operator=(Pool&& pool) noexcept
    {
        // For self assignment return the current pool.
        if (this == &pool)
        {
            return *this;
        }

        if constexpr (std::same_as<MemStrategy, ManagedMemory>)
        {
            // Release the buffer held by the current pool (ONLY applicable for managed pool)
            delete[] _buffer; // TODO: Update as we move to HAL
        }

        // Move the data members and null-out the moved data members.
        _buffer                  = std::exchange(pool._buffer, nullptr);
        _poolSize                = std::exchange(pool._poolSize, 0);
        _chunkSize               = std::exchange(pool._chunkSize, 0);
        _chunkAlignment          = std::exchange(pool._chunkAlignment, 0);
        _initialAlignmentPadding = std::exchange(pool._initialAlignmentPadding, 0);
        _chunkCount              = std::exchange(pool._chunkCount, 0);
        _head                    = std::exchange(pool._head, nullptr);
        _telemetry =
            std::exchange(pool._telemetry, getTelemetryInstance<TelPolicy>(_poolSize, _chunkSize, _chunkAlignment));

        return *this;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr size_t Pool<MemStrategy, TelPolicy>::getMaxAllocationCount() const noexcept
    { return _chunkCount; }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE void* Pool<MemStrategy, TelPolicy>::allocChunk() noexcept
    {
        // Return the current head's address and move the head forward
        const auto node = _head;

        PMM_ASSERT_MSG(node != nullptr, "Pool allocator has no free memory");
        _telemetry.logAlloc();

        _head = _head->next;
        return memset(node, 0, _chunkSize);
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    template <typename T, typename... Args>
    PMM_INLINE T* Pool<MemStrategy, TelPolicy>::alloc(Args... args) noexcept
    {
        PMM_ASSERT_MSG(sizeof(T) <= _chunkSize,
                       std::format("Size of object({}) exceeds chunk size({})", sizeof(T), _chunkSize).c_str());
        auto rawBuffer = allocChunk();
        return new (rawBuffer) T(std::forward<Args>(args)...);
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE void Pool<MemStrategy, TelPolicy>::freeChunk(void* ptr) noexcept
    {
        PMM_ASSERT_MSG(ptr != nullptr, "Cannot free a nullptr");
        [[maybe_unused]] const auto minFreeAddr = _buffer + _initialAlignmentPadding;
        [[maybe_unused]] const auto maxFreeAddr = _buffer + _poolSize - _initialAlignmentPadding - _chunkSize;
        PMM_ASSERT_MSG(ptr >= minFreeAddr && ptr <= maxFreeAddr, "Out of bounds free");
        // TODO: Add this assertion for out of alignment free.
        // PMM_ASSERT_MSG(reinterpret_cast<uintptr_t>(ptr) % _chunkSize == 0, "Cannot free invalid address");

        _telemetry.logFree();
        const auto freeNode = static_cast<PoolFreeNode*>(ptr);
        freeNode->next      = _head;
        _head               = freeNode;
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    template <typename T>
    PMM_INLINE void Pool<MemStrategy, TelPolicy>::free(T* ptr) noexcept
    {
        // Invoke the dtor if the type is not trivially destructible
        if (!std::is_trivially_destructible_v<T>)
        {
            ptr->~T();
        }
        // Free the memory
        freeChunk(ptr);
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE void Pool<MemStrategy, TelPolicy>::clear()
    {
        // Required as some compilers put pattern in debug mode
        // when the buffer is user provided.
        _head = nullptr;

        // Head -> Last Address -> Second Last Address -> .... -> nullptr
        // Iterate through each chunk, and set it's header to point to the next node
        // and make that node the current pool head.
        for (size_t i = 0; i < _chunkCount; ++i)
        {
            // Since we are storing alignment padding separately,
            // we need to account that when taking the address.
            const auto baseAddress = &_buffer[_initialAlignmentPadding + (i * _chunkSize)];
            const auto freeNode    = reinterpret_cast<PoolFreeNode*>(baseAddress);
            freeNode->next         = _head;
            _head                  = freeNode;
        }
        _telemetry.logClear();
    }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE Pool<MemStrategy, TelPolicy>::~Pool() noexcept
        requires std::same_as<MemStrategy, ManagedMemory>
    { delete[] _buffer; }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr const PoolTelemetryType<TelPolicy>& Pool<MemStrategy, TelPolicy>::getTelemetry() const noexcept
    { return _telemetry; }


    template <MemoryStrategy MemStrategy, telemetry::TelemetryPolicy TelPolicy>
    PMM_INLINE constexpr bool Pool<MemStrategy, TelPolicy>::isTelemetryEnabled() noexcept
    { return std::same_as<TelPolicy, telemetry::Enabled>; }

} // namespace pmm
