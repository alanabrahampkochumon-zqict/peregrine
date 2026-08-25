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
} // namespace asteroids::actor
