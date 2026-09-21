/**
 * @file main.cpp
 * @brief Entry point for RPG Engine: initializes Console UI and runs Game coordinator loop.
 * @author Phong & Team
 */

#include "Game.h"
#include "ui/ConsoleUI.h"
#include <iostream>

int main() {
    // 1. Initialize Console (UTF-8 & ANSI Escape Sequences)
    ConsoleUI::initConsole();

    // 2. Instantiate Game Coordinator & Run Gameplay Loop
    try {
        Game game("data/story.json", "saves/");
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "[Fatal Error] RPG Engine Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
