/**
 * @file Main.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Entry point for Demo application.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "game/AsteroidGame.h"

#include <stdlib.h>


int main()
{
    asteroids::AsteroidGame game;
    if (game.initialize())
    {
        game.run();
    }
    game.shutdown();
    return EXIT_SUCCESS;
}
