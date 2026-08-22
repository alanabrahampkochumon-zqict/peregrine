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


#include "Component.h"
#include "psm/PSMath.h"

#include <cstdint>
#include <unordered_set>


/// Forward declaration
namespace asteroids
{
    class AsteroidGame;
}

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

        explicit Actor(AsteroidGame* game) noexcept;

        virtual ~Actor();


        /// Update called from game.
        void update(float deltaTime);

        /// Update all components of the actor.
        void updateComponents(float deltaTime);

        /// Actor specific update code (can be overridden).
        virtual void updateActor(float deltaTime);


        /// Adds a component to the actor.
        void addComponent(Component* comp) noexcept;

        /// Removes a component from the actor.
        void removeComponent(Component* comp) noexcept;


        //+=+=+=+=+=
        // GETTERS
        //+=+=+=+=+=

        [[nodiscard]] psm::Vec2 getPosition() const noexcept { return _position; }
        [[nodiscard]] float getScale() const noexcept { return _scale; }
        [[nodiscard]] float getRotation() const noexcept { return _rotation; }
        [[nodiscard]] State getState() const noexcept { return _state; }
        [[nodiscard]] AsteroidGame* getGame() const noexcept { return _game; }



        //+=+=+=+=+=
        // SETTERS
        //+=+=+=+=+=

        /// @internal Vec2 is 8-bytes, we can pass it by value.
        void setPosition(const psm::Vec2 position) { _position = position; }
        void setScale(const float scale) { _scale = scale; }
        void setRotation(const float rotation) { _rotation = rotation; }



    private:
        State _state;
        psm::Vec2 _position;
        float _scale, _rotation;

        std::unordered_set<Component*> _components;
        AsteroidGame* _game;
    };
} // namespace asteroids::comp
