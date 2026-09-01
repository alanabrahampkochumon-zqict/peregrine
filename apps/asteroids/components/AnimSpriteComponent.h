#pragma once
/**
 * @file AnimSpriteComponent.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 24, 2026
 *
 * @brief Sprite component supporting animatable graphics.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "SpriteComponent.h"
#include "graphics/Texture.h"

#include <vector>

namespace asteroids::comp
{

    /// Sprite component with texture based animation looping.
    class AnimSpriteComponent: public SpriteComponent
    {

    public:
        static constexpr float DEFAULT_SPRITE_ANIM_FPS = 30; ///< Default sprite animation frame rate.

        /**
         * @brief Create an animated sprite component.
         *
         * @param owner     The actor that owns the sprite component.
         * @param drawOrder The drawing order or z-index.
         */
        [[nodiscard]] explicit AnimSpriteComponent(actors::Actor* owner, const int drawOrder = 100)
            : SpriteComponent{ owner, drawOrder }
        {}


        /**
         * @brief Set the textures used for animation.
         *
         * @param textures The texture set/vector used for animation.
         */
        constexpr void setAnimTextures(const std::vector<graphics::Texture<>*>& textures) noexcept
        { _textures = textures; }


        /**
         * @brief Update the sprite component.
         * @param deltaTime The frame time.
         */
        void update(float deltaTime) noexcept override;


        /**
         * @brief Set the animation frame rate(fps).
         * @param fps The fps to set.
         */
        constexpr void setFPS(const float fps) noexcept { _fps = fps; }

        /// @brief Get the current animation frame rate.
        [[nodiscard]] constexpr float getFPS() const noexcept { return _fps; }


        ~AnimSpriteComponent() noexcept override
        {
            // _owner->getGame().remove
        }

    private:
        std::vector<graphics::Texture<>*> _textures;
        float _currentFrame{ 0 }, _fps{ DEFAULT_SPRITE_ANIM_FPS };
    };
} // namespace asteroids::comp
