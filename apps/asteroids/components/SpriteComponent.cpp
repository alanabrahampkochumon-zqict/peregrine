//
// Created by Alan Abraham on 8/22/2026.
//

#include "SpriteComponent.h"

#include "../game/AsteroidGame.h"
#include "psm/PSMath.h"


namespace asteroids::comp
{
    using namespace math;

    // TODO: Check whether draw order and update order refer to the same thing
    SpriteComponent::SpriteComponent(Actor* owner, const int drawOrder)
        : Component{ owner }, _owner{ owner }, _drawOrder{ drawOrder }
    {
        _owner->getGame()->addSprite(this);
        _owner->addComponent(this);
    }


    SpriteComponent::~SpriteComponent() {}


    void SpriteComponent::draw(SDL_Renderer* renderer)
    {
        if (!_texture)
        {
            return; // If there are no textures then don't use the draw call.
        }

        SDL_FRect spriteRect;
        // Scale the rect to the actors width and height
        spriteRect.w = _texture->getWidth() * _owner->getScale();
        spriteRect.h = _texture->getHeight() * _owner->getScale();

        // Center the rectangle around the position of the owner
        spriteRect.x = _owner->getPosition().x - (spriteRect.w * 0.5f);
        spriteRect.y = _owner->getPosition().y - (spriteRect.h * 0.5f);

        const auto rotationRad = -_owner->getRotation() * std::numbers::inv_pi * 180;
        const int renderStatus = SDL_RenderTextureRotated(renderer, _texture->getSDLTexture(), nullptr, &spriteRect,
                                                          rotationRad, nullptr, SDL_FLIP_NONE);
        if (!renderStatus)
        {
            SDL_Log("There was an error rendering the texture.");
        }
    }

    void SpriteComponent::setTexture([[maybe_unused]] graphics::Texture<>* texture)
    {
        // TODO: Impl
    }

    void SpriteComponent::update([[maybe_unused]] float deltaTime) noexcept
    {
        // TODO: Impl
    }

} // namespace asteroids::comp
