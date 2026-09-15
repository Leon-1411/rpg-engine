#include "GameManager.h"
#include "ui/ConsoleUI.h"
#include "ui/BattleUI.h"
#include "ui/InventoryUI.h"
#include "CombatEngine.h"
#include "Minion.h"
#include <iostream>

GameManager::GameManager() 
    : currentState(GameState::INIT), saveManager("saves/") {}

void GameManager::run() {
    while (currentState != GameState::EXIT) {
        switch (currentState) {
            case GameState::INIT:
                handleInit();
                break;
            case GameState::MAIN_MENU:
                handleMainMenu();
                break;
            case GameState::STORY_MODE:
                handleStoryMode();
                break;
            case GameState::BATTLE_MODE:
                handleBattleMode();
                break;
            case GameState::INVENTORY_MODE:
                handleInventoryMode();
                break;
            case GameState::GAME_OVER:
                handleGameOver();
                break;
            case GameState::EXIT:
                // Exit condition
                break;
        }
    }
}

void GameManager::changeState(GameState newState) {
    currentState = newState;
}

void GameManager::handleInit() {
    // Basic setup if any
    changeState(GameState::MAIN_MENU);
}

void GameManager::handleMainMenu() {
    MainMenuOption option = mainMenu.showMenu();
    
    switch (option) {
        case MainMenuOption::NEW_GAME:
            startNewGame();
            break;
        case MainMenuOption::LOAD_GAME:
            loadGame();
            break;
        case MainMenuOption::HERO_SHOWCASE:
            mainMenu.showHeroShowcase();
            break;
        case MainMenuOption::HELP:
            mainMenu.showHelp();
            break;
        case MainMenuOption::EXIT:
            changeState(GameState::EXIT);
            break;
    }
}

void GameManager::startNewGame() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("TẠO NHÂN VẬT MỚI", 50, ConsoleUI::Colors::BRIGHT_CYAN);
    
    // Hardcoded Arthur for demo purposes, can be expanded to hero creation
    playerHero = std::make_shared<Hero>("Arthur", HeroClass::WARRIOR, 100, 30, 20, 5);
    
    ConsoleUI::printSuccess("Khởi tạo nhân vật Arthur (Warrior) thành công!");
    ConsoleUI::pause();
    
    // Start at story mode
    changeState(GameState::STORY_MODE);
}

void GameManager::loadGame() {
    std::vector<int> slots = saveManager.getExistingSlots();
    int slotToLoad = mainMenu.showLoadGameMenu(slots);
    
    if (slotToLoad != -1) {
        playerHero = std::make_shared<Hero>("Blank", HeroClass::WARRIOR, 1, 1, 1, 1);
        if (saveManager.loadGame(slotToLoad, *playerHero, story)) {
            ConsoleUI::printSuccess("Tải game thành công!");
            ConsoleUI::pause();
            changeState(GameState::STORY_MODE);
        } else {
            ConsoleUI::printError("Lỗi tải game!");
            ConsoleUI::pause();
        }
    }
}

void GameManager::handleStoryMode() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("CHẾ ĐỘ CỐT TRUYỆN (STORY MODE)", 60, ConsoleUI::Colors::BRIGHT_MAGENTA);
    
    StoryNode current = story.getCurrentNode();
    std::vector<std::string> storyBox = {
        "Node ID: " + current.id,
        current.text
    };
    ConsoleUI::printBox(storyBox, 60, ConsoleUI::Colors::BRIGHT_MAGENTA);
    
    std::cout << "\nCác lựa chọn của bạn:\n";
    std::cout << "1. Chiến đấu ngẫu nhiên (Demo BATTLE_MODE)\n";
    std::cout << "2. Mở túi đồ (Demo INVENTORY_MODE)\n";
    std::cout << "3. Lưu game (Save Slot 1)\n";
    std::cout << "4. Quay lại Menu Chính\n";
    std::cout << "5. Thoát Game\n";
    
    int choice = ConsoleUI::getIntInput(1, 5, "Nhập lựa chọn: ");
    
    if (choice == 1) {
        changeState(GameState::BATTLE_MODE);
    } else if (choice == 2) {
        changeState(GameState::INVENTORY_MODE);
    } else if (choice == 3) {
        if (playerHero) {
            saveManager.saveGame(1, *playerHero, story);
            ConsoleUI::printSuccess("Đã lưu tiến trình!");
        } else {
            ConsoleUI::printError("Lỗi: Không tìm thấy Hero!");
        }
        ConsoleUI::pause();
    } else if (choice == 4) {
        changeState(GameState::MAIN_MENU);
    } else if (choice == 5) {
        changeState(GameState::EXIT);
    }
}

void GameManager::handleBattleMode() {
    auto enemy = MinionFactory::createFromJson("goblin", "data/enemies.json");
    if (!enemy) enemy = std::make_shared<Goblin>();

    BattleUI battleUI;
    CombatEngine combat(*playerHero, *enemy);
    combat.startBattle();

    while (!combat.isBattleOver()) {
        battleUI.renderBattleScreen(*playerHero, *enemy, "Tới lượt của bạn!");
        BattleAction action = battleUI.getPlayerAction();
        
        int actionCode = static_cast<int>(action);
        combat.executeTurn(actionCode);
    }

    if (combat.getState() == CombatState::HERO_VICTORY) {
        battleUI.showVictory(*enemy);
        changeState(GameState::STORY_MODE);
    } else if (combat.getState() == CombatState::ENEMY_VICTORY) {
        changeState(GameState::GAME_OVER);
    } else {
        changeState(GameState::STORY_MODE); // Fled
    }
}

void GameManager::handleInventoryMode() {
    InventoryUI invUI;
    bool inInventory = true;
    while (inInventory) {
        invUI.renderInventory(playerHero->getInventory());
        int itemIdx = invUI.selectItemIndex(playerHero->getInventory());
        
        if (itemIdx == -1) {
            inInventory = false;
        } else {
            std::shared_ptr<Item> item = playerHero->getInventory().getItemPtr(itemIdx);
            if (item) {
                int action = invUI.selectItemAction(*item);
                if (action == 1 && item->getType() == ItemType::POTION) {
                    playerHero->getInventory().useItem(itemIdx, *playerHero);
                } else if (action == 2) {
                    if (item->getType() == ItemType::WEAPON) {
                        playerHero->getInventory().equipWeapon(itemIdx);
                    } else if (item->getType() == ItemType::ARMOR) {
                        playerHero->getInventory().equipArmor(itemIdx);
                    }
                } else if (action == 3) {
                    playerHero->getInventory().removeItem(itemIdx);
                }
            }
        }
    }
    changeState(GameState::STORY_MODE);
}

void GameManager::handleGameOver() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("GAME OVER", 60, ConsoleUI::Colors::BRIGHT_RED);
    std::cout << "Nhân vật của bạn đã ngã xuống!\n";
    ConsoleUI::pause();
    changeState(GameState::MAIN_MENU);
}
