#include "ui/InventoryUI.h"
#include "ui/ConsoleUI.h"
#include <iostream>
#include <iomanip>

static std::string itemTypeToString(ItemType t) {
    switch (t) {
        case ItemType::WEAPON: return ConsoleUI::colorize("Vũ khí", ConsoleUI::Colors::BRIGHT_RED);
        case ItemType::ARMOR:  return ConsoleUI::colorize("Giáp  ", ConsoleUI::Colors::BRIGHT_BLUE);
        case ItemType::POTION: return ConsoleUI::colorize("Thuốc ", ConsoleUI::Colors::BRIGHT_GREEN);
        default:               return "Khác  ";
    }
}

void InventoryUI::renderInventory(const Inventory& inventory) {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("TÚI ĐỒ (INVENTORY)", 68, ConsoleUI::Colors::BRIGHT_YELLOW);

    std::cout << "  " 
              << ConsoleUI::colorize("Trang bị hiện tại: ", ConsoleUI::Colors::BOLD)
              << "Vũ khí [+" << inventory.getEquippedWeaponBonus() << " ATK] | "
              << "Giáp [+" << inventory.getEquippedArmorBonus() << " DEF]\n";
    ConsoleUI::printDivider('-', 68, ConsoleUI::Colors::DIM);

    int count = inventory.getItemCount();
    if (count == 0) {
        std::cout << "\n  " << ConsoleUI::colorize("(Túi đồ hiện đang trống)", ConsoleUI::Colors::DIM) << "\n\n";
        ConsoleUI::printDivider('=', 68, ConsoleUI::Colors::CYAN);
        return;
    }

    std::cout << "  " 
              << std::left << std::setw(6)  << "#"
              << std::left << std::setw(16) << "Phân loại"
              << std::left << std::setw(20) << "Tên vật phẩm"
              << std::left << std::setw(10) << "Chỉ số"
              << "Mô tả\n";
    ConsoleUI::printDivider('-', 68, ConsoleUI::Colors::DIM);

    for (int i = 0; i < count; ++i) {
        Item item = inventory.getItem(i);
        std::string statStr = (item.getType() == ItemType::POTION) ? 
                              ("+" + std::to_string(item.getStatValue()) + " HP/MP") : 
                              ("+" + std::to_string(item.getStatValue()));

        std::cout << "  "
                  << "[" << ConsoleUI::colorize(std::to_string(i + 1), ConsoleUI::Colors::BRIGHT_CYAN) << "]   "
                  << std::left << std::setw(18) << itemTypeToString(item.getType())
                  << std::left << std::setw(20) << ConsoleUI::colorize(item.getName(), ConsoleUI::Colors::BRIGHT_WHITE)
                  << std::left << std::setw(10) << ConsoleUI::colorize(statStr, ConsoleUI::Colors::BRIGHT_YELLOW)
                  << ConsoleUI::colorize(item.getDescription(), ConsoleUI::Colors::DIM)
                  << "\n";
    }
    ConsoleUI::printDivider('=', 68, ConsoleUI::Colors::CYAN);
}

int InventoryUI::selectItemIndex(const Inventory& inventory) {
    int count = inventory.getItemCount();
    if (count == 0) {
        ConsoleUI::pause();
        return -1;
    }

    std::cout << "\n  Nhập số thứ tự vật phẩm (1 - " << count << ") hoặc [0] để quay lại.\n";
    int choice = ConsoleUI::getIntInput(0, count, "Chọn vật phẩm: ");
    if (choice == 0) return -1;
    return choice - 1; // 0-based index
}

int InventoryUI::selectItemAction(const Item& item) {
    std::cout << "\n  Vật phẩm đã chọn: " << ConsoleUI::colorize(item.getName(), ConsoleUI::Colors::BRIGHT_YELLOW) << "\n";
    if (item.getType() == ItemType::POTION) {
        std::cout << "  1. Sử dụng (Use)\n";
    } else {
        std::cout << "  1. Trang bị (Equip)\n";
    }
    std::cout << "  2. Vứt bỏ (Discard)\n";
    std::cout << "  0. Quay lại (Cancel)\n";

    return ConsoleUI::getIntInput(0, 2, "Chọn thao tác: ");
}
