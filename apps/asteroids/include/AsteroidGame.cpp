/**
 * @file AsteroidGame.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 20, 2026
 *
 * @brief Implementation of declarations defined in AsteroidGame.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "AsteroidGame.h"

namespace asteroids
{

    bool AsteroidGame::initialize() noexcept
    {
        _isRunning = true;
        return true;
    }

    void AsteroidGame::run() noexcept
    {
        while (_isRunning)
        {
            _handleInput();
            _update();
            _draw();
        }
    }

    void AsteroidGame::shutdown() const noexcept
    {
        // PERFORM CLEANUP
    }

    void AsteroidGame::_handleInput() {}

    void AsteroidGame::_update() {}

    void AsteroidGame::_draw() {}

} // namespace asteroids
