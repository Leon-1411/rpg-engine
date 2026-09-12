#include "GameManager.h"
#include "ui/ConsoleUI.h"
#include "ui/BattleUI.h"
#include "ui/InventoryUI.h"
#include "CombatEngine.h"
#include "Minion.h"
#include "BossMonster.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "DataLoader.h"
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

void GameManager::giveItemById(const std::string& itemId) {
    auto items = DataLoader::loadItems("data/items.json");
    for (const auto& itm : items) {
        if (itm.getId() == itemId) {
            if (playerHero) {
                playerHero->getInventory().addItem(itm);
                std::cout << "  + [Vật phẩm] " << itm.getName() << " (" << itm.getDescription() << ")\n";
            }
            return;
        }
    }
    // Fallback if not in items.json
    Item fallback(itemId, itemId, "Special Quest Item", ItemType::ARMOR, 0);
    if (playerHero) {
        playerHero->getInventory().addItem(fallback);
        std::cout << "  + [Vật phẩm đặc biệt] " << itemId << "\n";
    }
}

void GameManager::startNewGame() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("CHỌN LỚP NHÂN VẬT (HERO CLASS)", 60, ConsoleUI::Colors::BRIGHT_CYAN);
    
    std::cout << ConsoleUI::colorize("1. Warrior (Arthur)  ", ConsoleUI::Colors::BRIGHT_YELLOW) 
              << "- HP/DEF Cao, Chuyên Cận Chiến & Đỡ Đòn\n";
    std::cout << ConsoleUI::colorize("2. Mage (Morrigan)    ", ConsoleUI::Colors::BRIGHT_MAGENTA) 
              << "- MP Dồi Dào, Sát Thương Phép & Khắc Chế Cổ Ngữ\n";
    std::cout << ConsoleUI::colorize("3. Ranger (Lyra)      ", ConsoleUI::Colors::BRIGHT_GREEN) 
              << "- Nhanh Nhẹn, Bạo Kích Cao, Do Thám Rừng Sâu\n\n";
    
    int classChoice = ConsoleUI::getIntInput(1, 3, "Chọn lớp nhân vật của bạn (1-3): ");
    
    std::cout << "\nNhập tên nhân vật (Nhấn Enter để dùng tên mặc định): ";
    std::string heroName;
    std::getline(std::cin, heroName);
    
    if (classChoice == 1) {
        if (heroName.empty()) heroName = "Arthur";
        playerHero = std::make_shared<Warrior>(heroName);
        giveItemById("wpn_01");
        giveItemById("arm_01");
        giveItemById("pot_01");
        giveItemById("pot_01");
    } else if (classChoice == 2) {
        if (heroName.empty()) heroName = "Morrigan";
        playerHero = std::make_shared<Mage>(heroName);
        giveItemById("wpn_02");
        giveItemById("arm_02");
        giveItemById("pot_02");
        giveItemById("pot_02");
        giveItemById("pot_01");
    } else {
        if (heroName.empty()) heroName = "Lyra";
        playerHero = std::make_shared<Ranger>(heroName);
        giveItemById("wpn_03");
        giveItemById("arm_03");
        giveItemById("pot_01");
        giveItemById("pot_01");
    }

    story.loadStoryGraph("data/story.json");
    
    ConsoleUI::printSuccess("Khởi tạo nhân vật " + playerHero->getName() + " [" + playerHero->getHeroClassName() + "] thành công!");
    ConsoleUI::pause();
    
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
    StoryNode current = story.getCurrentNode();
    if (current.id.empty()) {
        story.loadStoryGraph("data/story.json");
        current = story.getCurrentNode();
    }

    std::string headerTitle = current.title.empty() ? ("CỐT TRUYỆN - " + current.id) : current.title;
    ConsoleUI::printHeader(headerTitle, 65, ConsoleUI::Colors::BRIGHT_MAGENTA);

    // 1. Check if Node is ENDING or GAME_OVER
    if (current.type == EventType::ENDING || current.rawType == "ENDING" || current.id == "GameOver") {
        std::vector<std::string> endingBox = {
            current.title,
            "",
            current.text
        };
        if (current.id == "End5") {
            ConsoleUI::printBox(endingBox, 65, ConsoleUI::Colors::BRIGHT_GREEN);
            ConsoleUI::printSuccess("\n★ CHÚC MỪNG! BẠN ĐÃ ĐẠT ĐƯỢC TRUE ENDING CỦA FRACTURED CROWN! ★");
        } else if (current.id == "GameOver") {
            ConsoleUI::printBox(endingBox, 65, ConsoleUI::Colors::BRIGHT_RED);
            ConsoleUI::printError("\nBạn đã thất bại trong hành trình...");
        } else {
            ConsoleUI::printBox(endingBox, 65, ConsoleUI::Colors::BRIGHT_YELLOW);
            std::cout << "\nBạn đã hoàn thành kết cục của cốt truyện!\n";
        }
        ConsoleUI::pause();
        changeState(GameState::MAIN_MENU);
        return;
    }

    // 2. Check if Node is REQUIREMENT_CHECK
    if (current.rawType == "REQUIREMENT_CHECK") {
        std::vector<std::string> checkLines = {
            current.title,
            current.text
        };
        ConsoleUI::printBox(checkLines, 65, ConsoleUI::Colors::BRIGHT_CYAN);
        std::cout << "\n[Hệ Thống] Đang kiểm tra điều kiện kích hoạt...\n";

        bool passed = true;
        // Check Hero Class if required
        if (!current.requiredHeroClass.empty()) {
            if (playerHero && playerHero->getHeroClassName() != current.requiredHeroClass) {
                passed = false;
                std::cout << "-> Yêu cầu lớp nhân vật " << current.requiredHeroClass << " (Hiện tại: " << playerHero->getHeroClassName() << "): THẤT BẠI!\n";
            } else {
                std::cout << "-> Yêu cầu lớp nhân vật " << current.requiredHeroClass << ": ĐẠT!\n";
            }
        }

        // Check required items if any
        for (const auto& itmId : current.requiredItems) {
            if (playerHero && !playerHero->getInventory().hasItem(itmId)) {
                passed = false;
                std::cout << "-> Yêu cầu vật phẩm [" << itmId << "]: THIẾU!\n";
            } else {
                std::cout << "-> Yêu cầu vật phẩm [" << itmId << "]: CÓ SẴN!\n";
            }
        }

        ConsoleUI::pause();
        if (passed) {
            ConsoleUI::printSuccess(">> Điều kiện thỏa mãn! Tiếp tục hành trình.");
            story.moveToNode(current.onPassNodeId);
        } else {
            ConsoleUI::printWarning(">> Không đủ điều kiện! Chuyển hướng nhánh rẽ.");
            story.moveToNode(current.onFailNodeId);
        }
        return;
    }

    // 3. Check if Node is REWARD
    if (current.type == EventType::ITEM || current.rawType == "REWARD") {
        std::vector<std::string> rewardBox = {
            current.title,
            current.text
        };
        ConsoleUI::printBox(rewardBox, 65, ConsoleUI::Colors::BRIGHT_GREEN);

        if (!current.rewardItems.empty() || current.rewardExp > 0) {
            std::cout << "\n" << ConsoleUI::colorize("★ PHẦN THƯỞNG NHẬN ĐƯỢC:", ConsoleUI::Colors::BRIGHT_YELLOW) << "\n";
            for (const auto& itmId : current.rewardItems) {
                giveItemById(itmId);
            }
            if (current.rewardExp > 0 && playerHero) {
                playerHero->setExp(playerHero->getExp() + current.rewardExp);
                std::cout << "  + " << current.rewardExp << " EXP\n";
            }
        }

        if (current.choices.empty()) {
            ConsoleUI::pause();
            if (!current.nextNodeId.empty()) {
                story.moveToNode(current.nextNodeId);
            }
            return;
        }
    } else {
        // Normal / Story Node
        std::vector<std::string> storyBox = {
            current.title,
            "",
            current.text
        };
        ConsoleUI::printBox(storyBox, 65, ConsoleUI::Colors::BRIGHT_MAGENTA);
    }

    // 4. Check if Node is COMBAT
    if (current.type == EventType::BATTLE || current.rawType == "COMBAT") {
        std::cout << "\n" << ConsoleUI::colorize("⚔ Chiến trường nguy hiểm phía trước! Đối thủ: " + current.enemyId, ConsoleUI::Colors::BRIGHT_RED) << "\n";
        std::cout << "1. Bước vào chiến đấu\n";
        std::cout << "2. Mở túi đồ (Chuẩn bị)\n";
        std::cout << "3. Lưu tiến trình game (Slot 1)\n";
        std::cout << "4. Quay lại Menu Chính\n";

        int opt = ConsoleUI::getIntInput(1, 4, "Lựa chọn của bạn: ");
        if (opt == 1) {
            currentEnemyId = current.enemyId;
            currentWinNodeId = current.onWinNodeId;
            currentLoseNodeId = current.onLoseNodeId;
            changeState(GameState::BATTLE_MODE);
        } else if (opt == 2) {
            changeState(GameState::INVENTORY_MODE);
        } else if (opt == 3) {
            if (playerHero) {
                saveManager.saveGame(1, *playerHero, story);
                ConsoleUI::printSuccess("Đã lưu tiến trình vào Slot 1!");
            }
            ConsoleUI::pause();
        } else if (opt == 4) {
            changeState(GameState::MAIN_MENU);
        }
        return;
    }

    // 5. Render Story Choices
    std::cout << "\n" << ConsoleUI::colorize("Các ngã rẽ lựa chọn của bạn:", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    int numChoices = static_cast<int>(current.choices.size());
    for (int i = 0; i < numChoices; ++i) {
        std::cout << "  " << (i + 1) << ". " << current.choices[i].text << "\n";
    }

    // Utility options
    int optInv = numChoices + 1;
    int optSave = numChoices + 2;
    int optMenu = numChoices + 3;

    std::cout << "\n--- Tiện ích ---\n";
    std::cout << "  " << optInv << ". Mở túi đồ (Inventory)\n";
    std::cout << "  " << optSave << ". Lưu game (Save Game Slot 1)\n";
    std::cout << "  " << optMenu << ". Quay về Menu chính\n";

    int choice = ConsoleUI::getIntInput(1, optMenu, "\nNhập lựa chọn của bạn: ");

    if (choice >= 1 && choice <= numChoices) {
        story.selectChoice(choice - 1);
    } else if (choice == optInv) {
        changeState(GameState::INVENTORY_MODE);
    } else if (choice == optSave) {
        if (playerHero) {
            saveManager.saveGame(1, *playerHero, story);
            ConsoleUI::printSuccess("Đã lưu game vào Slot 1 thành công!");
        }
        ConsoleUI::pause();
    } else if (choice == optMenu) {
        changeState(GameState::MAIN_MENU);
    }
}

void GameManager::handleBattleMode() {
    std::shared_ptr<Enemy> enemy = nullptr;

    if (!currentEnemyId.empty()) {
        enemy = BossFactory::createFromJson(currentEnemyId, "data/enemies.json");
        if (!enemy) {
            enemy = MinionFactory::createFromJson(currentEnemyId, "data/enemies.json");
        }
    }

    if (!enemy) {
        enemy = MinionFactory::createFromJson("goblin", "data/enemies.json");
        if (!enemy) enemy = std::make_shared<Goblin>();
    }

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
        if (!currentWinNodeId.empty()) {
            story.moveToNode(currentWinNodeId);
        }
        changeState(GameState::STORY_MODE);
    } else if (combat.getState() == CombatState::ENEMY_VICTORY) {
        if (!currentLoseNodeId.empty()) {
            story.moveToNode(currentLoseNodeId);
            changeState(GameState::STORY_MODE);
        } else {
            changeState(GameState::GAME_OVER);
        }
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
    std::cout << "Nhân vật của bạn đã ngã xuống nơi chiến trường!\n";
    ConsoleUI::pause();
    changeState(GameState::MAIN_MENU);
}
