#pragma once
/**
 * @file Asteroid.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 25, 2026
 *
 * @brief Asteroid game object/actor.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Actor.h"
#include "components/CircleComponent.h"
#include "components/SpriteSheetComponent.h"

#include <numbers>
#include <psm/PSMath.h>

namespace asteroids::actors
{
    class Asteroid: public Actor
    {
    public:
        explicit Asteroid(AsteroidGame* game) noexcept;

        void updateActor(float deltaTime) override;

        [[nodiscard]] comp::CircleComponent* getCollider() const noexcept { return _collider; }


    private:
        comp::CircleComponent* _collider{};

    public:
        static constexpr size_t SPRITE_WIDTH     = 64;
        static constexpr size_t SPRITE_HEIGHT    = 64;
        static constexpr size_t SPRITE_COUNT_X   = 4;
        static constexpr size_t SPRITE_COUNT_Y   = 1;
        static constexpr float MIN_FORWARD_SPEED = 4.0f;
        static constexpr float MAX_FORWARD_SPEED = 30.0f;
        static constexpr auto SPRITE_PATH        = "assets/Asteroids_Foreground.png";
        // For asteroids we can set the collision component to be the size of the bounding box length / 2
        // which equals the sprite height/width, since both are the same, and sprite is a square
        // the diameter of circle that fits in the length of side
        static constexpr size_t COLLISION_BOX_RADIUS = SPRITE_HEIGHT / 2;
    };

} // namespace asteroids::actors
