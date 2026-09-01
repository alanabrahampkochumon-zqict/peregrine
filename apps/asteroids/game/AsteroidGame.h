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

// Enable sprite bounding boxes
#define ENABLE_SPRITE_DEBUG
#define ENABLE_COLLISION_DEBUG


#include "actors/Actor.h"
#include "actors/Asteroid.h"
#include "components/InputComponent.h"
#include "components/MoveComponent.h"
#include "components/SpriteSheetComponent.h"

#include <SDL3/SDL.h>
#include <array>
#include <unordered_map>
#include <vector>


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
        void addActor(actors::Actor* actor) noexcept;

        /// @brief Remove an actor from the game
        void removeActor(const actors::Actor* actor) noexcept;

        /// @brief Add a sprite to the game
        void addSprite(comp::SpriteSheetComponent* sprite) noexcept;

        /// @brief Remove a sprite from the game
        void removeSprite(const comp::SpriteSheetComponent* sprite) noexcept;

        // /// @brief Add a move component
        // void addMoveComponent(comp::MoveComponent* comp) noexcept;
        //
        // /// @brief Remove a move component
        // void removeMoveComponent(comp::MoveComponent* comp) noexcept;
        //
        // /// @brief Add a move component
        // void addInputComponent(comp::InputComponent* comp) noexcept;
        //
        // /// @brief Remove a move component
        // void removeInputComponent(comp::InputComponent* comp) noexcept;


        /// @brief Read and return a texture with the given path/filename.
        graphics::Texture<>* getTexture(const std::string& filename) noexcept;

        //+=+=+=+=+=
        // GETTERS
        //+=+=+=+=+=

        [[nodiscard]] constexpr size_t getWindowHeight() const noexcept { return WINDOW_HEIGHT; }
        [[nodiscard]] constexpr size_t getWindowWidth() const noexcept { return WINDOW_WIDTH; }
        [[nodiscard]] const std::vector<actors::Asteroid*>& getAsteroids() const noexcept { return _asteroids; }

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
        void _draw() const;


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
        std::vector<actors::Actor*> _actors{}, _pendingActors{};
        std::vector<actors::Asteroid*> _asteroids{};

        std::vector<comp::SpriteSheetComponent*> _spriteComponents{};
        std::unordered_map<std::string, graphics::Texture<>*> _textureSet{};


        //+=+=+=+=+=+=+=+=+=
        // CONSTANTS
        //+=+=+=+=+=+=+=+=+=
        static constexpr auto GAME_NAME              = "Asteroids";
        static constexpr auto GAME_VERSION           = "1.0";
        static constexpr auto GAME_ID                = "com.peregrine.asteroids";
        static constexpr size_t WINDOW_WIDTH         = 1280;
        static constexpr size_t WINDOW_HEIGHT        = 720;
        static constexpr size_t ASTEROID_SPAWN_COUNT = 20;

        static constexpr std::array<uint8_t, 4> CLEAR_COLOR{ 0, 16, 32, 255 }; // RGBA
    };
} // namespace asteroids
