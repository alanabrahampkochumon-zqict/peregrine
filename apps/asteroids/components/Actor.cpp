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
        : _state{ State::ACTIVE }, _position{}, _scale{ 1 }, _rotation{ 0 }, _game{ game }
    { game->addActor(this); }

    Actor::~Actor() { _game->removeActor(this); }


    void Actor::update(const float deltaTime)
    {
        updateComponents(deltaTime);
        updateActor(deltaTime);
    }

    void Actor::updateComponents(const float deltaTime) const
    {
        for (const auto comp : _components)
        {
            comp->update(deltaTime);
        }
    }

    void Actor::updateActor([[maybe_unused]] float deltaTime) {}

    void Actor::addComponent(Component* comp) noexcept { _components.insert(comp); }

    void Actor::removeComponent(Component* comp) noexcept { _components.erase(comp); }


    void Actor::processInput(const bool* keyState) noexcept
    {
        if (_state == State::ACTIVE)
        {
            for (const auto& comp : _components)
            {
                comp->processInput(keyState);
            }
            actorInput(keyState);
        }
    }
} // namespace asteroids::comp
