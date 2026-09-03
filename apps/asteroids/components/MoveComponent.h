#pragma once
/**
 * @file MoveComponent.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 24, 2026
 *
 * @brief Define movement component.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Component.h"

namespace asteroids::comp
{
    class MoveComponent: public Component
    {
    public:
        explicit MoveComponent(actors::Actor* owner, int updateOrder = 10);

        void update(float deltaTime) noexcept override;

        [[nodiscard]] constexpr float getForwardSpeed() const noexcept { return _forwardSpeed; }
        [[nodiscard]] constexpr float getAngularSpeed() const noexcept { return _angularSpeed; }

        constexpr void setForwardSpeed(const float forwardSpeed) noexcept { _forwardSpeed = forwardSpeed; }
        constexpr void setAngularSpeed(const float angularSpeed) noexcept { _angularSpeed = angularSpeed; }

        ~MoveComponent() noexcept override;


    private:
        /// ForwardSpeed -> Translation speed in unit/s
        /// AngularSpeed -> Rotation speed in rad/s
        float _forwardSpeed{}, _angularSpeed{};
    };
} // namespace asteroids::comp
