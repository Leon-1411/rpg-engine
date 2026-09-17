#pragma once

#include <string>
#include <vector>
#include "Hero.h"
#include "StoryGraph.h"

enum class MainMenuOption {
    NEW_GAME = 1,
    LOAD_GAME = 2,
    HERO_SHOWCASE = 3,
    HELP = 4,
    EXIT = 5
};

class MainMenu {
public:
    MainMenu() = default;
    ~MainMenu() = default;

    // Main interactive loop
    void run();

    // Display the title and options, returns user's choice
    MainMenuOption showMenu();

    // Show Hero Class showcase with ASCII art and stats
    void showHeroShowcase();

    // Show Load Game slot selection menu (-1 if cancelled)
    int showLoadGameMenu(const std::vector<int>& availableSlots);

    // Show guide & instructions
    void showHelp();

    // Action handlers
    void handleNewGame();
    void handleLoadGame();
    void playStoryLoop(Hero& hero, StoryGraph& story);
};
