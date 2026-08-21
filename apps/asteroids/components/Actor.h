#pragma once
/**
 * @file Actor.h
 * @author Alan Abraham
 * @date Created on: August 21, 2026
 *
 * @brief Actor interface for component system.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "../../math/vec/Vec2.h"
#include "Component.h"

#include <cstdint>

namespace asteroids::comp
{
    class Actor
    {
    public:
        /// Actor state
        enum class State : uint8_t
        {
            Active,
            Paused,
            Dead
        };

        explicit Actor(class Game* game);

        virtual ~Actor() = 0;


        /// Update called from game.
        void update(float deltaTime);

        /// Update all components of the actor.
        void updateComponents(float deltaTime);

        /// Actor specific update code (can be overridden).
        virtual void updateActor(float deltaTime);


        /// Adds a component to the actor.
        void addComponent(Component* comp);

        /// Removes a component from the actor.
        void removeComponent(Component* comp);

    public:
        State _state;
        // psm::Vec2
    };
} // namespace asteroids::comp
