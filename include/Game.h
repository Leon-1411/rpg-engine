#pragma once

#include "Hero.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "Enemy.h"
#include "Minion.h"
#include "BossMonster.h"
#include "CombatEngine.h"
#include "StoryGraph.h"
#include "SaveManager.h"
#include "ui/ConsoleUI.h"
#include "ui/MainMenu.h"
#include "ui/BattleUI.h"
#include "ui/InventoryUI.h"
#include "ui/ASCIIArt.h"
#include <memory>
#include <string>

class Game {
private:
    std::shared_ptr<Hero> player;
    StoryGraph story;
    SaveManager saveManager;
    MainMenu mainMenu;
    BattleUI battleUI;
    InventoryUI inventoryUI;
    bool isRunning;

public:
    explicit Game(const std::string& storyFile = "data/story.json", 
                  const std::string& saveDir = "saves/");
    ~Game() = default;

    // Khởi chạy vòng lặp trò chơi chính
    void run();

private:
    // Khởi tạo hoặc tải game
    void startNewGame();
    void loadGame();

    // Vòng lặp khám phá cốt truyện & sự kiện
    void runStoryLoop();

    // Xử lý từng loại StoryNode
    void handleStoryNode(const StoryNode& node);
    void handleCombatNode(const StoryNode& node);
    void handleRewardNode(const StoryNode& node);
    void handleRequirementCheckNode(const StoryNode& node);
    void handleShopNode(const StoryNode& node);
    void handleEndingNode(const StoryNode& node);

    // Menu trung gian (Mở túi đồ, xem chỉ số, Lưu game, Tiếp tục)
    void openInGameMenu();
    void openInventoryScreen();
    void saveCurrentGame();

    // Helper tạo quái vật theo ID từ JSON (hỗ trợ Minion & Boss)
    std::shared_ptr<Enemy> createEnemyById(const std::string& enemyId);
};
