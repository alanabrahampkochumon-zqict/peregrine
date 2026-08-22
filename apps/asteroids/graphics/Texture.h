#pragma once
/**
 * @file Texture.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 22, 2026
 *
 * @brief Wrapper for SDLTexture
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */



#include <SDL3/SDL.h>
#include <cstdint>
#include <string>


namespace asteroids::graphics
{
    /// Image Type
    enum class ImageType : uint8_t
    {
        BMP,
        PNG
    };

    template <ImageType Type = ImageType::PNG>
    class Texture
    {
    public:
        /**
         * @brief Load and create a texture from the given file.
         *
         * @param renderer The renderer used.
         * @param path     The path to the texture file.
         */
        explicit Texture(SDL_Renderer& renderer, const std::string& path) noexcept;

        ~Texture() noexcept;

        /// @brief Get the internal SDL texture.
        /// @note User should check for nullptr.
        [[nodiscard]] SDL_Texture* getSDLTexture() const { return _texture; }

    private:
        SDL_Texture* _texture{};
    };
} // namespace asteroids::graphics
