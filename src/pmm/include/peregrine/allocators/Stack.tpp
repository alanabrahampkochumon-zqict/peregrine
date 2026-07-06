#pragma once
/**
 * @file Stack.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: June 20, 2026
 *
 * @brief Implementation of member functions defined in Stack.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

namespace pmm
{

    inline Stack::Stack(const std::size_t sizeInBytes) noexcept
        : _buffer(new uint8_t[sizeInBytes]), _size(sizeInBytes), _offset(0)
    {}


    constexpr std::size_t Stack::size() const noexcept
    {
        return _size;
    }

} // namespace pmm