#pragma once
/**
 * @file SpriteSheetComponent.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 25, 2026
 *
 * @brief Component for handling a set of sprites.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "SpriteComponent.h"

namespace asteroids::comp
{

    struct SpriteIndex
    {
        size_t x, y;
    };

    class SpriteSheetComponent: public Component
    {
    public:
        explicit SpriteSheetComponent(Actor* owner, const int drawOrder = 100);

        ~SpriteSheetComponent() override = default;

        void update(float deltaTime) noexcept override;

        void draw(SDL_Renderer* renderer) const noexcept;


        [[nodiscard]] int getDrawOrder() const noexcept { return _drawOrder; }
        [[nodiscard]] SpriteIndex getActiveIndex() const noexcept { return _activeSpriteIndex; }

        void setSpritesheetTexture(graphics::Texture<>* texture, size_t spriteWidth, size_t spriteHeight) noexcept;
        void setActiveIndex(const SpriteIndex index) noexcept { _activeSpriteIndex = index; }

    private:
        SpriteIndex _activeSpriteIndex{};
        int _drawOrder;
        size_t _horizontalSpriteCount{}, _verticalSpriteCount{};
        size_t _spriteWidth{}, _spriteHeight{};
        graphics::Texture<>* _spriteSheetTexture{};
    };
} // namespace asteroids::comp
