/**
 * @file CircleComponent.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Implementation for member functions declared in CircleComponent.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "CircleComponent.h"

#include "actors/Actor.h"

#include <SDL3/SDL.h>

namespace asteroids::comp
{

    CircleComponent::CircleComponent(actors::Actor* owner) noexcept: Component{ owner } { _owner->addComponent(this); }


    bool CircleComponent::intersect(const CircleComponent& other) const noexcept
    {
        // Compute the squared distance between this circle center and
        // the other circle's center.
        const auto distSq = getCenter().distSq(other.getCenter());

        // Compute the squared sum of radii
        auto radSq = getRadius() + other.getRadius();
        radSq *= radSq;

        // If the sum of radius is less than or equal to the distance between centers
        // then circles are intersecting
        return distSq <= radSq;
    }
} // namespace asteroids::comp
