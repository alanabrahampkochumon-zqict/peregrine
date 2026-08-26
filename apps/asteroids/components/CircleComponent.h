#pragma once
/**
 * @file CircleComponent.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Circle component for collision detection.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Component.h"

#include <psm/PSMath.h>

namespace asteroids
{
    /// Forward declaration
    class Actor;

    namespace comp
    {
        class CircleComponent: public Component
        {
        public:
            explicit CircleComponent(Actor* owner) noexcept;

            void setRadius(float radius) noexcept;

            [[nodiscard]] float getRadius() noexcept;
            [[nodiscard]] psm::Vec2 getCenter() noexcept;


        private:
            float _radius{};
        };
    } // namespace comp
} // namespace asteroids
