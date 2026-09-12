#include "ui/MainMenu.h"
#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include "ui/BattleUI.h"
#include "SaveManager.h"
#include "DataLoader.h"
#include "CombatEngine.h"
#include <iostream>
#include <fstream>

void MainMenu::run() {
    bool running = true;
    while (running) {
        MainMenuOption option = showMenu();
        switch (option) {
            case MainMenuOption::NEW_GAME:
                handleNewGame();
                break;
            case MainMenuOption::LOAD_GAME:
                handleLoadGame();
                break;
            case MainMenuOption::HERO_SHOWCASE:
                showHeroShowcase();
                break;
            case MainMenuOption::HELP:
                showHelp();
                break;
            case MainMenuOption::EXIT:
                ConsoleUI::clearScreen();
                ConsoleUI::printHeader("CẢM ƠN BẠN ĐÃ TRẢI NGHIỆM RPG ENGINE!", 60, ConsoleUI::Colors::BRIGHT_YELLOW);
                std::cout << "\n  Chúc bạn có những giờ phút lập trình và chơi game vui vẻ!\n\n";
                running = false;
                break;
        }
    }
}

MainMenuOption MainMenu::showMenu() {
    ConsoleUI::clearScreen();
    ASCIIArt::printTitleLogo();

    ConsoleUI::printHeader("MAIN MENU", 50, ConsoleUI::Colors::BRIGHT_YELLOW);
    std::cout << "\n";
    std::cout << "  " << ConsoleUI::colorize("1.", ConsoleUI::Colors::BRIGHT_CYAN) 
              << " Tạo trò chơi mới (New Game)\n";
    std::cout << "  " << ConsoleUI::colorize("2.", ConsoleUI::Colors::BRIGHT_CYAN) 
              << " Tải trò chơi (Load Game)\n";
    std::cout << "  " << ConsoleUI::colorize("3.", ConsoleUI::Colors::BRIGHT_CYAN) 
              << " Xem thông tin các lớp Hero (Showcase)\n";
    std::cout << "  " << ConsoleUI::colorize("4.", ConsoleUI::Colors::BRIGHT_CYAN) 
              << " Hướng dẫn chơi (Help)\n";
    std::cout << "  " << ConsoleUI::colorize("5.", ConsoleUI::Colors::BRIGHT_RED) 
              << " Thoát (Exit)\n\n";
    ConsoleUI::printDivider('-', 50, ConsoleUI::Colors::DIM);

    int choice = ConsoleUI::getIntInput(1, 5, "Nhập lựa chọn của bạn [1-5]: ");
    return static_cast<MainMenuOption>(choice);
}

void MainMenu::showHeroShowcase() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("LỚP NHÂN VẬT (HERO CLASSES)", 60, ConsoleUI::Colors::BRIGHT_CYAN);

    // Warrior
    ASCIIArt::printWarriorArt();
    std::cout << "  " << ConsoleUI::colorize("• Chiến Binh (Warrior):", ConsoleUI::Colors::BRIGHT_BLUE)
              << " Máu cao (120 HP), giáp cứng (8 DEF), sát thương vật lý mạnh mẽ.\n";
    ConsoleUI::printDivider('.', 60, ConsoleUI::Colors::DIM);

    // Mage
    ASCIIArt::printMageArt();
    std::cout << "  " << ConsoleUI::colorize("• Pháp Sư (Mage):", ConsoleUI::Colors::BRIGHT_MAGENTA)
              << " Lượng MP dồi dào (100 MP), kỹ năng phép thuật tầm rộng cực mạnh.\n";
    ConsoleUI::printDivider('.', 60, ConsoleUI::Colors::DIM);

    // Ranger
    ASCIIArt::printRangerArt();
    std::cout << "  " << ConsoleUI::colorize("• Xạ Thủ (Ranger):", ConsoleUI::Colors::BRIGHT_GREEN)
              << " Nhanh nhẹn, tỉ lệ bạo kích cao, tấn công tầm xa chuẩn xác.\n";
    ConsoleUI::printDivider('=', 60, ConsoleUI::Colors::CYAN);

    ConsoleUI::pause();
}

int MainMenu::showLoadGameMenu(const std::vector<int>& availableSlots) {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("CHỌN SLOT LƯU GAME", 50, ConsoleUI::Colors::BRIGHT_YELLOW);

    if (availableSlots.empty()) {
        ConsoleUI::printWarning("Chưa có bản lưu game nào!");
        ConsoleUI::pause();
        return -1;
    }

    std::cout << "\nCác slot hiện có:\n";
    for (int slot : availableSlots) {
        std::cout << "  [" << ConsoleUI::colorize(std::to_string(slot), ConsoleUI::Colors::BRIGHT_GREEN) 
                  << "] Slot " << slot << " (slot" << slot << ".json)\n";
    }
    std::cout << "  [" << ConsoleUI::colorize("0", ConsoleUI::Colors::BRIGHT_RED) << "] Quay lại\n\n";

    while (true) {
        int choice = ConsoleUI::getIntInput(0, 9, "Nhập số slot muốn tải [hoặc 0 để hủy]: ");
        if (choice == 0) return -1;
        for (int slot : availableSlots) {
            if (slot == choice) return choice;
        }
        ConsoleUI::printError("Slot bạn chọn không tồn tại file save! Vui lòng chọn lại.");
    }
}

void MainMenu::showHelp() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("HƯỚNG DẪN CHƠI (HOW TO PLAY)", 60, ConsoleUI::Colors::BRIGHT_GREEN);
    
    std::vector<std::string> lines = {
        "1. Lựa chọn hành động bằng cách nhập số hiển thị trên màn hình.",
        "2. Hệ thống kiểm tra đầu vào nghiêm ngặt: khi nhập sai chữ,",
        "   ký tự lạ hoặc để trống, hệ thống sẽ cảnh báo lịch sự.",
        "3. Chiến đấu theo lượt (Turn-based): Tấn công, dùng Skill, Item.",
        "4. Khám phá cốt truyện phân nhánh nạp trực tiếp từ file JSON.",
        "5. Dữ liệu game được lưu và tải theo chuẩn JSON nlohmann."
    };
    ConsoleUI::printBox(lines, 60, ConsoleUI::Colors::BRIGHT_BLUE);
    
    ConsoleUI::pause();
}

void MainMenu::handleNewGame() {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("TẠO NHÂN VẬT MỚI (NEW GAME)", 60, ConsoleUI::Colors::BRIGHT_CYAN);

    std::cout << "\n";
    std::string heroName = ConsoleUI::getStringInput("  Nhập tên Anh Hùng của bạn: ");
    if (heroName.empty()) {
        heroName = "Arthur";
    }

    std::cout << "\n  Chọn lớp nhân vật (Class):\n";
    std::cout << "  " << ConsoleUI::colorize("1.", ConsoleUI::Colors::BRIGHT_BLUE) << " Chiến Binh (Warrior)  [HP: 120 | MP: 30  | ATK: 18 | DEF: 8]\n";
    std::cout << "  " << ConsoleUI::colorize("2.", ConsoleUI::Colors::BRIGHT_MAGENTA) << " Pháp Sư (Mage)         [HP: 80  | MP: 100 | ATK: 24 | DEF: 3]\n";
    std::cout << "  " << ConsoleUI::colorize("3.", ConsoleUI::Colors::BRIGHT_GREEN) << " Xạ Thủ (Ranger)        [HP: 95  | MP: 50  | ATK: 20 | DEF: 5]\n\n";

    int classChoice = ConsoleUI::getIntInput(1, 3, "  Chọn lớp [1-3]: ");

    Hero player(heroName, HeroClass::WARRIOR, 120, 30, 18, 8);
    if (classChoice == 1) {
        player = Hero(heroName, HeroClass::WARRIOR, 120, 30, 18, 8);
    } else if (classChoice == 2) {
        player = Hero(heroName, HeroClass::MAGE, 80, 100, 24, 3);
    } else {
        player = Hero(heroName, HeroClass::RANGER, 95, 50, 20, 5);
    }

    ConsoleUI::printSuccess("Khởi tạo nhân vật " + player.getName() + " thành công!");
    ConsoleUI::pause();

    // Load story from JSON
    StoryGraph story;
    std::string storyPath = "data/story.json";
    if (!std::ifstream(storyPath).good()) storyPath = "../data/story.json";
    if (!DataLoader::loadStory(storyPath, story)) {
        ConsoleUI::printWarning("Không tìm thấy data/story.json, sử dụng cốt truyện mặc định.");
    }

    playStoryLoop(player, story);
}

void MainMenu::handleLoadGame() {
    SaveManager saveMgr("saves/");
    std::vector<int> slots = saveMgr.getExistingSlots();
    int slot = showLoadGameMenu(slots);
    if (slot <= 0) return;

    Hero player("TempHero", HeroClass::WARRIOR, 100, 20, 10, 5);
    StoryGraph story;
    std::string storyPath = "data/story.json";
    if (!std::ifstream(storyPath).good()) storyPath = "../data/story.json";
    if (!DataLoader::loadStory(storyPath, story)) {
        // Use default story graph
    }

    if (saveMgr.loadGame(slot, player, story)) {
        ConsoleUI::printSuccess("Tải bản lưu Slot " + std::to_string(slot) + " thành công!");
        ConsoleUI::pause();
        playStoryLoop(player, story);
    } else {
        ConsoleUI::printError("Không thể tải bản lưu Slot " + std::to_string(slot) + "!");
        ConsoleUI::pause();
    }
}

void MainMenu::playStoryLoop(Hero& hero, StoryGraph& story) {
    SaveManager saveMgr("saves/");

    while (true) {
        ConsoleUI::clearScreen();
        ConsoleUI::printHeader("RPG ADVENTURE: " + hero.getName(), 65, ConsoleUI::Colors::BRIGHT_YELLOW);

        // Display Hero status
        std::string classStr = "Chiến Binh";
        if (hero.getHeroClass() == HeroClass::MAGE) classStr = "Pháp Sư";
        else if (hero.getHeroClass() == HeroClass::RANGER) classStr = "Xạ Thủ";

        std::cout << "  Hero: " << ConsoleUI::colorize(hero.getName(), ConsoleUI::Colors::BRIGHT_WHITE)
                  << " | Lớp: " << ConsoleUI::colorize(classStr, ConsoleUI::Colors::BRIGHT_BLUE)
                  << " | Level: " << ConsoleUI::colorize(std::to_string(hero.getLevel()), ConsoleUI::Colors::BRIGHT_YELLOW) << "\n";
        ConsoleUI::printProgressBar("  Máu (HP)", hero.getHp(), hero.getMaxHp(), 20);
        ConsoleUI::printProgressBar("  Mana(MP)", hero.getMp(), hero.getMaxMp(), 20, ConsoleUI::Colors::BRIGHT_BLUE);
        ConsoleUI::printDivider('-', 65, ConsoleUI::Colors::DIM);

        // Display Current Story Node
        StoryNode node = story.getCurrentNode();
        std::vector<std::string> storyBox = {
            "Địa điểm: " + node.id,
            node.text
        };
        ConsoleUI::printBox(storyBox, 65, ConsoleUI::Colors::BRIGHT_MAGENTA);

        // Handle BATTLE event
        if (node.type == EventType::BATTLE) {
            ConsoleUI::printWarning("Chiến trường nguy hiểm! Một kẻ địch xuất hiện!");
            Enemy enemy("Quái Vật Hắc Ám", EnemyType::MINION, 50, 14, 4, 50, 20);
            BattleUI battleUI;
            battleUI.renderBattleScreen(hero, enemy, "Trận chiến nổ ra khốc liệt!");
            CombatEngine combat(hero, enemy);
            combat.startBattle();
            while (!combat.isBattleOver()) {
                combat.executeTurn(1);
            }
            if (hero.isAlive()) {
                battleUI.showVictory(enemy);
            } else {
                ConsoleUI::printError("Bạn đã tử trận trên chiến trường! Game Over.");
                ConsoleUI::pause();
                return;
            }
        }

        // Check if Ending
        if (story.isEnding() || node.choices.empty()) {
            std::cout << "\n";
            ASCIIArt::printVictoryBanner();
            ConsoleUI::printHeader("KẾT THÚC CỐT TRUYỆN", 65, ConsoleUI::Colors::BRIGHT_GREEN);
            ConsoleUI::pause();
            return;
        }

        // Show Choices
        std::cout << "\n" << ConsoleUI::colorize("Các lựa chọn của bạn:", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
        int choiceCount = static_cast<int>(node.choices.size());
        for (int i = 0; i < choiceCount; ++i) {
            std::cout << "  " << ConsoleUI::colorize(std::to_string(i + 1) + ".", ConsoleUI::Colors::BRIGHT_GREEN)
                      << " " << node.choices[i].text << "\n";
        }
        int saveOpt = choiceCount + 1;
        int exitOpt = choiceCount + 2;
        std::cout << "  " << ConsoleUI::colorize(std::to_string(saveOpt) + ".", ConsoleUI::Colors::BRIGHT_YELLOW)
                  << " Lưu trò chơi (Save Game vào slot JSON)\n";
        std::cout << "  " << ConsoleUI::colorize(std::to_string(exitOpt) + ".", ConsoleUI::Colors::BRIGHT_RED)
                  << " Quay lại Main Menu\n\n";

        int playerChoice = ConsoleUI::getIntInput(1, exitOpt, "Lựa chọn của bạn [1-" + std::to_string(exitOpt) + "]: ");

        if (playerChoice == exitOpt) {
            return;
        } else if (playerChoice == saveOpt) {
            int slot = ConsoleUI::getIntInput(1, 9, "Chọn số slot lưu [1-9]: ");
            if (saveMgr.saveGame(slot, hero, story)) {
                ConsoleUI::printSuccess("Đã lưu tiến trình thành công vào slot " + std::to_string(slot) + "!");
            } else {
                ConsoleUI::printError("Không thể lưu tiến trình!");
            }
            ConsoleUI::pause();
        } else {
            story.selectChoice(playerChoice - 1);
        }
    }
}
