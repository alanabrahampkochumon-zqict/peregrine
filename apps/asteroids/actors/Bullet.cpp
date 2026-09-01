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

#include "components/MoveComponent.h"
#include "components/SpriteSheetComponent.h"
#include "game/AsteroidGame.h"

#include <format>
#include <iostream>

namespace asteroids::actors
{
    Bullet::Bullet(AsteroidGame* game) noexcept: Actor{ game }
    {
        // Create the sprite component
        _sprite = new comp::SpriteSheetComponent(this);
        _sprite->setSpritesheetTexture(game->getTexture(SPRITE_PATH), SPRITE_WIDTH, SPRITE_HEIGHT);
        _sprite->setActiveIndex({ .x = SPRITE_COUNT_X - 1, .y = SPRITE_COUNT_Y - 1 });

        // Add a move component
        const auto moveComp = new comp::MoveComponent(this);
        moveComp->setForwardSpeed(BULLET_SPEED);

        // Collision component
        _collider = new comp::CircleComponent(this);
        _collider->setCenter(getPosition());
        _collider->setRadius(COLLISION_BOX_RADIUS);
    }

    Bullet::~Bullet() noexcept
    {
        // Since sprite is a separate component we need to remove it from the game
        // when this entity is removed
        getGame()->removeSprite(_sprite);
    }

    void Bullet::updateActor([[maybe_unused]] float deltaTime)
    {
        // Update the collider center(position)
        _collider->setCenter(getPosition());

        // Test for intersection with asteroids
        // Update the state to be dead and this will be cleaned up
        // by the game class on next update
        for (const auto asteroid : getGame()->getAsteroids())
        {
            if (_collider->intersect(*asteroid->getCollider()))
            {
                setState(State::DEAD);
                asteroid->setState(State::DEAD);
                break;
            }
        }
    }

} // namespace asteroids::actors
