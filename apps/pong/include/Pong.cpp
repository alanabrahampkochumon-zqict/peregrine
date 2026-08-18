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
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }




    //-+-+-+-+-+-+-+-+-+-+-+
    // PRIVATE METHODS
    //-+-+-+-+-+-+-+-+-+-+-+

    void Game::processInput() {}

    void Game::updateGame() {}

    void Game::generateOutput() {}
} // namespace pong
