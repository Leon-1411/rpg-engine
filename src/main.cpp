/**
 * @file main.cpp
 * @brief Entry point for RPG Engine, combining all system modules with ANSI Console UI.
 * @author Phong & Team
 */

#include "Hero.h"
#include "Enemy.h"
#include "Minion.h"
#include "BossMonster.h"
#include "Item.h"
#include "Inventory.h"
#include "CombatEngine.h"
#include "StoryGraph.h"
#include "SaveManager.h"
#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include "ui/BattleUI.h"
#include "ui/InventoryUI.h"
#include "ui/MainMenu.h"
#include <iostream>

int main() {
    // 0. Initialize Terminal (Enable ANSI Escape Sequences & UTF-8 on Windows / Linux)
    ConsoleUI::initConsole();
    ConsoleUI::clearScreen();

    // 1. Display Title Logo & Art
    ASCIIArt::printTitleLogo();
    ConsoleUI::printHeader("RPG ENGINE INITIALIZATION & DEMO", 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    // 2. Initialize Hero (Quý) & Display with ANSI Color Bars
    std::cout << "\n" << ConsoleUI::colorize("=== [1] KHỞI TẠO HERO ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    Hero player("Arthur", HeroClass::WARRIOR, 100, 30, 20, 5);
    ASCIIArt::printWarriorArt();
    std::cout << "  Tên: " << ConsoleUI::colorize(player.getName(), ConsoleUI::Colors::BRIGHT_WHITE)
              << " | Lớp: " << ConsoleUI::colorize("Chiến Binh (Warrior)", ConsoleUI::Colors::BRIGHT_BLUE)
              << " | Cấp độ: " << ConsoleUI::colorize(std::to_string(player.getLevel()), ConsoleUI::Colors::BRIGHT_YELLOW) << "\n";
    ConsoleUI::printProgressBar("  Máu (HP)", player.getHp(), player.getMaxHp(), 20);
    ConsoleUI::printProgressBar("  Mana(MP)", player.getMp(), player.getMaxMp(), 20, ConsoleUI::Colors::BRIGHT_BLUE);
    ConsoleUI::printSuccess("Khởi tạo Hero thành công!");

    // 3. Initialize Inventory & Items (Sang) & Display with InventoryUI
    std::cout << "\n" << ConsoleUI::colorize("=== [2] TÚI ĐỒ & TRANG BỊ ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    Inventory inv;
    Item potion("pot_01", "Health Potion", "Hồi phục 30 HP", ItemType::POTION, 30);
    Item sword("wpn_01", "Thanh Kiếm Sắt", "Lưỡi kiếm sắc bén tăng 10 ATK", ItemType::WEAPON, 10);
    Item armor("arm_01", "Giáp Thép Hiệp Sĩ", "Áo giáp bảo vệ tăng 8 DEF", ItemType::ARMOR, 8);
    inv.addItem(potion);
    inv.addItem(sword);
    inv.addItem(armor);
    inv.equipWeapon(1);
    inv.equipArmor(2);

    InventoryUI invUI;
    invUI.renderInventory(inv);
    ConsoleUI::printSuccess("Khởi tạo Túi đồ và trang bị thành công!");

    // 4. Initialize Story Graph (Nghĩa)
    std::cout << "\n" << ConsoleUI::colorize("=== [3] CỐT TRUYỆN (STORY GRAPH) ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    StoryGraph story;
    StoryNode current = story.getCurrentNode();
    std::vector<std::string> storyBox = {
        "Node ID: " + current.id,
        current.text
    };
    ConsoleUI::printBox(storyBox, 68, ConsoleUI::Colors::BRIGHT_MAGENTA);
    ConsoleUI::printSuccess("Cốt truyện đã sẵn sàng!");

    // 5. Initialize Enemy & Combat Engine from JSON (Nhật & Lợi) with BattleUI
    std::cout << "\n" << ConsoleUI::colorize("=== [4] HỆ THỐNG GIAO TRANH (BATTLE HUD) ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    std::cout << "[Loading Minions from data/enemies.json]\n";
    auto loadedMinions = MinionFactory::loadAllFromJson("data/enemies.json");
    for (const auto& m : loadedMinions) {
        std::cout << " - Loaded: " << m->getName() << " [" << minionTypeToString(m->getMinionType())
                  << "] (HP: " << m->getHp() << ", ATK: " << m->getAttack()
                  << ", DEF: " << m->getDefense() << ")\n";
    }

    auto minion = MinionFactory::createFromJson("goblin", "data/enemies.json");
    if (!minion) {
        minion = std::make_shared<Goblin>();
    }
    std::cout << "\nEncountered an enemy:\n";
    minion->displayStats();

    BattleUI battleUI;
    battleUI.renderBattleScreen(player, *minion, "Một tên Goblin Scout bất ngờ xuất hiện chắn đường!");

    CombatEngine combat(player, *minion);
    combat.startBattle();
    
    battleUI.printCombatLog("Arthur vung kiếm tấn công Goblin Scout!");
    combat.executeTurn(1);
    if (!combat.isBattleOver()) {
        battleUI.printCombatLog("Arthur tiếp tục dồn đòn kết liễu!");
        combat.executeTurn(1);
    }
    battleUI.showVictory(*minion);

    // 6. Save Manager Test with nlohmann/json (Phong)
    std::cout << "\n" << ConsoleUI::colorize("=== [5] LƯU DỮ LIỆU GAME (SAVE / LOAD JSON) ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    SaveManager saveMgr("saves/");
    bool saved = saveMgr.saveGame(1, player, story);
    if (saved) {
        ConsoleUI::printSuccess("Đã lưu tiến trình vào file JSON: saves/slot1.json (chuẩn nlohmann/json)");
    } else {
        ConsoleUI::printError("Lỗi khi lưu game!");
    }

    // Verify loading back from JSON
    Hero loadedHero("Blank", HeroClass::WARRIOR, 1, 1, 1, 1);
    StoryGraph loadedStory;
    if (saveMgr.loadGame(1, loadedHero, loadedStory)) {
        ConsoleUI::printSuccess("Đã tải lại thành công dữ liệu từ file JSON! Hero: " + loadedHero.getName() + " (HP: " + std::to_string(loadedHero.getHp()) + ")");
    }

    // 7. Boss Monster & Enrage Mechanic (Nhật & Lợi)
    std::cout << "\n" << ConsoleUI::colorize("=== [6] BOSS MONSTER & ENRAGE MECHANIC ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    std::cout << "[Loading Boss from data/enemies.json]\n";
    auto boss = BossFactory::createFromJson("dragon_lord", "data/enemies.json");
    if (!boss) {
        boss = std::make_shared<BossMonster>();
    }
    boss->displayStats();

    // Create high-level hero to battle the boss
    Hero raidBossHero("Grand Paladin", HeroClass::WARRIOR, 600, 150, 150, 30);
    CombatEngine bossCombat(raidBossHero, *boss);
    bossCombat.startBattle();

    // Turn 1
    bossCombat.executeTurn(1);

    // Simulate boss HP dropping below 30% to demonstrate Enrage & slight healing
    std::cout << "\n>>> Inflicting heavy blow to boss to test < 30% HP Enrage mechanic...\n";
    boss->setHp(120); // 120 / 525 = 22.8% < 30%
    bossCombat.executeTurn(1); // Triggers enrage +50% ATK/DEF and slight HP regen on boss turn

    // Summary banner
    std::cout << "\n";
    ConsoleUI::printDivider('=', 68, ConsoleUI::Colors::BRIGHT_GREEN);
    ConsoleUI::printHeader("HỆ THỐNG UI CONSOLE & NLOHMANN/JSON ĐÃ SETUP HOÀN TẤT!", 68, ConsoleUI::Colors::BRIGHT_GREEN);
    ConsoleUI::printDivider('=', 68, ConsoleUI::Colors::BRIGHT_GREEN);

    return 0;
}
