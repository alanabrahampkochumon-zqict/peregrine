#pragma once
/**
 * @file InputComponent.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Handles game input, producing the correct effect.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "MoveComponent.h"


namespace asteroids::comp
{
    /// Forward declaration
    class Actor;

    class InputComponent: MoveComponent
    {
    public:
        explicit InputComponent(Actor* owner) noexcept;

        void processInput(const bool* keyState) noexcept override;


        //+=+=+=+=+=+=+=+=+=
        // GETTERS/SETTERS
        //+=+=+=+=+=+=+=+=+=

        [[nodiscard]] float getMaxForwardSpeed() const noexcept { return _maxForwardSpeed; }
        [[nodiscard]] float getMaxAngularSpeed() const noexcept { return _maxAngularSpeed; }
        [[nodiscard]] int getForwardKey() const noexcept { return _forwardKey; }
        [[nodiscard]] int getBackwardKey() const noexcept { return _backwardKey; }
        [[nodiscard]] int getClockwiseKey() const noexcept { return _clockwiseKey; }
        [[nodiscard]] int getAnticlockwiseKey() const noexcept { return _anticlockwiseKey; }


        void setMaxForwardSpeed(const float forwardSpeed) noexcept { _maxForwardSpeed = forwardSpeed; }
        void setMaxAngularSpeed(const float angularSpeed) noexcept { _maxAngularSpeed = angularSpeed; }
        void setForwardKey(const int key) noexcept { _forwardKey = key; }
        void setBackwardKey(const int key) noexcept { _backwardKey = key; }
        void setClockwiseKey(const int key) noexcept { _clockwiseKey = key; }
        void setAnticlockwiseKey(const int key) noexcept { _anticlockwiseKey = key; }

    private:
        // Maximum speeds
        float _maxForwardSpeed, _maxAngularSpeed;
        // Key bindings
        int _forwardKey{}, _backwardKey{};
        int _clockwiseKey{}, _anticlockwiseKey{};
    };
} // namespace asteroids::comp
