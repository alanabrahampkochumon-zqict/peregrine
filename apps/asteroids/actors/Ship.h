#pragma once
/**
 * @file Ship.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 26, 2026
 *
 * @brief Ship implementation.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "actors/Actor.h"
#include "components/CircleComponent.h"
#include "components/SpriteSheetComponent.h"

#include <SDL3/SDL.h>

namespace asteroids::actors
{
    class Ship: public Actor
    {
    public:
        explicit Ship(AsteroidGame* game) noexcept;
        ~Ship() noexcept override;

        [[nodiscard]] comp::CircleComponent* getCollider() const noexcept { return _collider; }

        void updateActor(float deltaTime) override;

        void actorInput(const bool* keyState) noexcept override;

    private:
        comp::CircleComponent* _collider{};
        comp::SpriteSheetComponent* _sprite{};
        float _bulletCooldown;

        static constexpr size_t SPRITE_WIDTH        = 64;
        static constexpr size_t SPRITE_HEIGHT       = 64;
        static constexpr size_t SPRITE_COUNT_X      = 5;
        static constexpr size_t SPRITE_COUNT_Y      = 3;
        static constexpr float SHIP_SCALE           = 1.2f;
        static constexpr float SHIP_FORWARD_SPEED   = 100.0f;
        static constexpr float SHIP_ROTATION_SPEED  = 1.0f;
        static constexpr uint32_t FORWARD_KEY       = SDL_SCANCODE_W;
        static constexpr uint32_t BACKWARD_KEY      = SDL_SCANCODE_S;
        static constexpr uint32_t CLOCKWISE_KEY     = SDL_SCANCODE_D;
        static constexpr uint32_t ANTICLOCKWISE_KEY = SDL_SCANCODE_A;
        static constexpr auto SPRITE_PATH           = "assets/Spaceships.png";

        // For the ship we can set the collision component to be the size of the bounding box length / 2
        // which equals the sprite height/width, since both are the same, and sprite is a square
        // the diameter of circle that fits in the length of side
        static constexpr size_t COLLISION_BOX_RADIUS = SPRITE_HEIGHT / 2;
        static constexpr float BULLET_COOLDOWN       = 0.5f;
    };

} // namespace asteroids::actors
