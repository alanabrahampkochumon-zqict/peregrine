/**
 * @file Actor.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 22, 2026
 *
 * @brief Implementation of member functions declared in Actor.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Actor.h"

#include "game/AsteroidGame.h"

namespace asteroids::comp
{
    /// Actor addition and deletion is managed in the actor component itself.
    Actor::Actor(AsteroidGame* game) noexcept
        : _state{ State::Paused }, _position{}, _scale{ 1 }, _rotation{ 0 }, _game{ game }
    { game->addActor(this); }

    Actor::~Actor() { _game->removeActor(this); }


    void Actor::update([[maybe_unused]] float deltaTime)
    {
        // TODO: Impl remove maybe unused
    }

    void Actor::updateComponents([[maybe_unused]] float deltaTime)
    {
        // TODO: Impl
    }
    void Actor::updateActor([[maybe_unused]] float deltaTime)
    {
        // TODO: Impl
    }


    void Actor::addComponent(Component* comp) noexcept { _components.insert(comp); }

    void Actor::removeComponent(Component* comp) noexcept { _components.erase(comp); }
} // namespace asteroids::comp
