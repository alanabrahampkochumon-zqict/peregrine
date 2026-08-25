/**
 * @file Texture.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 22, 2026
 *
 * @brief Implementation of member functions defined in Texture.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


namespace asteroids::graphics
{
    template <ImageType Type>
    Texture<Type>::Texture(SDL_Renderer* renderer, const std::string& path) noexcept
    {
        SDL_Surface* image;
        if constexpr (Type == ImageType::BMP)
        {
            image = SDL_LoadBMP(path.c_str());
        }
        else if (Type == ImageType::PNG)
        {
            image = SDL_LoadPNG(path.c_str());
        }
        else
        {
            SDL_Log("Unsupported Texture Format");
            return;
        }
        _texture = SDL_CreateTextureFromSurface(renderer, image);
        if (!_texture)
        {
            SDL_Log("There was an error creating the texture");
        }
        else
        {
            _width  = _texture->w;
            _height = _texture->h;
        }

        // We need to free the surface/image
        SDL_DestroySurface(image);
    }


    template <ImageType Type>
    Texture<Type>::~Texture() noexcept
    { SDL_DestroyTexture(_texture); }
} // namespace asteroids::graphics
