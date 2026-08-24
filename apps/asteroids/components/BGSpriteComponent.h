#pragma once
/**
 * @file BGSpriteComponent.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 24, 2026
 *
 * @brief Scrolling background component.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "SpriteComponent.h"

#include <psm/PSMath.h>
#include <vector>

namespace asteroids
{
    /// Forward declaration
    class Actor;

    namespace comp
    {
        /// @brief Scrolling background component.
        class BGSpriteComponent: public SpriteComponent
        {
        public:
            /**
             * @brief Create background sprite component.
             *
             * @param actor     The owning actor.
             * @param drawOrder The draw order or z-index of the background.
             */
            explicit BGSpriteComponent(Actor* actor, const int drawOrder = 100) noexcept
                : SpriteComponent(actor, drawOrder), _screenSize{ 0.0f, 0.0f }
            {}


            void update(float deltaTime) noexcept override;

            void draw(SDL_Renderer* renderer) override;

            void setBGTexture(const std::vector<graphics::Texture<>*>& textures) noexcept;

            void setScreenSize(const psm::Vec2 screenSize) noexcept { _screenSize = screenSize; }

            void setScrollSpeed(const float speed) noexcept { _speed = speed; }

            [[nodiscard]] float getScrollSpeed() const noexcept { return _speed; }

        private:
            struct BGTexture
            {
                graphics::Texture<>* texture;
                psm::Vec2 offset;
            };

            std::vector<BGTexture> _bgTextures;
            psm::Vec2 _screenSize;
            float _speed{};
        };
    } // namespace comp
} // namespace asteroids
