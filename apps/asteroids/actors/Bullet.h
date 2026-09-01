#pragma once
/**
 * @file Bullet.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Spaceship bullet.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Actor.h"

namespace asteroids::actors
{
    class Bullet: public Actor
    {
        explicit Bullet(AsteroidGame* game) noexcept;


        static constexpr size_t SPRITE_WIDTH   = 64;
        static constexpr size_t SPRITE_HEIGHT  = 64;
        static constexpr size_t SPRITE_COUNT_X = 4;
        static constexpr size_t SPRITE_COUNT_Y = 1;
        static constexpr float FORWARD_SPEED   = 4.0f;
        static constexpr auto SPRITE_PATH      = "assets/Missiles.png";
    };
} // namespace asteroids::actors
