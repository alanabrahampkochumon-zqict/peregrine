#pragma once
#include "../../../out/build/Clang-Debug/_deps/sdl3-src/include/SDL3/SDL_video.h"
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
        SDL_Window* _window{ nullptr };
        uint64_t lastFrameTick{ 0 };



        //+=+=+=+=+=+=+=+=+=
        // CONSTANTS
        //+=+=+=+=+=+=+=+=+=
        static constexpr auto GAME_NAME       = "Asteroids";
        static constexpr auto GAME_VERSION    = "1.0";
        static constexpr auto GAME_ID         = "com.peregrine.asteroids";
        static constexpr size_t WINDOW_WIDTH  = 1280;
        static constexpr size_t WINDOW_HEIGHT = 720;
    };
} // namespace asteroids
