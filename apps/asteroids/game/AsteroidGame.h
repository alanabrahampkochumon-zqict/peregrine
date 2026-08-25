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

#include "components/Actor.h"
#include "components/SpriteComponent.h"

#include <SDL3/SDL.h>
#include <array>
#include <vector>
#include <unordered_map>


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

        /// @brief Add an actor to the game
        void addActor(comp::Actor* actor) noexcept;

        /// @brief Remove an actor from the game
        void removeActor(const comp::Actor* actor) noexcept;

        /// @brief Add a sprite to the game
        void addSprite(comp::SpriteComponent* sprite) noexcept;

        /// @brief Remove a sprite from the game
        void removeSprite(const comp::SpriteComponent* sprite) noexcept;

        /// @brief Read and return a texture with the given path/filename.
        graphics::Texture<>* getTexture(const std::string& filename) noexcept;

        //+=+=+=+=+=
        // GETTERS
        //+=+=+=+=+=

        [[nodiscard]] constexpr size_t getWindowHeight() const noexcept { return WINDOW_HEIGHT; }
        [[nodiscard]] constexpr size_t getWindowWidth() const noexcept { return WINDOW_WIDTH; }

    private:
        //+=+=+=+=+=+=+=+=+=
        // INTERNAL HELPERS
        //+=+=+=+=+=+=+=+=+=

        /// @brief Loads the game data
        void _loadData();

        /// @brief Handles game input.
        void _handleInput();

        /// @brief Update game object(s) states.
        void _update();

        /// @brief Draw the current game state to screen.
        void _draw();


        //+=+=+=+=+=+=+=+=+=
        // MEMBER VARIABLES
        //+=+=+=+=+=+=+=+=+=
        SDL_Window* _window{ nullptr };
        SDL_Renderer* _renderer{ nullptr };

        bool _isRunning{ false };
        uint64_t _lastFrameTick{ 0 };
        bool _isUpdatingActors{ false };

        // Actors are the collection of all active actors.
        // Pending actors is used to add actors to the game while
        // it is in the middle of an update loop.
        std::vector<comp::Actor*> _actors{}, _pendingActors{};

        std::vector<comp::SpriteComponent*> _spriteComponents{};
        std::unordered_map<std::string, graphics::Texture<>*> _textureSet{};


        //+=+=+=+=+=+=+=+=+=
        // CONSTANTS
        //+=+=+=+=+=+=+=+=+=
        static constexpr auto GAME_NAME       = "Asteroids";
        static constexpr auto GAME_VERSION    = "1.0";
        static constexpr auto GAME_ID         = "com.peregrine.asteroids";
        static constexpr size_t WINDOW_WIDTH  = 1280;
        static constexpr size_t WINDOW_HEIGHT = 720;

        static constexpr std::array<uint8_t, 4> CLEAR_COLOR{ 255, 255, 255, 255 }; // RGBA
    };
} // namespace asteroids
