/**
 * @file Pong.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 18, 2026
 *
 * @brief Pong member function implementation.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Pong.h"

#include <print>


namespace pong
{

    bool Game::initialize() noexcept
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("An error occurred while initializing SDL\n%s", SDL_GetError());
            return false;
        }


        _window = SDL_CreateWindow(GAME_NAME.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (_window == nullptr)
        {
            SDL_Log("An error occurred while creating the window\n%s", SDL_GetError());
            return false;
        }


        _renderer = SDL_CreateRenderer(_window, nullptr);
        if (_renderer == nullptr)
        {
            SDL_Log("An error occurred while creating the renderer\n%s", SDL_GetError());
            return false;
        }


        // After initializing all the subsystem signal that the game can start running.
        _isRunning = true;

        return true;
    }

    void Game::run() noexcept
    {
        while (_isRunning)
        {
            processInput();
            updateGame();
            generateOutput();
        }
    }


    void Game::shutdown() const noexcept
    {
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }




    //-+-+-+-+-+-+-+-+-+-+-+
    // PRIVATE METHODS
    //-+-+-+-+-+-+-+-+-+-+-+

    void Game::processInput()
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

        /// Get and process the keyboard state
        const auto kbState = SDL_GetKeyboardState(nullptr);

        // Quit the application if ESC key is pressed
        if (kbState[SDL_SCANCODE_ESCAPE])
        {
            _isRunning = false;
        }
    }


    void Game::updateGame() {}

    void Game::generateOutput()
    {
        // Set the draw color
        SDL_SetRenderDrawColor(_renderer, 0, 60, 120, 255);

        // Render the clear color
        SDL_RenderClear(_renderer);

        // Swap buffers
        SDL_RenderPresent(_renderer);
    }
} // namespace pong
