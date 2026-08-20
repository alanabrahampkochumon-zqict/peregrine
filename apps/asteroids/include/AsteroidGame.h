#pragma once
/**
 * @file AsteroidGame.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 20, 2026
 *
 * @brief Defines asteroid game interface.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

namespace asteroids
{

    class AsteroidGame
    {
    public:
        constexpr AsteroidGame() = default;

        /// @brief Initialize a game
        bool initialize() noexcept;

        /// @brief Run the main game loop
        void run() noexcept;

        /// @brief Perform game deallocations and cleanup
        void shutdown() const noexcept;


    private:
        //+=+=+=+=+=+=+=+=+=
        // INTERNAL HELPERS
        //+=+=+=+=+=+=+=+=+=

        /// @brief Handles game input.
        void _handleInput();

        /// @brief Update game object(s) states.
        void _update();

        /// @brief Draw the current game state to screen.
        void _draw();


        //+=+=+=+=+=+=+=+=+=
        // MEMBER VARIABLES
        //+=+=+=+=+=+=+=+=+=
        bool _isRunning{ false };
    };
} // namespace asteroids
