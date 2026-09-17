#pragma once

#include "Hero.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "Enemy.h"
#include "Minion.h"
#include "BossMonster.h"
#include "CombatEngine.h"
#include "DamageCalculator.h"
#include "Item.h"
#include "Inventory.h"
#include "StoryGraph.h"
#include "SaveManager.h"
#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include "ui/BattleUI.h"
#include "ui/InventoryUI.h"
#include "ui/MainMenu.h"
#include <memory>

class Game {
private:
    std::unique_ptr<Hero> hero;
    Inventory inventory;
    StoryGraph story;
    SaveManager saveManager;
    MainMenu mainMenu;
    BattleUI battleUI;
    InventoryUI inventoryUI;
    bool isRunning;

    void handleNewGame();
    void handleLoadGame();
    void handleSaveGame();
    void handleInventoryMenu();
    void handleStoryLoop();
    bool executeCombat(std::shared_ptr<Enemy> enemy);

public:
    Game();
    ~Game() = default;

    void run();
};
