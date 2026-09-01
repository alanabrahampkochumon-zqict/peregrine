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

#include "components/InputComponent.h"
#include "game/AsteroidGame.h"

#include <psm/PSMath.h>


namespace asteroids::actors
{

    Ship::Ship(AsteroidGame* game) noexcept: Actor{ game }
    {
        const auto position = psm::Vec2{ static_cast<float>(game->getWindowWidth()) / 2.0f,
                                         static_cast<float>(game->getWindowHeight()) / 2.0f };
        setPosition(position);
        setRotation(0.0f);
        setScale(SHIP_SCALE);

        // Create a sprite component
        const auto spriteComp = new comp::SpriteSheetComponent(this);
        const auto texture    = game->getTexture(SPRITE_PATH);
        spriteComp->setSpritesheetTexture(texture, SPRITE_WIDTH, SPRITE_HEIGHT);
        // NOTE: Update sprite sheet index
        spriteComp->setActiveIndex({ .x = 4, .y = 0 });

        const auto inputComp = new comp::InputComponent(this);
        inputComp->setForwardKey(FORWARD_KEY);
        inputComp->setBackwardKey(BACKWARD_KEY);
        inputComp->setClockwiseKey(CLOCKWISE_KEY);
        inputComp->setAnticlockwiseKey(ANTICLOCKWISE_KEY);

        inputComp->setMaxForwardSpeed(SHIP_FORWARD_SPEED);
        inputComp->setMaxAngularSpeed(SHIP_ROTATION_SPEED);

        setState(State::ACTIVE);
    }
} // namespace asteroids::actors
