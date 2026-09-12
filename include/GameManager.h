#pragma once

#include "Hero.h"
#include "StoryGraph.h"
#include "SaveManager.h"
#include "ui/MainMenu.h"
#include <memory>

enum class GameState {
    INIT,
    MAIN_MENU,
    STORY_MODE,
    BATTLE_MODE,
    INVENTORY_MODE,
    GAME_OVER,
    EXIT
};

class GameManager {
private:
    GameState currentState;
    std::shared_ptr<Hero> playerHero;
    StoryGraph story;
    SaveManager saveManager;
    MainMenu mainMenu;

    std::string currentEnemyId;
    std::string currentWinNodeId;
    std::string currentLoseNodeId;

    void handleInit();
    void handleMainMenu();
    void handleStoryMode();
    void handleBattleMode();
    void handleInventoryMode();
    void handleGameOver();

    // Helper functions
    void startNewGame();
    void loadGame();
    void giveItemById(const std::string& itemId);

public:
    GameManager();
    ~GameManager() = default;

    void run();
    void changeState(GameState newState);
};
