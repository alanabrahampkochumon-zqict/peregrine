#pragma once
/**
 * @file Pong.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 18, 2026
 *
 * @brief Pong function defintions.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <SDL3/SDL.h>

namespace pong
{
    class Game
    {
    public:
        Game() = default;

        /// Initialize a game object. This method must be called explicitly after object creation
        bool initialize();

        /// Game loop
        void run();

        /// Stop the game system.
        void shutdown();

    private:
        /// Process user input
        void processInput();

        /// Update game state
        void updateGame();

        /// Create game output, displaying it on screen or sending signals to other output devices
        void generateOutput();



        //-+-+-+-+-+-+-+-+-+-+-+-+
        // MEMBER VARIABLES
        //-+-+-+-+-+-+-+-+-+-+-+-+

        SDL_Window* _window{ nullptr };
        bool _isRunning{ false };
    };
} // namespace pong
