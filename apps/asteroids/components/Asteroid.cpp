//
// Created by Admin on 8/25/2026.
//

#include "Asteroid.h"

#include "MoveComponent.h"
#include "SpriteSheetComponent.h"

namespace asteroids::actor
{
    Asteroid::Asteroid(AsteroidGame* game) noexcept: Actor{ game }
    {
        const auto position = psm::Vec2::getRandom(
            psm::ZERO,
            psm::Vec2{ static_cast<float>(game->getWindowWidth()), static_cast<float>(game->getWindowHeight()) });
        setPosition(position);
        setRotation(psm::genRand(0.0f, std::numbers::pi_v<float>));

        auto* sprite = new comp::SpriteSheetComponent(this);
        sprite->setSpritesheetTexture(game->getTexture("assets/Asteroids_Foreground.png"), SPRITE_WIDTH, SPRITE_HEIGHT);
        // Select an active sprite index at random
        sprite->setActiveIndex(
            { .x = psm::genRand(0ull, SPRITE_COUNT_X - 1), .y = psm::genRand(0ull, SPRITE_COUNT_Y - 1) });

        auto* moveComp = new comp::MoveComponent(this);
        moveComp->setForwardSpeed(150.0f);

        setState(State::ACTIVE);
    }

} // namespace asteroids::actor
