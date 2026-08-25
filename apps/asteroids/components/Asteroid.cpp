//
// Created by Admin on 8/25/2026.
//

#include "Asteroid.h"

#include "MoveComponent.h"
#include "SpriteComponent.h"

namespace asteroids::actor
{
    Asteroid::Asteroid(AsteroidGame* game) noexcept: Actor{ game }
    {
        const auto position = psm::Vec2::getRandom(
            psm::ZERO,
            psm::Vec2{ static_cast<float>(game->getWindowWidth()), static_cast<float>(game->getWindowHeight()) });
        setPosition(position);
        setRotation(psm::genRand(0.0f, std::numbers::pi_v<float>));

        auto* sprite = new comp::SpriteComponent(this);
        sprite->setTexture(game->getTexture("assets/Asteroids_Foreground.png"));

        auto* moveComp = new comp::MoveComponent(this);
        moveComp->setForwardSpeed(150.0f);
    }

} // namespace asteroids::actor
