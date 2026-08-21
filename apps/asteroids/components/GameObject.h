#pragma once
/**
 * @file GameObject.h
 * @author Alan Abraham
 * @date Created on: August 21, 2026
 *
 * @brief Object interface for creating a aggregating an entity with its components.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Component.h"

#include <unordered_set>

namespace asteroids::comp
{
    class GameObject
    {
    public:
        /// Add a component to the game object.
        void addComponent(Component* comp) const;

        /// Remove a component form the game object.
        void removeComponent(Component* comp) const;

    private:
        std::unordered_set<Component*> _components{};
    };
} // namespace asteroids::comp
