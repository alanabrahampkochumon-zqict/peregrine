/**
 * @file BGSpriteComponent.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 24, 2026
 *
 * @brief Implementation of header files defined in BGSpriteComponents.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "BGSpriteComponent.h"


namespace asteroids::comp
{

    void BGSpriteComponent::update(const float deltaTime) noexcept
    {
        SpriteComponent::update(deltaTime);
        // Update each texture's offset to create the scrolling motion
        // [....][....] => [...][....][.] => [..][....][..]
        for (auto& [texture, offset] : _bgTextures)
        {
            offset.x = _speed * deltaTime;
            // If the texture has moved passed the screen bounds
            // move the offset to the image position's x where we can
            // make it loop around
            if (offset.x < -_screenSize.x)
            {
                offset.x = static_cast<float>(_bgTextures.size() - 1) * _screenSize.x - 1;
            }
        }
    }


    void BGSpriteComponent::draw(SDL_Renderer* renderer)
    {
        SpriteComponent ::draw(renderer);
    }


    void BGSpriteComponent::setBGTexture(const std::vector<graphics::Texture<>*>& textures) noexcept
    {
        size_t count = 0;
        for (const auto& texture : textures)
        {
            BGTexture bgTexture;
            bgTexture.texture = texture;
            // FIXME: Update code as per background texture
            // currently the code assume backgrounds are tiled horizontally.
            bgTexture.offset.x = static_cast<float>(count * texture->getWidth());
            bgTexture.offset.y = 0;
            ++count;
            _bgTextures.emplace_back(bgTexture);
        }
    }

} // namespace asteroids::comp
