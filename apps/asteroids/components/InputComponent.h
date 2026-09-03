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
    class InputComponent: public MoveComponent
    {
    public:
        explicit InputComponent(actors::Actor* owner) noexcept;

        void processInput(const bool* keyState) noexcept override;

        ~InputComponent() noexcept override;


        //+=+=+=+=+=+=+=+=+=
        // GETTERS/SETTERS
        //+=+=+=+=+=+=+=+=+=

        [[nodiscard]] float getMaxForwardSpeed() const noexcept { return _maxForwardSpeed; }
        [[nodiscard]] float getMaxAngularSpeed() const noexcept { return _maxAngularSpeed; }
        [[nodiscard]] uint32_t getForwardKey() const noexcept { return _forwardKey; }
        [[nodiscard]] uint32_t getBackwardKey() const noexcept { return _backwardKey; }
        [[nodiscard]] uint32_t getClockwiseKey() const noexcept { return _clockwiseKey; }
        [[nodiscard]] uint32_t getAnticlockwiseKey() const noexcept { return _anticlockwiseKey; }


        void setMaxForwardSpeed(const float forwardSpeed) noexcept { _maxForwardSpeed = forwardSpeed; }
        void setMaxAngularSpeed(const float angularSpeed) noexcept { _maxAngularSpeed = angularSpeed; }
        void setForwardKey(const uint32_t key) noexcept { _forwardKey = key; }
        void setBackwardKey(const uint32_t key) noexcept { _backwardKey = key; }
        void setClockwiseKey(const uint32_t key) noexcept { _clockwiseKey = key; }
        void setAnticlockwiseKey(const uint32_t key) noexcept { _anticlockwiseKey = key; }

    private:
        // Maximum speeds
        float _maxForwardSpeed{}, _maxAngularSpeed{};
        // Key bindings
        uint32_t _forwardKey{}, _backwardKey{};
        uint32_t _clockwiseKey{}, _anticlockwiseKey{};
    };
} // namespace asteroids::comp
