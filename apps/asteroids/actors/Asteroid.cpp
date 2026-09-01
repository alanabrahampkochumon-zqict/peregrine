//
// Created by Admin on 8/25/2026.
//

#include "Asteroid.h"

#include "components/MoveComponent.h"
#include "components/SpriteSheetComponent.h"
#include "game/AsteroidGame.h"

namespace asteroids::actors
{
    Asteroid::Asteroid(AsteroidGame* game) noexcept: Actor{ game }
    {
        const auto position = psm::Vec2::getRandom(
            psm::ZERO,
            psm::Vec2{ static_cast<float>(game->getWindowWidth()), static_cast<float>(game->getWindowHeight()) });
        setPosition(position);
        setRotation(psm::genRand(0.0f, std::numbers::pi_v<float>));

        auto* sprite = new comp::SpriteSheetComponent(this);
        sprite->setSpritesheetTexture(game->getTexture(SPRITE_PATH), SPRITE_WIDTH, SPRITE_HEIGHT);
        // Select an active sprite index at random
        sprite->setActiveIndex(
            { .x = psm::genRand<size_t>(0, SPRITE_COUNT_X - 1), .y = psm::genRand<size_t>(0, SPRITE_COUNT_Y - 1) });

        auto* moveComp = new comp::MoveComponent(this);
        moveComp->setForwardSpeed(psm::genRand(MIN_FORWARD_SPEED, MAX_FORWARD_SPEED));

        setState(State::ACTIVE);
    }

} // namespace asteroids::actors
