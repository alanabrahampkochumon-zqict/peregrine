/**
 * @file SpriteSheetComponent.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 25, 2026
 *
 * @brief Implementation of member functions defined in SpriteSheetComponent.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "SpriteSheetComponent.h"

#include "../actors/Actor.h"
#include "game/AsteroidGame.h"

#include <cassert>


namespace asteroids::comp
{

    SpriteSheetComponent::SpriteSheetComponent(actors::Actor* owner, const int drawOrder)
        : Component{ owner }, _drawOrder{ drawOrder }
    {
        _owner->getGame()->addSprite(this);
        _owner->addComponent(this);
    }

    SpriteSheetComponent::~SpriteSheetComponent() noexcept { _owner->getGame()->removeSprite(this); }


    void SpriteSheetComponent::update([[maybe_unused]] const float deltaTime) noexcept
    {
        // TODO: Impl
    }

    void SpriteSheetComponent::draw([[maybe_unused]] SDL_Renderer* renderer) const noexcept
    {
        assert(nullptr != _spriteSheetTexture &&
               "Spritesheet cannot be nullptr. Set your spritesheet using setSpritesheetTexture()");

        SDL_FRect drawTarget, srcTarget;
        // Scale the target rect to be scaled by the sprite's width and height
        drawTarget.w = static_cast<float>(_spriteWidth) * _owner->getScale();
        drawTarget.h = static_cast<float>(_spriteHeight) * _owner->getScale();

        // Center the sprite
        drawTarget.x = _owner->getPosition().x - drawTarget.w * 0.5f;
        drawTarget.y = _owner->getPosition().y - drawTarget.h * 0.5f;

        // Define the src target to be sprite dimension with offset
        srcTarget.w = static_cast<float>(_spriteWidth);
        srcTarget.h = static_cast<float>(_spriteHeight);
        srcTarget.x = static_cast<float>(_spriteWidth * _activeSpriteIndex.x);
        srcTarget.y = static_cast<float>(_spriteHeight * _activeSpriteIndex.y);

#ifdef ENABLE_SPRITE_DEBUG
        SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);
        SDL_RenderRect(renderer, &drawTarget);
#endif


        const auto rotation     = -math::toRad(_owner->getRotation());
        const auto renderStatus = SDL_RenderTextureRotated(renderer, _spriteSheetTexture->getSDLTexture(), &srcTarget,
                                                           &drawTarget, rotation, nullptr, SDL_FLIP_NONE);
        if (!renderStatus)
        {
            SDL_Log("There was an error rendering the texture.");
        }
    }

    void SpriteSheetComponent::setSpritesheetTexture(graphics::Texture<>* texture, const size_t spriteWidth,
                                                     const size_t spriteHeight) noexcept
    {
        _spriteSheetTexture = texture;
        // Calculate the number of sprites in the sprite sheet from the texture dimension and passed in
        // sprite dimension
        _spriteWidth           = spriteWidth;
        _spriteHeight          = spriteHeight;
        _horizontalSpriteCount = texture->getWidth() / spriteWidth;
        _verticalSpriteCount   = texture->getHeight() / spriteHeight;
    }

} // namespace asteroids::comp
