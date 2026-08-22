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

namespace asteroids::comp
{
    void Actor::update(float deltaTime)
    {
        // TODO: Impl
    }

    void Actor::updateComponents(float deltaTime)
    {
        // TODO: Impl
    }


    void Actor::addComponent(Component* comp) { _components.insert(comp); }

    void Actor::removeComponent(Component* comp) { _components.erase(comp); }
} // namespace asteroids::comp
