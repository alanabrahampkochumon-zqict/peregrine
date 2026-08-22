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


namespace asteroids::comp
{
    class Texture
    {
    public:
        /// Image Type
        enum class Type : uint8_t
        {
            PNG,
            JPEG,
            TIFF
        };

        /**
         * @brief Load and create a texture from the given file.
         *
         * @param path The path to the texture file.
         * @param type The mimetype of the file. @see Texture::Type
         */
        explicit Texture(std::string path, Type type = Type::PNG) noexcept;

    private:
        SDL_Texture _texture{};
    };
} // namespace asteroids::comp
