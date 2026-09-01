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

namespace asteroids::comp
{
    class CircleComponent: public Component
    {
    public:
        explicit CircleComponent(actors::Actor* owner) noexcept;

        constexpr void setRadius(const float radius) noexcept { _radius = radius; };
        constexpr void setCenter(const psm::Vec2& center) noexcept { _center = center; };

        [[nodiscard]] constexpr float getRadius() const noexcept { return _radius; };
        [[nodiscard]] psm::Vec2 getCenter() const noexcept { return _center; }

        /**
         * Check whether this component is intersecting with @p other.
         *
         * @param other The component to check the intersection with.
         * @return True if this component intersects with @p other, false otherwise.
         */
        [[nodiscard]] bool intersect(const CircleComponent& other) const noexcept;


    private:
        float _radius{};
        psm::Vec2 _center{};
    };
} // namespace asteroids::comp
