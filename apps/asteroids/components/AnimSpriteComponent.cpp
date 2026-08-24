/**
 * @file AnimSpriteComponent.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 24, 2026
 *
 * @brief Buffer update
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "AnimSpriteComponent.h"

namespace asteroids::comp
{
    void AnimSpriteComponent::update(const float deltaTime) noexcept
    {
        SpriteComponent::update(deltaTime);
        if (_textures.size() > 0) // TODO: Update to an assert
        {
            // Increment current frame with respect to delta time
            // to ensure decoupled animation(from frame rate of the game play loop)
            // 30 vs 60 fps should produce the same animation change in 1 sec
            _currentFrame += _fps * deltaTime;

            // TODO: Change the conditional to modulo evaluation
            // which can be sped up by using rem = _currentFrame - _fps * _fpsQuotient(this can calculated in the
            // setter)
            while (_currentFrame >= static_cast<float>(_textures.size()))
            {
                _currentFrame -= static_cast<float>(_textures.size());
            }

            // Set the current sprite texture
            setTexture(_textures[static_cast<int>(_currentFrame)]);
        }
    }
} // namespace asteroids::comp
