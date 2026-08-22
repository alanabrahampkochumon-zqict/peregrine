/**
 * @file AsteroidGame.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: August 20, 2026
 *
 * @brief Implementation of declarations defined in AsteroidGame.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "AsteroidGame.h"

#include <SDL3/SDL.h>
#include <algorithm>
#include <ranges>

namespace asteroids
{

    bool AsteroidGame::initialize() noexcept
    {
        SDL_SetAppMetadata(GAME_NAME, GAME_VERSION, GAME_ID);

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("There was an error initializing SDL.\n%s", SDL_GetError());
            return false;
        }

        _window = SDL_CreateWindow(GAME_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (!_window)
        {
            SDL_Log("There was an error creating window.\n%s", SDL_GetError());
            return false;
        }


        _isRunning = true;
        return true;
    }

    void AsteroidGame::run() noexcept
    {
        while (_isRunning)
        {
            _handleInput();
            _update();
            _draw();
        }
    }

    void AsteroidGame::shutdown() const noexcept
    {
        /// Remove the actors
        while (!_actors.empty())
        {
            // This calls the actor's dtor which pops the actor from the game's
            // actors.
            // delete actors.back() -> ~Actor() -> game._actors.pop_back()
            delete _actors.back();
        }

        SDL_DestroyWindow(_window);
        SDL_Quit();
    }


    void AsteroidGame::addActor(comp::Actor* actor) noexcept
    {
        // Add the actor to pending list if the game is in an update state
        // else to the actors list.
        if (_isUpdatingActors)
        {
            _pendingActors.emplace_back(actor);
        }
        else
        {
            _actors.emplace_back(actor);
        }
    }


    void AsteroidGame::removeActor(const comp::Actor* actor) noexcept
    {
        // Remove the actor by first searching pending actors
        // and if it's not there remove it from the actors vector.
        auto actorToRemove = std::ranges::find(_pendingActors, actor);
        if (actorToRemove != std::ranges::end(_pendingActors))
        {
            std::ranges::iter_swap(*actorToRemove, _pendingActors.back());
            _pendingActors.pop_back();
        }
        else if (actorToRemove = std::ranges::find(_actors, actor); actorToRemove != std::ranges::end(_actors))
        {
            std::ranges::iter_swap(*actorToRemove, _pendingActors.back());
            _actors.pop_back();
        }
    }


    void AsteroidGame::_handleInput()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    _isRunning = false;
                    break;
                default:
                    break;
            }
        }
    }

    void AsteroidGame::_update()
    {
        while (SDL_GetTicks() < lastFrameTick + 16) {} // Frame Limiting to 60 FPS

        const auto currentTick = SDL_GetTicks();
        float deltaTime        = static_cast<float>(currentTick - lastFrameTick) * 0.001f;
        lastFrameTick          = currentTick;

#ifndef NDEBUG
        // Clamp maximum delta time (useful for debug breaks)
        deltaTime = deltaTime > 0.05f ? 0.05f : deltaTime;
#endif

        // Update the actors
        _isUpdatingActors = true;
        for (const auto actor : _actors)
        {
            actor->update(deltaTime);
        }
        _isUpdatingActors = false;

        // Move any actors from pending actors to actors vector.
        for (auto pendingActor : _pendingActors)
        {
            _actors.emplace_back(pendingActor);
        }
        _pendingActors.clear(); // Clear the pending actors


        // Create a temporary list for dead actors
        std::vector<comp::Actor*> deadActors;
        for (const auto actor : _actors)
        {
            if (actor->getState() == comp::Actor::State::Dead)
            {
                deadActors.emplace_back(actor);
            }
        }

        // Delete the dead actors
        for (const auto actor : deadActors)
        {
            // This invokes the dtor and removes it from the actors as well
            delete actor;
        }


        SDL_Log("Delta time: %0.03f", deltaTime);
    }

    void AsteroidGame::_draw() {}

} // namespace asteroids
