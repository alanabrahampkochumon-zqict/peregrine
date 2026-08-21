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

#include "SDL3/SDL.h"

namespace asteroids
{

    bool AsteroidGame::initialize() noexcept
    {
        SDL_SetAppMetadata(GAME_NAME, GAME_VERSION, GAME_ID);

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("There was an error initializing SDL.\n%s", SDL_GetError());
            return false;
        }

        _window = SDL_CreateWindow(GAME_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (!_window)
        {
            SDL_Log("There was an error creating window.\n%s", SDL_GetError());
            return false;
        }


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
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    void AsteroidGame::_handleInput()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    _isRunning = false;
                    break;
                default:
                    break;
            }
        }
    }

    void AsteroidGame::_update()
    {
        while (SDL_GetTicks() < lastFrameTick + 16) {} // Frame Limiting to 60 FPS

        const auto currentTick = SDL_GetTicks();
        float deltaTime  = static_cast<float>(currentTick - lastFrameTick) * 0.001f;
        lastFrameTick = currentTick;

#ifndef NDEBUG
        // Clamp maximum delta time (useful for debug breaks)
        deltaTime = deltaTime > 0.05f ? 0.05f : deltaTime;
#endif

        SDL_Log("Delta time: %0.03f", deltaTime);
    }

    void AsteroidGame::_draw() {}

} // namespace asteroids
