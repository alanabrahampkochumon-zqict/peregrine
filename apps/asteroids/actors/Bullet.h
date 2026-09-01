#pragma once
/**
 * @file Bullet.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Spaceship bullet.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Actor.h"
#include "components/CircleComponent.h"
#include "components/SpriteSheetComponent.h"

namespace asteroids::actors
{
    class Bullet: public Actor
    {
    public:
        explicit Bullet(AsteroidGame* game) noexcept;

        ~Bullet() noexcept override;

        void updateActor(float deltaTime) override;

    private:
        comp::CircleComponent* _collider{};
        comp::SpriteSheetComponent* _sprite{};

    public:
        static constexpr size_t SPRITE_WIDTH   = 64;
        static constexpr size_t SPRITE_HEIGHT  = 64;
        static constexpr size_t SPRITE_COUNT_X = 4;
        static constexpr size_t SPRITE_COUNT_Y = 1;
        static constexpr float BULLET_SPEED    = 250.0f;
        static constexpr float BULLET_COOLDOWN = 0.5f;
        static constexpr auto SPRITE_PATH      = "assets/Missiles.png";

        // For bullet we need a custom collision radius since the sprite dimensions are very large
        // compared to the actual size of the bullet.
        static constexpr auto COLLISION_BOX_RADIUS = 10.0f; // 20 diameter
    };
} // namespace asteroids::actors
