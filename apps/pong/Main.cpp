/**
 * @file Main.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Entry point for Demo application.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "include/Pong.h"

int main()
{
    pong::Game game;
    game.initialize();
    game.run();
    game.shutdown();
}
