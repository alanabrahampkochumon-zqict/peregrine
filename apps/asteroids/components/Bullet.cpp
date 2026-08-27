/**
 * @file Bullet.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Implementation of member functions declared in Bullet.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Bullet.h"

#include "game/AsteroidGame.h"


namespace asteroids::actor
{
    Bullet::Bullet(AsteroidGame* game) noexcept: Actor{game}
    {
        
    }
} // namespace asteroids::actor
