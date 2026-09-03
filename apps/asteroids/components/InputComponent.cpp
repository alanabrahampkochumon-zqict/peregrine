/**
 * @file InputComponent.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Implementation of member functions declared in InputComponent.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "InputComponent.h"

#include "actors/Actor.h"
#include "game/AsteroidGame.h"


namespace asteroids::comp
{

    InputComponent::InputComponent(actors::Actor* owner) noexcept: MoveComponent{ owner }
    { _owner->addComponent(this); }


    void InputComponent::processInput(const bool* keyState) noexcept
    {
        // Update the forward and backward speed of move component
        float forwardSpeed = 0.0f;
        if (keyState[_forwardKey])
        {
            forwardSpeed += _maxForwardSpeed;
        }
        if (keyState[_backwardKey])
        {
            forwardSpeed -= _maxForwardSpeed;
        }
        setForwardSpeed(forwardSpeed);

        // Update the angular speed
        float angularSpeed = 0.0f;
        if (keyState[_clockwiseKey])
        {
            angularSpeed -= _maxAngularSpeed;
        }
        if (keyState[_anticlockwiseKey])
        {
            angularSpeed += _maxAngularSpeed;
        }
        setAngularSpeed(angularSpeed);
    }

    InputComponent::~InputComponent() noexcept { _owner->removeComponent(this); }

} // namespace asteroids::comp
