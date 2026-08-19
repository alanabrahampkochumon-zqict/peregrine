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

        // Update paddle direction
        if (kbState[SDL_SCANCODE_W])
        {
            _paddleDir = 1;
        }
        if (kbState[SDL_SCANCODE_S])
        {
            _paddleDir = -1;
        }
    }


    void Game::updateGame()
    {
        // Frame Limiting
        // Run the looping until the latest frame time is greater than or equals
        // our previous frame time + the frame time difference, which locks
        // the delta time at the frame limiter delta time(eg: 16ms).
        while (SDL_GetTicks() < _tickCount + FRAME_LIMITER_DELTA) {}

        // Get the delta time(time between frames)
        // Delta time is in ms
        uint64_t currentTick = SDL_GetTicks();
        float deltaTime      = static_cast<float>(currentTick - _tickCount) / 1000.0f;
        _tickCount           = currentTick;

        // Clamp maximum delta time (useful for debug breaks)
        deltaTime = deltaTime > 0.05f ? 0.05f : deltaTime;

        // If the padding direction is non zero move the padding
        if (_paddleDir != 0)
        {
            _paddlePos.y += static_cast<float>(_paddleDir) * PADDLE_SPEED * deltaTime;

            // Lock the paddle between wall boundaries
            // Top position
            if (_paddlePos.y < WALL_THICKNESS)
            {
                _paddlePos.y = WALL_THICKNESS;
            }
            // Bottom position
            else if (_paddlePos.y > WINDOW_HEIGHT - PADDLE_HEIGHT - WALL_THICKNESS)
            {
                _paddlePos.y = WINDOW_HEIGHT - PADDLE_HEIGHT - WALL_THICKNESS;
            }
        }
        // SDL_Log("%0.05f ms", deltaTime);
    }

    void Game::generateOutput() const
    {
        // Set the draw color
        SDL_SetRenderDrawColor(_renderer, 0, 60, 120, 255);

        // Render the clear color
        SDL_RenderClear(_renderer);

        // Wall
        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
        const SDL_FRect wallTop{ .x = 0, .y = 0, .w = WINDOW_WIDTH, .h = WALL_THICKNESS };
        const SDL_FRect wallRight{
            .x = WINDOW_WIDTH - WALL_THICKNESS, .y = 0, .w = WALL_THICKNESS, .h = WINDOW_HEIGHT
        };
        const SDL_FRect wallBottom{
            .x = 0, .y = WINDOW_HEIGHT - WALL_THICKNESS, .w = WINDOW_WIDTH, .h = WALL_THICKNESS
        };
        SDL_RenderFillRect(_renderer, &wallTop);
        SDL_RenderFillRect(_renderer, &wallRight);
        SDL_RenderFillRect(_renderer, &wallBottom);


        // Paddle
        const SDL_FRect paddle{ .x = _paddlePos.x, .y = _paddlePos.y, .w = PADDLE_WIDTH, .h = PADDLE_HEIGHT };
        SDL_RenderFillRect(_renderer, &paddle);

        // Ball
        const SDL_FRect ball{ .x = INITIAL_BALL_X, .y = INITIAL_BALL_Y, .w = BALL_SIZE, .h = BALL_SIZE };
        SDL_RenderFillRect(_renderer, &ball);

        // Swap buffers
        SDL_RenderPresent(_renderer);
    }
} // namespace pong
