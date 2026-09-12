/**
 * @file main.cpp
 * @brief Entry point for RPG Engine, using GameManager state machine with ANSI Console UI.
 * @author Phong & Team
 */

#include "GameManager.h"
#include "ui/ConsoleUI.h"

int main() {
    // Initialize Terminal (Enable ANSI Escape Sequences & UTF-8 on Windows / Linux)
    ConsoleUI::initConsole();
    ConsoleUI::clearScreen();

    // Start Game Manager State Machine
    GameManager game;
    game.run();

    return 0;
}
