#pragma once
/**
 * @file Asteroid.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 25, 2026
 *
 * @brief Asteroid game object/actor.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "../game/AsteroidGame.h"
#include "Actor.h"

#include <numbers>
#include <psm/PSMath.h>

namespace asteroids::actor
{
    class Asteroid: public comp::Actor
    {
    public:
        explicit Asteroid(AsteroidGame* game) noexcept;
    };

    static constexpr size_t SPRITE_WIDTH     = 64;
    static constexpr size_t SPRITE_HEIGHT    = 64;
    static constexpr size_t SPRITE_COUNT_X   = 4;
    static constexpr size_t SPRITE_COUNT_Y   = 1;
    static constexpr float MIN_FORWARD_SPEED = 4.0f;
    static constexpr float MAX_FORWARD_SPEED = 30.0f;
    static constexpr auto SPRITE_PATH        = "assets/Asteroids_Foreground.png";
} // namespace asteroids::actor
