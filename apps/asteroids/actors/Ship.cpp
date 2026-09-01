/**
 * @file Ship.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Implmentation of member functions declared in Ship.cpp
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Ship.h"

#include "Bullet.h"
#include "components/InputComponent.h"
#include "game/AsteroidGame.h"

#include <psm/PSMath.h>


namespace asteroids::actors
{

    Ship::Ship(AsteroidGame* game) noexcept: Actor{ game }, _bulletCooldown{ BULLET_COOLDOWN }
    {
        const auto position = psm::Vec2{ static_cast<float>(game->getWindowWidth()) / 2.0f,
                                         static_cast<float>(game->getWindowHeight()) / 2.0f };
        setPosition(position);
        setRotation(0.0f);
        setScale(SHIP_SCALE);

        // Create a sprite component
        const auto sprite  = new comp::SpriteSheetComponent(this);
        const auto texture = game->getTexture(SPRITE_PATH);
        sprite->setSpritesheetTexture(texture, SPRITE_WIDTH, SPRITE_HEIGHT);
        // NOTE: Update sprite sheet index
        sprite->setActiveIndex({ .x = 4, .y = 0 });

        const auto inputComp = new comp::InputComponent(this);
        inputComp->setForwardKey(FORWARD_KEY);
        inputComp->setBackwardKey(BACKWARD_KEY);
        inputComp->setClockwiseKey(CLOCKWISE_KEY);
        inputComp->setAnticlockwiseKey(ANTICLOCKWISE_KEY);

        inputComp->setMaxForwardSpeed(SHIP_FORWARD_SPEED);
        inputComp->setMaxAngularSpeed(SHIP_ROTATION_SPEED);


        _collider = new comp::CircleComponent(this);
        _collider->setCenter(getPosition());
        _collider->setRadius(COLLISION_BOX_RADIUS);
    }


    void Ship::updateActor(const float deltaTime)
    {
        // Update reduce the cooldown by the delta time
        _bulletCooldown -= deltaTime;
    }

    void Ship::actorInput(const bool* keyState) noexcept
    {
        // Create a bullet if space is pressed
        if (keyState[SDL_SCANCODE_SPACE] && _bulletCooldown <= 0.0f)
        {
            const auto bullet = new Bullet(getGame());
            bullet->setPosition(getPosition());
            bullet->setRotation(getRotation());

            // Reset the cooldown
            _bulletCooldown = BULLET_COOLDOWN;
        }
    }

} // namespace asteroids::actors
