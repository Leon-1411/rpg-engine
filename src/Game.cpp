/**
 * @file Game.cpp
 * @brief Implement Game coordinator class for RPG Engine.
 */

#include "Game.h"
#include <iostream>
#include <sstream>

Game::Game() : saveManager("saves/"), isRunning(true) {}

void Game::run() {
    ConsoleUI::initConsole();

    while (isRunning) {
        ConsoleUI::clearScreen();
        MainMenuOption option = mainMenu.showMenu();

        switch (option) {
            case MainMenuOption::NEW_GAME:
                handleNewGame();
                break;
            case MainMenuOption::LOAD_GAME:
                handleLoadGame();
                break;
            case MainMenuOption::HERO_SHOWCASE:
                mainMenu.showHeroShowcase();
                break;
            case MainMenuOption::HELP:
                mainMenu.showHelp();
                break;
            case MainMenuOption::EXIT:
                isRunning = false;
                ConsoleUI::clearScreen();
                std::cout << ConsoleUI::colorize("\n  Cảm ơn bạn đã trải nghiệm RPG Engine! Hẹn gặp lại!\n\n", 
                                                  ConsoleUI::Colors::BRIGHT_CYAN);
                break;
        }
    }
}

void Game::handleNewGame() {
    ConsoleUI::clearScreen();
    ASCIIArt::printTitleLogo();
    ConsoleUI::printHeader("KHỞI TẠO ANH HÙNG MỚI", 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    std::string name = ConsoleUI::getStringInput("  Nhập tên anh hùng của bạn [mặc định: Arthur]: ");
    if (name.empty()) name = "Arthur";

    std::cout << "\n  " << ConsoleUI::colorize("Chọn Lớp Nhân Vật:", ConsoleUI::Colors::BRIGHT_CYAN) << "\n"
              << "  1. " << ConsoleUI::colorize("Chiến Binh (Warrior)", ConsoleUI::Colors::BRIGHT_RED) 
              << " - Máu & Giáp cao, càn quét cận chiến (Power Slash, Shield Block, Berserk)\n"
              << "  2. " << ConsoleUI::colorize("Pháp Sư (Mage)", ConsoleUI::Colors::BRIGHT_BLUE) 
              << "       - Mana dồi dào, sát thương phép diện rộng (Fireball, Ice Blast, Meteor)\n"
              << "  3. " << ConsoleUI::colorize("Xạ Thủ (Ranger)", ConsoleUI::Colors::BRIGHT_GREEN) 
              << "      - Nhanh nhẹn, bạo kích cao (Double Shot, Poison Arrow, Rain of Arrows)\n\n";

    int classChoice = ConsoleUI::getIntInput(1, 3, "  Chọn lớp nhân vật [1-3]: ");

    inventory.clear();

    if (classChoice == 1) {
        hero = std::make_unique<Warrior>(name);
        ASCIIArt::printWarriorArt();
        inventory.addItem(ItemFactory::createFromJson("wpn_iron_sword"));
        inventory.addItem(ItemFactory::createFromJson("arm_iron_plate"));
        inventory.addItem(ItemFactory::createFromJson("pot_01"));
        inventory.addItem(ItemFactory::createFromJson("pot_01"));
        inventory.equipWeapon(0);
        inventory.equipArmor(1);
    } else if (classChoice == 2) {
        hero = std::make_unique<Mage>(name);
        inventory.addItem(ItemFactory::createFromJson("wpn_magic_staff"));
        inventory.addItem(ItemFactory::createFromJson("arm_magic_robe"));
        inventory.addItem(ItemFactory::createFromJson("pot_01"));
        inventory.addItem(ItemFactory::createFromJson("pot_02"));
        inventory.addItem(ItemFactory::createFromJson("pot_02"));
        inventory.equipWeapon(0);
        inventory.equipArmor(1);
    } else {
        hero = std::make_unique<Ranger>(name);
        inventory.addItem(ItemFactory::createFromJson("wpn_long_bow"));
        inventory.addItem(ItemFactory::createFromJson("arm_leather_vest"));
        inventory.addItem(ItemFactory::createFromJson("pot_01"));
        inventory.addItem(ItemFactory::createFromJson("pot_01"));
        inventory.addItem(ItemFactory::createFromJson("pot_02"));
        inventory.equipWeapon(0);
        inventory.equipArmor(1);
    }

    hero->addGold(50);
    story.loadStoryGraph("data/story.json");

    ConsoleUI::printSuccess("Khởi tạo Anh Hùng " + hero->getName() + " thành công!");
    hero->displayStats();
    ConsoleUI::pause("Nhấn Enter để bước vào hành trình...");

    handleStoryLoop();
}

void Game::handleLoadGame() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("TẢI BẢN LƯU GAME", 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    auto slots = saveManager.getExistingSlots();
    int choice = mainMenu.showLoadGameMenu(slots);
    if (choice < 0) return;

    // Default placeholder hero
    hero = std::make_unique<Warrior>("LoadedHero");
    if (saveManager.loadGame(choice, *hero, story, &inventory)) {
        // Upgrade pointer to exact class if Mage/Ranger
        if (hero->getHeroClass() == HeroClass::MAGE) {
            auto m = std::make_unique<Mage>(hero->getName());
            m->setLevel(hero->getLevel());
            m->setExp(hero->getExp());
            m->setMaxHp(hero->getMaxHp());
            m->setMaxMp(hero->getMaxMp());
            m->setHp(hero->getHp());
            m->setMp(hero->getMp());
            m->setAttack(hero->getAttack());
            m->setDefense(hero->getDefense());
            m->setGold(hero->getGold());
            hero = std::move(m);
        } else if (hero->getHeroClass() == HeroClass::RANGER) {
            auto r = std::make_unique<Ranger>(hero->getName());
            r->setLevel(hero->getLevel());
            r->setExp(hero->getExp());
            r->setMaxHp(hero->getMaxHp());
            r->setMaxMp(hero->getMaxMp());
            r->setHp(hero->getHp());
            r->setMp(hero->getMp());
            r->setAttack(hero->getAttack());
            r->setDefense(hero->getDefense());
            r->setGold(hero->getGold());
            hero = std::move(r);
        }

        ConsoleUI::printSuccess("Tải thành công bản lưu Slot " + std::to_string(choice) + "!");
        hero->displayStats();
        ConsoleUI::pause("Nhấn Enter để tiếp tục hành trình...");
        handleStoryLoop();
    } else {
        ConsoleUI::printError("Không thể tải bản lưu từ Slot " + std::to_string(choice) + "!");
        ConsoleUI::pause();
    }
}

void Game::handleSaveGame() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("LƯU TIẾN TRÌNH GAME", 68, ConsoleUI::Colors::BRIGHT_GREEN);

    std::cout << "\n  Chọn vị trí lưu từ 1 đến 3 (hoặc 0 để quay lại):\n";
    int slot = ConsoleUI::getIntInput(0, 3, "  Chọn Slot [1-3]: ");
    if (slot == 0) return;

    if (hero && saveManager.saveGame(slot, *hero, story, &inventory)) {
        ConsoleUI::printSuccess("Đã lưu game thành công vào Slot " + std::to_string(slot) + "!");
    } else {
        ConsoleUI::printError("Lỗi khi lưu game!");
    }
    ConsoleUI::pause();
}

void Game::handleInventoryMenu() {
    while (true) {
        inventoryUI.renderInventory(inventory);
        int itemIndex = inventoryUI.selectItemIndex(inventory);
        if (itemIndex < 0) break;

        Item chosen = inventory.getItem(itemIndex);
        int action = inventoryUI.selectItemAction(chosen);

        if (action == 1) { // Use or Equip
            if (chosen.getType() == ItemType::POTION) {
                if (inventory.useItem(itemIndex, *hero)) {
                    ConsoleUI::printSuccess("Đã sử dụng " + chosen.getName() + "!");
                }
            } else if (chosen.getType() == ItemType::WEAPON) {
                if (inventory.equipWeapon(itemIndex)) {
                    ConsoleUI::printSuccess("Đã trang bị " + chosen.getName() + "!");
                }
            } else if (chosen.getType() == ItemType::ARMOR) {
                if (inventory.equipArmor(itemIndex)) {
                    ConsoleUI::printSuccess("Đã mặc giáp " + chosen.getName() + "!");
                }
            }
        } else if (action == 2) { // Discard
            inventory.removeItem(itemIndex);
            ConsoleUI::printWarning("Đã vứt bỏ " + chosen.getName() + " khỏi túi đồ!");
        }
        ConsoleUI::pause();
    }
}

void Game::handleStoryLoop() {
    while (hero && hero->isAlive()) {
        StoryNode current = story.getCurrentNode();

        // 1. Process special event nodes
        if (current.type == EventType::ITEM) {
            if (!current.rewardItemId.empty()) {
                Item item = ItemFactory::createFromJson(current.rewardItemId);
                inventory.addItem(item);
                ConsoleUI::printSuccess("Bạn phát hiện được vật phẩm: " + item.getName() + " (" + item.getDescription() + ")!");
            }
            if (current.rewardGold > 0) {
                hero->addGold(current.rewardGold);
                ConsoleUI::printSuccess("Bạn thu thập thêm được: +" + std::to_string(current.rewardGold) + " Vàng!");
            }
        } else if (current.type == EventType::BATTLE) {
            std::string enemyId = current.enemyId.empty() ? "goblin" : current.enemyId;
            auto minion = MinionFactory::createFromJson(enemyId);
            if (!minion) {
                minion = std::make_shared<Goblin>();
            }
            bool victory = executeCombat(minion);
            if (!victory) return; // Hero defeated -> return to main menu
        } else if (current.type == EventType::BOSS) {
            std::string bossId = current.enemyId.empty() ? "dragon_lord" : current.enemyId;
            auto boss = BossFactory::createFromJson(bossId);
            if (!boss) {
                boss = std::make_shared<BossMonster>();
            }
            bool victory = executeCombat(boss);
            if (!victory) return; // Defeated by Boss -> return to main menu
        } else if (current.type == EventType::ENDING) {
            ConsoleUI::clearScreen();
            ASCIIArt::printVictoryBanner();
            ConsoleUI::printHeader("KẾT THÚC CỐT TRUYỆN", 68, ConsoleUI::Colors::BRIGHT_MAGENTA);

            std::vector<std::string> endingLines = {
                current.text,
                "",
                "Anh Hùng: " + hero->getName() + " | Cấp: " + std::to_string(hero->getLevel()) + " | Vàng: " + std::to_string(hero->getGold()) + "G",
                "Máu cuối: " + std::to_string(hero->getHp()) + "/" + std::to_string(hero->getMaxHp())
            };
            ConsoleUI::printBox(endingLines, 68, ConsoleUI::Colors::BRIGHT_CYAN);
            ConsoleUI::pause("Nhấn Enter để quay về Menu chính...");
            return;
        }

        // 2. Render Current Scene & Story Box
        ConsoleUI::clearScreen();
        std::string title = "CỐT TRUYỆN - [Vị trí: " + current.id + "]";
        ConsoleUI::printHeader(title, 68, ConsoleUI::Colors::BRIGHT_CYAN);

        std::vector<std::string> sceneBox = {
            current.text,
            "",
            "Anh Hùng: " + hero->getName() + " | HP: " + std::to_string(hero->getHp()) + "/" + std::to_string(hero->getMaxHp()) + 
            " | MP: " + std::to_string(hero->getMp()) + "/" + std::to_string(hero->getMaxMp()) +
            " | Vàng: " + std::to_string(hero->getGold()) + "G"
        };
        ConsoleUI::printBox(sceneBox, 68, ConsoleUI::Colors::BRIGHT_WHITE);

        // 3. Display Choices
        std::cout << "\n  " << ConsoleUI::colorize("Lựa chọn hành động:", ConsoleUI::Colors::BRIGHT_YELLOW) << "\n";
        int choiceCount = static_cast<int>(current.choices.size());
        for (int i = 0; i < choiceCount; ++i) {
            std::cout << "  [" << ConsoleUI::colorize(std::to_string(i + 1), ConsoleUI::Colors::BRIGHT_CYAN) 
                      << "] " << current.choices[i].text << "\n";
        }

        std::cout << "\n  " << ConsoleUI::colorize("Lựa chọn hệ thống:", ConsoleUI::Colors::DIM) << "\n"
                  << "  [" << ConsoleUI::colorize("I", ConsoleUI::Colors::BRIGHT_MAGENTA) << "] Mở Túi Đồ (Inventory)\n"
                  << "  [" << ConsoleUI::colorize("S", ConsoleUI::Colors::BRIGHT_GREEN) << "] Lưu game (Save Slot)\n"
                  << "  [" << ConsoleUI::colorize("Q", ConsoleUI::Colors::BRIGHT_RED) << "] Thoát về Menu Chính\n\n";

        std::string input = ConsoleUI::getStringInput("  Nhập lựa chọn của bạn: ");
        if (input == "I" || input == "i") {
            handleInventoryMenu();
        } else if (input == "S" || input == "s") {
            handleSaveGame();
        } else if (input == "Q" || input == "q") {
            return;
        } else {
            try {
                int c = std::stoi(input);
                if (c >= 1 && c <= choiceCount) {
                    story.selectChoice(c - 1);
                } else {
                    ConsoleUI::printWarning("Lựa chọn không hợp lệ!");
                    ConsoleUI::pause();
                }
            } catch (...) {
                ConsoleUI::printWarning("Vui lòng nhập số thứ tự hoặc phím tắt (I, S, Q)!");
                ConsoleUI::pause();
            }
        }
    }
}

bool Game::executeCombat(std::shared_ptr<Enemy> enemy) {
    CombatEngine combat(*hero, *enemy, &inventory);
    combat.startBattle();

    std::string lastMessage = "Một " + enemy->getName() + " xuất hiện chắn đường! Chuẩn bị giao tranh!";

    while (!combat.isBattleOver()) {
        battleUI.renderBattleScreen(*hero, *enemy, lastMessage);
        BattleAction action = battleUI.getPlayerAction();

        if (action == BattleAction::ATTACK) {
            combat.executeTurn(1);
        } else if (action == BattleAction::SKILL) {
            std::cout << "\n";
            hero->displaySkills();
            int skillIdx = ConsoleUI::getIntInput(1, 3, "  Chọn kỹ năng [1-3] (hoặc 0 để hủy): ");
            if (skillIdx == 0) {
                lastMessage = "Đã hủy dùng kỹ năng.";
                continue;
            }
            combat.executeTurn(2, skillIdx);
        } else if (action == BattleAction::ITEM) {
            inventoryUI.renderInventory(inventory);
            int itemIdx = inventoryUI.selectItemIndex(inventory);
            if (itemIdx < 0) {
                lastMessage = "Đã hủy sử dụng vật phẩm.";
                continue;
            }
            combat.executeTurn(3, itemIdx);
        } else if (action == BattleAction::DEFEND) {
            combat.executeTurn(4);
        } else if (action == BattleAction::RUN) {
            combat.executeTurn(5);
        }

        // Aggregate turn logs for BattleUI HUD
        std::stringstream ss;
        for (const auto& log : combat.getTurnLogs()) {
            if (log.find("--- Turn") == std::string::npos && log.find("===") == std::string::npos) {
                ss << log << " ";
            }
        }
        lastMessage = ss.str();
    }

    if (combat.getState() == CombatState::HERO_VICTORY) {
        battleUI.showVictory(*enemy);
        hero->addGold(enemy->getGoldReward());
        return true;
    } else if (combat.getState() == CombatState::FLED) {
        ConsoleUI::printInfo("Bạn đã trốn thoát an toàn!");
        ConsoleUI::pause();
        return true;
    } else {
        battleUI.showDefeat();
        return false;
    }
}
