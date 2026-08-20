#pragma once
/**
 * @file Pong.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 18, 2026
 *
 * @brief Pong function definitions.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Vec2.h"

#include <SDL3/SDL.h>
#include <string>

namespace pong
{
    class Game
    {
    public:
        Game() = default;

        /// Initialize a game object. This method must be called explicitly after object creation
        bool initialize() noexcept;

        /// Game loop
        void run() noexcept;

        /// Stop the game system.
        void shutdown() const noexcept;

    private:
        /// Process user input
        void _processInput();

        /// Update game state
        void _updateGame();

        /// Create game output, displaying it on screen or sending signals to other output devices
        void _generateOutput() const;



        //-+-+-+-+-+-+-+-+-+-+-+-+
        // MEMBER VARIABLES
        //-+-+-+-+-+-+-+-+-+-+-+-+

        SDL_Window* _window{ nullptr };
        SDL_Renderer* _renderer{ nullptr };
        bool _isRunning{ false };
        uint64_t _tickCount{ 0 };

        int _paddleDir{ 0 };
        math::Vec2 _paddlePos{ INITIAL_PADDLE_X, INITIAL_PADDLE_Y };

        math::Vec2 _ballVel{ -200.0f, 235.0f };
        math::Vec2 _ballPos{ INITIAL_BALL_X, INITIAL_BALL_Y };


        //-+-+-+-+-+-+-+-+-+-+-+-+
        // CONSTANTS
        //-+-+-+-+-+-+-+-+-+-+-+-+
        static constexpr auto GAME_NAME = "Pong";
        static constexpr size_t WINDOW_WIDTH   = 1280;
        static constexpr size_t WINDOW_HEIGHT  = 720;

        static constexpr size_t WALL_THICKNESS = 16;

        static constexpr size_t PADDLE_WIDTH     = 16;
        static constexpr size_t PADDLE_HEIGHT    = 120;
        static constexpr size_t INITIAL_PADDLE_X = 16;
        static constexpr size_t INITIAL_PADDLE_Y = static_cast<size_t>(WINDOW_HEIGHT * 0.5f - PADDLE_HEIGHT * 0.5f);
        static constexpr float PADDLE_SPEED      = 300.0f;

        static constexpr size_t BALL_SIZE     = 20;
        static constexpr float INITIAL_BALL_X = WINDOW_WIDTH * 0.5f;
        static constexpr float INITIAL_BALL_Y = WINDOW_HEIGHT * 0.5f;

        static constexpr size_t FRAME_LIMITER_DELTA = 16;
    };
} // namespace pong
