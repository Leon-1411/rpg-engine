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
    STORY_CODEX = 5,
    UNDER_DEV_FEATURES = 6,
    EXIT = 7
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

    // Show Story Progress & Endings Codex
    void showStoryCodex(const StoryGraph& story);

    // Show warning for features currently in development
    void showUnderDevelopmentWarning();

    // Action handlers
    void handleNewGame();
    void handleLoadGame();
    void playStoryLoop(Hero& hero, StoryGraph& story);
};
