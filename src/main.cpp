#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include "ui/MainMenu.h"
#include "Hero.h"
#include "Enemy.h"
#include "Item.h"
#include "Inventory.h"
#include "CombatEngine.h"
#include "StoryGraph.h"
#include "SaveManager.h"
#include "ui/BattleUI.h"
#include "ui/InventoryUI.h"
#include <iostream>
#include <string>

static void runDemo() {
    ConsoleUI::clearScreen();
    ASCIIArt::printTitleLogo();
    ConsoleUI::printHeader("RPG ENGINE AUTOMATED DEMO", 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    std::cout << "\n" << ConsoleUI::colorize("=== [1] KHỞI TẠO HERO ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    Hero player("Arthur", HeroClass::WARRIOR, 100, 30, 20, 5);
    ASCIIArt::printWarriorArt();
    std::cout << "  Tên: " << ConsoleUI::colorize(player.getName(), ConsoleUI::Colors::BRIGHT_WHITE)
              << " | Lớp: " << ConsoleUI::colorize("Chiến Binh (Warrior)", ConsoleUI::Colors::BRIGHT_BLUE)
              << " | Cấp độ: " << ConsoleUI::colorize(std::to_string(player.getLevel()), ConsoleUI::Colors::BRIGHT_YELLOW) << "\n";
    ConsoleUI::printProgressBar("  Máu (HP)", player.getHp(), player.getMaxHp(), 20);
    ConsoleUI::printProgressBar("  Mana(MP)", player.getMp(), player.getMaxMp(), 20, ConsoleUI::Colors::BRIGHT_BLUE);
    ConsoleUI::printSuccess("Khởi tạo Hero thành công!");

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

    std::cout << "\n" << ConsoleUI::colorize("=== [3] CỐT TRUYỆN (STORY GRAPH) ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    StoryGraph story;
    StoryNode current = story.getCurrentNode();
    std::vector<std::string> storyBox = {
        "Node ID: " + current.id,
        current.text
    };
    ConsoleUI::printBox(storyBox, 68, ConsoleUI::Colors::BRIGHT_MAGENTA);
    ConsoleUI::printSuccess("Cốt truyện đã sẵn sàng!");

    std::cout << "\n" << ConsoleUI::colorize("=== [4] HỆ THỐNG GIAO TRANH (BATTLE HUD) ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    Enemy minion("Goblin Scout", EnemyType::MINION, 40, 12, 3, 50, 15);
    BattleUI battleUI;
    battleUI.renderBattleScreen(player, minion, "Một tên Goblin Scout bất ngờ xuất hiện chắn đường!");
    CombatEngine combat(player, minion);
    combat.startBattle();
    battleUI.printCombatLog("Arthur vung kiếm tấn công Goblin Scout!");
    combat.executeTurn(1);
    if (!combat.isBattleOver()) {
        battleUI.printCombatLog("Arthur tiếp tục dồn đòn kết liễu!");
        combat.executeTurn(1);
    }
    battleUI.showVictory(minion);

    std::cout << "\n" << ConsoleUI::colorize("=== [5] LƯU DỮ LIỆU GAME (SAVE / LOAD JSON) ===", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
    SaveManager saveMgr("saves/");
    bool saved = saveMgr.saveGame(1, player, story);
    if (saved) {
        ConsoleUI::printSuccess("Đã lưu tiến trình vào file JSON: saves/slot1.json");
    } else {
        ConsoleUI::printError("Lỗi khi lưu game!");
    }

    Hero loadedHero("Blank", HeroClass::WARRIOR, 1, 1, 1, 1);
    StoryGraph loadedStory;
    if (saveMgr.loadGame(1, loadedHero, loadedStory)) {
        ConsoleUI::printSuccess("Đã tải lại thành công dữ liệu từ file JSON! Hero: " + loadedHero.getName());
    }

    std::cout << "\n";
    ConsoleUI::printDivider('=', 68, ConsoleUI::Colors::BRIGHT_GREEN);
    ConsoleUI::printHeader("HỆ THỐNG UI CONSOLE & NLOHMANN/JSON ĐÃ HOÀN TẤT!", 68, ConsoleUI::Colors::BRIGHT_GREEN);
    ConsoleUI::printDivider('=', 68, ConsoleUI::Colors::BRIGHT_GREEN);
}

int main(int argc, char* argv[]) {
    // 0. Initialize Terminal (ANSI sequences & UTF-8)
    ConsoleUI::initConsole();

    // If --demo flag passed, run automated demo
    if (argc > 1 && std::string(argv[1]) == "--demo") {
        runDemo();
        return 0;
    }

    // Default: Run interactive Terminal Menu
    MainMenu menu;
    menu.run();

    return 0;
}
