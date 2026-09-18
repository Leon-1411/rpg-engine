#include "ui/InventoryUI.h"
#include "ui/ConsoleUI.h"
#include <iostream>
#include <iomanip>

static std::string itemTypeToString(ItemType t) {
    switch (t) {
        case ItemType::WEAPON:   return ConsoleUI::colorize("Vũ khí  ", ConsoleUI::Colors::BRIGHT_RED);
        case ItemType::ARMOR:    return ConsoleUI::colorize("Giáp    ", ConsoleUI::Colors::BRIGHT_BLUE);
        case ItemType::POTION:   return ConsoleUI::colorize("Thuốc   ", ConsoleUI::Colors::BRIGHT_GREEN);
        case ItemType::KEY_ITEM: return ConsoleUI::colorize("Nhiệm vụ", ConsoleUI::Colors::BRIGHT_MAGENTA);
        default:                 return "Khác    ";
    }
}

void InventoryUI::renderInventory(const Inventory& inventory) {
    ConsoleUI::clearScreen();
    ConsoleUI::printHeader("TÚI ĐỒ (INVENTORY)", 72, ConsoleUI::Colors::BRIGHT_YELLOW);

    std::cout << "  " 
              << ConsoleUI::colorize("Trang bị hiện tại: ", ConsoleUI::Colors::BOLD)
              << "Vũ khí [+" << inventory.getEquippedWeaponBonus() << " ATK] | "
              << "Giáp [+" << inventory.getEquippedArmorBonus() << " DEF] | "
              << "Sức chứa: " << inventory.getItemCount() << "/" << inventory.getCapacity() << "\n";
    ConsoleUI::printDivider('-', 72, ConsoleUI::Colors::DIM);

    int count = inventory.getItemCount();
    if (count == 0) {
        std::cout << "\n  " << ConsoleUI::colorize("(Túi đồ hiện đang trống)", ConsoleUI::Colors::DIM) << "\n\n";
        ConsoleUI::printDivider('=', 72, ConsoleUI::Colors::CYAN);
        return;
    }

    std::cout << "  " 
              << std::left << std::setw(6)  << "#"
              << std::left << std::setw(14) << "Phân loại"
              << std::left << std::setw(24) << "Tên vật phẩm"
              << std::left << std::setw(14) << "Chỉ số"
              << "Trạng thái / Mô tả\n";
    ConsoleUI::printDivider('-', 72, ConsoleUI::Colors::DIM);

    for (int i = 0; i < count; ++i) {
        auto itemPtr = inventory.getItemPtr(i);
        if (!itemPtr) continue;

        std::string statStr;
        if (itemPtr->getType() == ItemType::POTION) {
            auto potion = std::dynamic_pointer_cast<Potion>(itemPtr);
            statStr = (potion && potion->isMana()) ?
                      ("+" + std::to_string(itemPtr->getStatValue()) + " MP") :
                      ("+" + std::to_string(itemPtr->getStatValue()) + " HP");
        } else if (itemPtr->getType() == ItemType::KEY_ITEM) {
            statStr = "Cốt truyện";
        } else {
            statStr = "+" + std::to_string(itemPtr->getStatValue());
        }

        std::string statusTag = "";
        if (i == inventory.getEquippedWeaponIndex()) {
            statusTag = ConsoleUI::colorize("[ĐANG TRANG BỊ VŨ KHÍ] ", ConsoleUI::Colors::BRIGHT_GREEN);
        } else if (i == inventory.getEquippedArmorIndex()) {
            statusTag = ConsoleUI::colorize("[ĐANG TRANG BỊ GIÁP] ", ConsoleUI::Colors::BRIGHT_GREEN);
        }

        std::string displayName = itemPtr->getName();
        if (itemPtr->getType() == ItemType::POTION) {
            auto pot = std::dynamic_pointer_cast<Potion>(itemPtr);
            if (pot && pot->getQuantity() > 1) {
                displayName += " (x" + std::to_string(pot->getQuantity()) + ")";
            }
        }

        std::cout << "  "
                  << "[" << ConsoleUI::colorize(std::to_string(i + 1), ConsoleUI::Colors::BRIGHT_CYAN) << "]   "
                  << std::left << std::setw(16) << itemTypeToString(itemPtr->getType())
                  << std::left << std::setw(24) << ConsoleUI::colorize(displayName, ConsoleUI::Colors::BRIGHT_WHITE)
                  << std::left << std::setw(14) << ConsoleUI::colorize(statStr, ConsoleUI::Colors::BRIGHT_YELLOW)
                  << statusTag << ConsoleUI::colorize(itemPtr->getDescription(), ConsoleUI::Colors::DIM)
                  << "\n";
    }
    ConsoleUI::printDivider('=', 72, ConsoleUI::Colors::CYAN);
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
        std::cout << "  2. Vứt bỏ (Discard)\n";
        std::cout << "  0. Quay lại (Cancel)\n";
        return ConsoleUI::getIntInput(0, 2, "Chọn thao tác: ");
    } else if (item.getType() == ItemType::KEY_ITEM) {
        std::cout << "  1. Xem chi tiết (View Info)\n";
        std::cout << "  0. Quay lại (Cancel)\n";
        return ConsoleUI::getIntInput(0, 1, "Chọn thao tác: ");
    } else {
        std::cout << "  1. Trang bị (Equip)\n";
        std::cout << "  2. Vứt bỏ (Discard)\n";
        std::cout << "  0. Quay lại (Cancel)\n";
        return ConsoleUI::getIntInput(0, 2, "Chọn thao tác: ");
    }
}

