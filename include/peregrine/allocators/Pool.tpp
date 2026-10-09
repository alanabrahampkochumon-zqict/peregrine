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



#include "peregrine/memory/Memory.h"
#include "peregrine/utils/Helpers.h"
#include "peregrine/utils/Preprocessors.h"

#include <format>
#include <utility>


namespace pmm
{

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::Pool(
        const size_t poolSize, const size_t chuckSize, const size_t chunkAlignment) noexcept
        requires(MemoryPolicy == MemPolicy::Internal)
        : _buffer{ static_cast<uint8_t*>(memAlloc(poolSize)) },
          _poolSize{ poolSize },
          _chunkSize{ chuckSize },
          _chunkAlignment{ chunkAlignment },
          _head(nullptr),
          _telemetry{ getTelemetryInstance<TelemetryPolicy>(poolSize, chuckSize, chunkAlignment) }
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
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::Pool(
        uint8_t* backingBuffer, const size_t bufferSize, const size_t chuckSize, const size_t chunkAlignment) noexcept
        requires(MemoryPolicy == MemPolicy::External)
        : _buffer{ backingBuffer },
          _poolSize{ bufferSize },
          _chunkSize{ chuckSize },
          _chunkAlignment{ chunkAlignment },
          _telemetry{ getTelemetryInstance<TelemetryPolicy>(bufferSize, chuckSize, chunkAlignment) }
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
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::Pool(Pool&& pool) noexcept
        : _buffer{ std::exchange(pool._buffer, nullptr) },
          _poolSize{ pool._poolSize },
          _chunkSize{ pool._chunkSize },
          _chunkAlignment{ pool._chunkAlignment },
          _initialAlignmentPadding{ pool._initialAlignmentPadding },
          _chunkCount{ pool._chunkCount },
          _head{ std::exchange(pool._head, nullptr) },
          _telemetry{ std::exchange(pool._telemetry,
                                    getTelemetryInstance<TelemetryPolicy>(_poolSize, _chunkSize, _chunkAlignment)) }
    {}


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>& Pool<
        MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::operator=(Pool&& pool) noexcept
    {
        // For self assignment return the current pool.
        if (this == &pool)
        {
            return *this;
        }

        if constexpr (MemoryPolicy == MemPolicy::Internal)
        {
            // Release the buffer held by the current pool (ONLY applicable for managed pool)
            memFree(_buffer, _poolSize);
        }

        // Move the data members and null-out the moved data members.
        _buffer                  = std::exchange(pool._buffer, nullptr);
        _poolSize                = pool._poolSize;
        _chunkSize               = pool._chunkSize;
        _chunkAlignment          = pool._chunkAlignment;
        _initialAlignmentPadding = pool._initialAlignmentPadding;
        _chunkCount              = pool._chunkCount;
        _head                    = std::exchange(pool._head, nullptr);
        _telemetry               = std::exchange(pool._telemetry,
                                                 getTelemetryInstance<TelemetryPolicy>(_poolSize, _chunkSize, _chunkAlignment));

        return *this;
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr size_t Pool<MemoryPolicy, TelemetryPolicy, SafeMode,
                                     MultithreadingPolicy>::getMaxAllocationCount() const noexcept
    { return _chunkCount; }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::allocChunk() noexcept
    {
        // Return the current head's address and move the head forward
        const auto node = _head;

        PMM_ASSERT_MSG(node != nullptr, "Pool allocator has no free memory");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (node == nullptr)
            {
                return nullptr;
            }
        }

        _telemetry.logAlloc();

        _head = _head->next;
        return node;
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T, typename... Args>
    PMM_INLINE T* Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::alloc(Args... args) noexcept
    {
        PMM_ASSERT_MSG(sizeof(T) <= _chunkSize,
                       std::format("Size of object({}) exceeds chunk size({})", sizeof(T), _chunkSize).c_str());
        auto rawBuffer = allocChunk();
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            if (rawBuffer == nullptr)
            {
                return nullptr;
            }
        }
        return new (rawBuffer) T(std::forward<Args>(args)...);
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE bool Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::freeChunk(void* ptr) noexcept
    {
        PMM_ASSERT_MSG(ptr != nullptr, "Cannot free a nullptr");
        [[maybe_unused]] const auto minFreeAddr = _buffer + _initialAlignmentPadding;
        [[maybe_unused]] const auto maxFreeAddr = _buffer + _poolSize - _initialAlignmentPadding - _chunkSize;
        PMM_ASSERT_MSG(ptr >= minFreeAddr && ptr <= maxFreeAddr, "Out of bounds free");
        if constexpr (SafeMode == SafeModePolicy::Safe)
        {
            // Validate if the free is possible
            if (ptr == nullptr || ptr < minFreeAddr || ptr > maxFreeAddr ||
                (reinterpret_cast<uintptr_t>(ptr) & (_chunkSize - 1)) != 0)
            {
                return false;
            }
        }

        _telemetry.logFree();
        const auto freeNode = static_cast<PoolFreeNode*>(ptr);
        freeNode->next      = _head;
        _head               = freeNode;
        return true;
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T>
    PMM_INLINE bool Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::free(T* ptr) noexcept
    {
        // Invoke the dtor if the type is not trivially destructible
        if (!std::is_trivially_destructible_v<T>)
        {
            ptr->~T();
        }
        // Free the memory
        return freeChunk(ptr);
    }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::clear()
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


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE Pool<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::~Pool() noexcept
        requires(MemoryPolicy == MemPolicy::Internal)
    { memFree(_buffer, _poolSize); }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr const PoolTelemetryType<TelemetryPolicy>& Pool<
        MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::getTelemetry() const noexcept
    { return _telemetry; }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr bool Pool<MemoryPolicy, TelemetryPolicy, SafeMode,
                                   MultithreadingPolicy>::isTelemetryEnabled() noexcept
    { return TelemetryPolicy == TelPolicy::Enabled; }

} // namespace pmm
