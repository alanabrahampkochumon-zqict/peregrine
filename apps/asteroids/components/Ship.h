#pragma once
/**
 * @file Ship.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Ship implementation.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Actor.h"

#include <SDL3/SDL.h>

namespace asteroids
{
    class AsteroidGame;
    namespace actor
    {
        class Ship: public comp::Actor
        {
        public:
            explicit Ship(AsteroidGame* game) noexcept;

            static constexpr size_t SPRITE_WIDTH        = 64;
            static constexpr size_t SPRITE_HEIGHT       = 64;
            static constexpr size_t SPRITE_COUNT_X      = 5;
            static constexpr size_t SPRITE_COUNT_Y      = 3;
            static constexpr float SHIP_SCALE          = 1.2f;
            static constexpr float SHIP_FORWARD_SPEED   = 100.0f;
            static constexpr float SHIP_ROTATION_SPEED  = 1.0f;
            static constexpr uint32_t FORWARD_KEY       = SDL_SCANCODE_W;
            static constexpr uint32_t BACKWARD_KEY      = SDL_SCANCODE_S;
            static constexpr uint32_t CLOCKWISE_KEY     = SDL_SCANCODE_D;
            static constexpr uint32_t ANTICLOCKWISE_KEY = SDL_SCANCODE_A;
            static constexpr auto SPRITE_PATH           = "assets/Spaceships.png";
        };

    } // namespace actor
} // namespace asteroids
