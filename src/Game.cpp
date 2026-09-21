/**
 * @file Game.cpp
 * @brief Implement Game coordinator class methods for complete interactive gameplay loop.
 * @author Phong & Team
 */

#include "Game.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

// Helper tải vật phẩm từ data/items.json theo ID
static std::shared_ptr<Item> loadItemById(const std::string& itemId, const std::string& filepath = "data/items.json") {
    try {
        std::ifstream f(filepath);
        if (f.is_open()) {
            nlohmann::json j;
            f >> j;
            if (j.contains("items") && j["items"].is_array()) {
                for (const auto& itemJ : j["items"]) {
                    if (itemJ.value("id", "") == itemId) {
                        std::string id = itemJ.value("id", "");
                        std::string name = itemJ.value("name", "");
                        std::string desc = itemJ.value("description", "");
                        std::string type = itemJ.value("type", "POTION");
                        int statVal = itemJ.value("statValue", 0);
                        if (type == "WEAPON") {
                            int atk = statVal > 0 ? statVal : itemJ.value("attackBonus", 10);
                            return std::make_shared<Weapon>(id, name, desc, atk);
                        } else if (type == "ARMOR") {
                            int def = statVal > 0 ? statVal : itemJ.value("defenseBonus", 5);
                            return std::make_shared<Armor>(id, name, desc, def);
                        } else if (type == "POTION") {
                            bool isMana = itemJ.value("isMana", false);
                            int heal = itemJ.value("healAmount", statVal > 0 ? statVal : 35);
                            return std::make_shared<Potion>(id, name, desc, heal, isMana, 1);
                        }
                    }
                }
            }
        }
    } catch (...) {}

    // Fallbacks
    if (itemId == "IceKey") {
        return std::make_shared<Armor>("IceKey", "Chìa Khóa Băng Bất Hoại", "Cổ vật chạm khắc hoa tuyết, dùng mở phong ấn bí mật.", 5);
    } else if (itemId == "GreaterHealthPotion") {
        return std::make_shared<Potion>("GreaterHealthPotion", "Bình Thuốc Máu Lớn", "Hồi phục tức thì 75 HP.", 75, false, 1);
    }
    return std::make_shared<Potion>(itemId, itemId, "Vật phẩm phục hồi", 35, false, 1);
}

Game::Game(const std::string& storyFile, const std::string& saveDir)
    : player(nullptr), saveManager(saveDir), isRunning(false) {
    story.loadStoryGraph(storyFile);
}

void Game::run() {
    bool inApp = true;
    while (inApp) {
        MainMenuOption opt = mainMenu.showMenu();
        switch (opt) {
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
                ConsoleUI::printInfo("Cảm ơn bạn đã trải nghiệm RPG Engine! Tạm biệt!");
                inApp = false;
                break;
        }
    }
}

void Game::startNewGame() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("KHỞI TẠO NHÂN VẬT (CHARACTER CREATION)", 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    std::cout << "\n  Nhập tên dũng sĩ của bạn: ";
    std::string heroName = ConsoleUI::getStringInput();
    if (heroName.empty()) heroName = "Arthur";

    std::cout << "\n  " << ConsoleUI::colorize("Chọn lớp nhân vật (Class):", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    std::cout << "  [1] Chiến Binh (Warrior) - Máu trâu, phòng thủ vững vàng\n";
    std::cout << "  [2] Pháp Sư   (Mage)    - Mana dồi dào, phép hủy diệt\n";
    std::cout << "  [3] Xạ Thủ    (Ranger)  - Nhanh nhẹn, bạo kích cao\n\n";

    int classChoice = ConsoleUI::getIntInput(1, 3, "Nhập lựa chọn của bạn [1-3]: ");
    if (classChoice == 1) {
        player = std::make_shared<Warrior>(heroName);
        ASCIIArt::printWarriorArt();
    } else if (classChoice == 2) {
        player = std::make_shared<Mage>(heroName);
        ASCIIArt::printMageArt();
    } else {
        player = std::make_shared<Ranger>(heroName);
        ASCIIArt::printRangerArt();
    }

    // Trang bị vật phẩm khởi đầu
    auto startSword = loadItemById("wpn_01");
    auto startArmor = loadItemById("arm_03");
    auto startPotion1 = loadItemById("pot_01");
    auto startPotion2 = loadItemById("pot_01");

    player->getInventory().addItem(startSword);
    player->getInventory().addItem(startArmor);
    player->getInventory().addItem(startPotion1);
    player->getInventory().addItem(startPotion2);

    player->getInventory().equipWeapon(0);
    player->getInventory().equipArmor(1);

    ConsoleUI::printSuccess("Khởi tạo nhân vật '" + player->getName() + "' thành công với trang bị khởi đầu!");
    ConsoleUI::pause();

    // Reset cốt truyện về node bắt đầu
    story.moveToNode("node_01");
    isRunning = true;
    runStoryLoop();
}

void Game::loadGame() {
    auto slots = saveManager.getExistingSlots();
    int slot = mainMenu.showLoadGameMenu(slots);
    if (slot <= 0) return;

    // Peek heroClass từ file JSON để tạo đúng class Hero
    HeroClass savedClass = HeroClass::WARRIOR;
    {
        std::string slotPath = saveManager.getSaveDirectory() + "slot" + std::to_string(slot) + ".json";
        std::ifstream f(slotPath);
        if (f.is_open()) {
            try {
                nlohmann::json j;
                f >> j;
                if (j.contains("heroClass")) {
                    savedClass = static_cast<HeroClass>(j["heroClass"].get<int>());
                }
            } catch (...) {}
        }
    }

    // Tạo đúng class Hero dựa trên savedClass
    if (savedClass == HeroClass::MAGE) {
        player = std::make_shared<Mage>("Hero");
    } else if (savedClass == HeroClass::RANGER) {
        player = std::make_shared<Ranger>("Hero");
    } else {
        player = std::make_shared<Warrior>("Hero");
    }

    if (saveManager.loadGame(slot, *player, story)) {
        ConsoleUI::printSuccess("Tải tiến trình Slot " + std::to_string(slot) + " thành công!");
        ConsoleUI::pause();
        isRunning = true;
        runStoryLoop();
    } else {
        ConsoleUI::printError("Không thể tải bản lưu Slot " + std::to_string(slot) + "!");
        ConsoleUI::pause();
    }
}

void Game::runStoryLoop() {
    while (isRunning && player && player->isAlive()) {
        StoryNode currentNode = story.getCurrentNode();
        std::string type = currentNode.rawType;

        if (type == "COMBAT" || type == "BATTLE" || currentNode.type == EventType::BATTLE) {
            handleCombatNode(currentNode);
        } else if (type == "REWARD" || type == "ITEM" || currentNode.type == EventType::ITEM) {
            handleRewardNode(currentNode);
        } else if (type == "REQUIREMENT_CHECK") {
            handleRequirementCheckNode(currentNode);
        } else if (type == "SHOP" || currentNode.type == EventType::SHOP) {
            handleShopNode(currentNode);
        } else if (type == "GAME_OVER" || type == "VICTORY" || type == "ENDING" || currentNode.type == EventType::ENDING) {
            handleEndingNode(currentNode);
            break;
        } else {
            handleStoryNode(currentNode);
        }
    }
}

void Game::handleStoryNode(const StoryNode& node) {
    ConsoleUI::clearScreen();
    std::string title = node.title.empty() ? "KHÁM PHÁ CỐT TRUYỆN" : node.title;
    ConsoleUI::printHeader(title, 68, ConsoleUI::Colors::BRIGHT_MAGENTA);

    std::vector<std::string> descLines = { node.text };
    ConsoleUI::printBox(descLines, 68, ConsoleUI::Colors::BRIGHT_CYAN);

    std::cout << "\n  " << ConsoleUI::colorize("Các lựa chọn của bạn:", ConsoleUI::Colors::BRIGHT_YELLOW) << "\n";
    int choiceCount = static_cast<int>(node.choices.size());
    for (int i = 0; i < choiceCount; ++i) {
        std::cout << "  [" << ConsoleUI::colorize(std::to_string(i + 1), ConsoleUI::Colors::BRIGHT_GREEN) << "] "
                  << node.choices[i].text << "\n";
    }

    // Tùy chọn In-Game Menu
    int menuOptionNumber = choiceCount + 1;
    std::cout << "  [" << ConsoleUI::colorize(std::to_string(menuOptionNumber), ConsoleUI::Colors::BRIGHT_BLUE) << "] "
              << ConsoleUI::colorize("Mở Menu (Túi đồ, Chỉ số, Lưu game)", ConsoleUI::Colors::BRIGHT_BLUE) << "\n\n";

    ConsoleUI::printDivider('-', 68, ConsoleUI::Colors::DIM);

    int choice = ConsoleUI::getIntInput(1, menuOptionNumber, "Nhập lựa chọn của bạn: ");
    if (choice == menuOptionNumber) {
        openInGameMenu();
        return;
    }

    story.selectChoice(choice - 1);
}

void Game::handleCombatNode(const StoryNode& node) {
    auto enemy = createEnemyById(node.enemyId);
    if (!enemy) {
        story.moveToNode(node.onWinNodeId.empty() ? node.nextNodeId : node.onWinNodeId);
        return;
    }

    CombatEngine combat(*player, *enemy);
    combat.startBattle();

    std::string battleMsg = "Một " + enemy->getName() + " xuất hiện!";

    while (!combat.isBattleOver()) {
        battleUI.renderBattleScreen(*player, *enemy, battleMsg);
        BattleAction act = battleUI.getPlayerAction();

        if (act == BattleAction::ATTACK) {
            combat.executeTurn(1);
            battleMsg = player->getName() + " tấn công dũng mãnh!";
        } else if (act == BattleAction::SKILL) {
            std::cout << "\n";
            player->displaySkills();
            int skillIdx = ConsoleUI::getIntInput(1, 3, "Chọn kỹ năng [1-3]: "); // Skill index: 1-3 (không trừ 1)
            combat.executeTurn(2, skillIdx);
            battleMsg = player->getName() + " thi triển kỹ năng " + player->getSkillName(skillIdx) + "!";
        } else if (act == BattleAction::ITEM) {
            combat.executeTurn(3);
            battleMsg = player->getName() + " sử dụng vật phẩm hồi phục!";
        } else if (act == BattleAction::DEFEND) {
            combat.executeTurn(4);
            battleMsg = player->getName() + " nâng cao thế phòng thủ (giảm 50% sát thương)!";
        } else if (act == BattleAction::RUN) {
            combat.executeTurn(5);
            battleMsg = player->getName() + " tháo chạy khỏi trận chiến!";
            break;
        }
    }

    if (combat.getState() == CombatState::HERO_VICTORY) {
        battleUI.showVictory(*enemy);
        std::string nextId = node.onWinNodeId.empty() ? node.nextNodeId : node.onWinNodeId;
        story.moveToNode(nextId);
    } else if (combat.getState() == CombatState::ENEMY_VICTORY) {
        battleUI.showDefeat();
        std::string nextId = node.onLoseNodeId.empty() ? "node_06" : node.onLoseNodeId;
        story.moveToNode(nextId);
    } else {
        // Fled
        std::string nextId = node.onWinNodeId.empty() ? node.nextNodeId : node.onWinNodeId;
        story.moveToNode(nextId);
    }
}

void Game::handleRewardNode(const StoryNode& node) {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader(node.title.empty() ? "PHẦN THƯỞNG CỐT TRUYỆN" : node.title, 68, ConsoleUI::Colors::BRIGHT_GREEN);

    std::vector<std::string> lines = { node.text };
    ConsoleUI::printBox(lines, 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    if (node.rewardExp > 0) {
        player->addExp(node.rewardExp);
        std::cout << "  " << ConsoleUI::colorize("+ Nhận được: ", ConsoleUI::Colors::BRIGHT_YELLOW)
                  << ConsoleUI::colorize(std::to_string(node.rewardExp) + " EXP", ConsoleUI::Colors::BRIGHT_GREEN) << "\n";
    }

    for (const auto& itemId : node.rewardItems) {
        auto item = loadItemById(itemId);
        if (item) {
            player->getInventory().addItem(item);
            std::cout << "  " << ConsoleUI::colorize("+ Nhận được vật phẩm: ", ConsoleUI::Colors::BRIGHT_YELLOW)
                      << ConsoleUI::colorize(item->getName(), ConsoleUI::Colors::BRIGHT_WHITE) << "\n";
        }
    }

    ConsoleUI::pause();
    story.moveToNode(node.nextNodeId);
}

void Game::handleRequirementCheckNode(const StoryNode& node) {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader(node.title.empty() ? "KIỂM TRA ĐIỀU KIỆN" : node.title, 68, ConsoleUI::Colors::BRIGHT_CYAN);

    std::vector<std::string> lines = { node.text };
    ConsoleUI::printBox(lines, 68, ConsoleUI::Colors::BRIGHT_CYAN);

    bool hasItem = player->getInventory().hasItem(node.requiredItem);
    if (hasItem) {
        ConsoleUI::printSuccess("Bạn sở hữu '" + node.requiredItem + "'! Lối đi bí mật đã được mở!");
        ConsoleUI::pause();
        story.moveToNode(node.onPassNodeId);
    } else {
        ConsoleUI::printWarning("Bạn không có '" + node.requiredItem + "'! Không thể mở phong ấn.");
        ConsoleUI::pause();
        story.moveToNode(node.onFailNodeId);
    }
}

void Game::handleShopNode(const StoryNode& node) {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("CỬA HÀNG / THƯƠNG QUÁN", 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    std::vector<std::string> warningBox = {
        "[ CẢNH BÁO / WARNING ]",
        "",
        "  >>> Tính năng đang được phát triển <<<",
        "",
        "Hệ thống mua sắm & trao đổi với Thương Gia sẽ có mặt trong bản cập nhật kế tiếp.",
        "Mời bạn tiếp tục cuộc phiêu lưu!"
    };
    ConsoleUI::printBox(warningBox, 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    ConsoleUI::pause("Nhấn Enter để tiếp tục hành trình...");
    if (!node.nextNodeId.empty()) {
        story.moveToNode(node.nextNodeId);
    } else if (!node.choices.empty()) {
        story.moveToNode(node.choices[0].nextNodeId);
    } else {
        story.moveToNode("node_01");
    }
}

void Game::handleEndingNode(const StoryNode& node) {
    ConsoleUI::clearScreen();
    if (node.rawType == "VICTORY") {
        ASCIIArt::printVictoryBanner();
        ConsoleUI::printHeader(node.title, 68, ConsoleUI::Colors::BRIGHT_GREEN);
    } else {
        ASCIIArt::printGameOverBanner();
        ConsoleUI::printHeader(node.title, 68, ConsoleUI::Colors::BRIGHT_RED);
    }

    std::vector<std::string> lines = { node.text };
    ConsoleUI::printBox(lines, 68, (node.rawType == "VICTORY" ? ConsoleUI::Colors::BRIGHT_GREEN : ConsoleUI::Colors::BRIGHT_RED));

    ConsoleUI::pause("Nhấn Enter để quay về Menu chính...");
    isRunning = false;
}

void Game::openInGameMenu() {
    bool inMenu = true;
    while (inMenu) {
        ConsoleUI::clearScreen();
        ConsoleUI::printHeader("IN-GAME MENU", 50, ConsoleUI::Colors::BRIGHT_CYAN);
        std::cout << "  [1] Xem Túi Đồ & Trang Bị\n";
        std::cout << "  [2] Xem Chỉ Số Nhân Vật\n";
        std::cout << "  [3] Lưu Game (Save Game)\n";
        std::cout << "  [4] Tiếp Tục Chơi\n\n";

        int choice = ConsoleUI::getIntInput(1, 4, "Chọn [1-4]: ");
        if (choice == 1) {
            openInventoryScreen();
        } else if (choice == 2) {
            ConsoleUI::clearScreen();
            player->displayStats();
            ConsoleUI::pause();
        } else if (choice == 3) {
            saveCurrentGame();
        } else {
            inMenu = false;
        }
    }
}

void Game::openInventoryScreen() {
    bool inInv = true;
    while (inInv) {
        inventoryUI.renderInventory(player->getInventory());
        std::cout << "  [1] Sử dụng / Trang bị vật phẩm\n";
        std::cout << "  [0] Quay lại\n\n";

        int opt = ConsoleUI::getIntInput(0, 1, "Chọn: ");
        if (opt == 0) {
            inInv = false;
        } else {
            int idx = inventoryUI.selectItemIndex(player->getInventory());
            if (idx >= 0) {
                auto item = player->getInventory().getItemPtr(idx);
                if (item) {
                    int action = inventoryUI.selectItemAction(*item);
                    if (action == 1) { // Use
                        player->getInventory().useItem(idx, *player);
                    } else if (action == 2) { // Equip
                        if (item->getType() == ItemType::WEAPON) player->getInventory().equipWeapon(idx);
                        else if (item->getType() == ItemType::ARMOR) player->getInventory().equipArmor(idx);
                    } else if (action == 3) { // Discard
                        player->getInventory().removeItem(idx);
                    }
                }
            }
        }
    }
}

void Game::saveCurrentGame() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("LƯU GAME (SAVE GAME)", 50, ConsoleUI::Colors::BRIGHT_YELLOW);
    std::cout << "  Nhập số slot muốn lưu [1-9, hoặc 0 để hủy]: ";
    int slot = ConsoleUI::getIntInput(0, 9);
    if (slot == 0) return;

    if (saveManager.saveGame(slot, *player, story)) {
        ConsoleUI::printSuccess("Đã lưu game vào Slot " + std::to_string(slot) + " thành công!");
    } else {
        ConsoleUI::printError("Lỗi khi lưu game vào Slot " + std::to_string(slot) + "!");
    }
    ConsoleUI::pause();
}

std::shared_ptr<Enemy> Game::createEnemyById(const std::string& enemyId) {
    if (enemyId.rfind("boss", 0) == 0 || enemyId == "dragon_lord") {
        auto boss = BossFactory::createFromJson(enemyId, "data/enemies.json");
        if (boss) {
            boss->addDropItem(loadItemById("GreaterHealthPotion"), 1.0);
            return boss;
        }
    }

    auto minion = MinionFactory::createFromJson(enemyId, "data/enemies.json");
    if (minion) {
        minion->addDropItem(loadItemById("pot_01"), 0.7);
        minion->addDropItem(loadItemById("arm_03"), 0.4);
        return minion;
    }

    // Fallback nếu không khớp ID
    auto fallbackMinion = std::make_shared<Goblin>();
    fallbackMinion->addDropItem(loadItemById("pot_01"), 0.5);
    return fallbackMinion;
}
