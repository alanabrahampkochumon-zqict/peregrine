/**
 * @file MoveComponent.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 24, 2026
 *
 * @brief Implmentation of move component defined in MoveComponent.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "MoveComponent.h"

#include "Actor.h"
#include "game/AsteroidGame.h"

#include <psm/PSMath.h>


namespace asteroids::comp
{
    MoveComponent::MoveComponent(Actor* owner, const int updateOrder): Component{ owner, updateOrder }
    { owner->getGame()->addMoveComponent(this); }

    void MoveComponent::update(const float deltaTime) noexcept
    {
        // TODO: Add screen warping when the point moves outside screen bounds.
        // Update the actor's position if the speed is non-zero
        if (!psm::nearZero(_forwardSpeed))
        {
            psm::Vec2 position = _owner->getPosition();
            const auto forward = _owner->getForward().xy();
            position += forward * _forwardSpeed * deltaTime;
            _owner->setPosition(position);
        }
        // Update the actor's orientation if the angular speed is non-zero
        if (!psm::nearZero(_angularSpeed))
        {
            auto rotation = _owner->getRotation();
            rotation += _angularSpeed * deltaTime;
            _owner->setRotation(rotation);
        }
    }

    MoveComponent::~MoveComponent() noexcept { _owner->getGame()->removeMoveComponent(this); }

} // namespace asteroids::comp
