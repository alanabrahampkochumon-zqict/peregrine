#pragma once
/**
 * @file SpriteComponent.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 22, 2026
 *
 * @brief 2D game sprite.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "../graphics/Texture.h"
#include "Component.h"

#include <SDL3/SDL.h>

namespace asteroids::comp
{
    /// Forward declaration
    class Actor;

    class SpriteComponent: Component
    {
    public:
        explicit SpriteComponent(Actor* owner, int drawOrder = 100) noexcept;

        ~SpriteComponent() override;

        virtual void draw(SDL_Renderer* renderer) noexcept;

        virtual void setTexture(graphics::Texture<>* texture) noexcept;

        void update(float deltaTime) noexcept override;

        [[nodiscard]] int getDrawOrder() const noexcept { return _drawOrder; }

    private:
        graphics::Texture<>* _texture{ nullptr };
        Actor* _owner;

        int _drawOrder;
    };
} // namespace asteroids::comp
