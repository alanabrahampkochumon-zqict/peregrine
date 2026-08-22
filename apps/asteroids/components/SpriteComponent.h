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

namespace asteroids
{
    class Actor;

    namespace comp
    {
        class SpriteComponent: Component
        {
        public:
            explicit SpriteComponent(Actor* owner, int drawOrder = 100);

            ~SpriteComponent() override;

            virtual void draw(SDL_Renderer* renderer);

            virtual void setTexture(graphics::Texture<>* texture);

            void update(float deltaTime) noexcept override;

            int getDrawOrder() const noexcept { return _drawOrder; }

        private:
            graphics::Texture<>* _texture{ nullptr };
            Actor* _owner;

            int _drawOrder;
            int _textureWidth;
            int _textureHeight;
        };
    } // namespace comp
} // namespace asteroids
