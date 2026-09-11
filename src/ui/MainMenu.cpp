#include "ui/MainMenu.h"
#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include <iostream>

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
              << " Máu cao, giáp cứng, sát thương vật lý mạnh mẽ.\n";
    ConsoleUI::printDivider('.', 60, ConsoleUI::Colors::DIM);

    // Mage
    ASCIIArt::printMageArt();
    std::cout << "  " << ConsoleUI::colorize("• Pháp Sư (Mage):", ConsoleUI::Colors::BRIGHT_MAGENTA)
              << " Lượng MP dồi dào, kỹ năng phép thuật tầm rộng cực mạnh.\n";
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
        "2. Chiến đấu theo lượt (Turn-based):",
        "   - Tấn công, dùng kỹ năng tiêu tốn MP, dùng bình máu/mana.",
        "   - Quản lý HP không để tụt về 0.",
        "3. Khám phá cốt truyện phân nhánh qua các quyết định.",
        "4. Dữ liệu game được lưu tự động theo chuẩn JSON."
    };
    ConsoleUI::printBox(lines, 60, ConsoleUI::Colors::BRIGHT_BLUE);
    
    ConsoleUI::pause();
}
