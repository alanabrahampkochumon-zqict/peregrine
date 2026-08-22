//
// Created by Alan Abraham on 8/22/2026.
//

#include "SpriteComponent.h"

#include "../game/AsteroidGame.h"

namespace asteroids::comp
{
    // TODO: Check whether draw order and update order refer to the same thing
    SpriteComponent::SpriteComponent(Actor* owner, const int drawOrder)
        : Component{ owner }, _owner{ owner }, _drawOrder{ drawOrder }
    {
        _owner->getGame()->addSprite(this);
        _owner->addComponent(this);
    }


    SpriteComponent::~SpriteComponent() {}


    void SpriteComponent::draw([[maybe_unused]] SDL_Renderer* renderer)
    {
        // TODO: Impl
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
