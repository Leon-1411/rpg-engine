#include "Shop.h"
#include <iostream>

Shop::Shop(const std::string& name, const std::string& desc)
    : shopName(name), description(desc) {}

void Shop::addItem(std::shared_ptr<Item> item, int buyPrice, int sellPrice, int stock) {
    if (item) {
        goods.emplace_back(item, buyPrice, sellPrice, stock);
    }
}

void Shop::loadDefaultStock() {
    goods.clear();
    goods.emplace_back(std::make_shared<Potion>("pot_01", "Health Potion", "Binh thuoc hoi phuc 35 HP", 35, false), 30, 15, -1);
    goods.emplace_back(std::make_shared<Potion>("pot_02", "Mana Potion", "Binh thuoc hoi phuc 25 MP", 25, true), 30, 15, -1);
    goods.emplace_back(std::make_shared<Potion>("GreaterHealthPotion", "Greater Health Potion", "Binh dai duoc hoi phuc 70 HP", 70, false), 60, 30, 5);
    goods.emplace_back(std::make_shared<Weapon>("wpn_01", "Iron Sword", "Thanh kiem sat co ban, ATK +12", 12), 80, 40, 2);
    goods.emplace_back(std::make_shared<Weapon>("wpn_02", "Magic Staff", "Gay phep khuech dai ma thuat, ATK +15", 15), 100, 50, 2);
    goods.emplace_back(std::make_shared<Weapon>("wpn_03", "Long Bow", "Cung dai ngam ban tu xa, ATK +10", 10), 75, 35, 2);
    goods.emplace_back(std::make_shared<Armor>("arm_01", "Iron Armor", "Bo giap sat kien co, DEF +8", 8), 70, 35, 2);
    goods.emplace_back(std::make_shared<Armor>("arm_02", "Magic Robe", "Ao choang phap thuat, DEF +4", 4), 60, 30, 2);
    goods.emplace_back(std::make_shared<Armor>("arm_03", "Leather Armor", "Ao giap da nhe nhang, DEF +6", 6), 55, 25, 2);
}

void Shop::clear() {
    goods.clear();
}

const std::vector<ShopItem>& Shop::getGoods() const {
    return goods;
}

const std::string& Shop::getName() const {
    return shopName;
}

const std::string& Shop::getDescription() const {
    return description;
}

bool Shop::buyItem(int shopIndex, Hero& hero) {
    if (shopIndex < 0 || shopIndex >= static_cast<int>(goods.size())) {
        std::cout << "[Shop] Lua chon khong hop le!\n";
        return false;
    }

    ShopItem& gItem = goods[shopIndex];
    if (gItem.stock == 0) {
        std::cout << "[Shop] Mat hang nay da het!\n";
        return false;
    }

    if (hero.getGold() < gItem.buyPrice) {
        std::cout << "[Shop] Ban khong du vang! Can: " << gItem.buyPrice << " Vang (Hien co: " << hero.getGold() << " Vang).\n";
        return false;
    }

    if (hero.getInventory().isFull()) {
        std::cout << "[Shop] Tui do cua ban da day, khong the mua them!\n";
        return false;
    }

    hero.removeGold(gItem.buyPrice);
    hero.getInventory().addItem(gItem.item->clone());
    if (gItem.stock > 0) {
        gItem.stock--;
    }

    std::cout << "[Shop] Giao dich thanh cong! Ban da mua " << gItem.item->getName() 
              << " voi gia " << gItem.buyPrice << " Vang. Con lai: " << hero.getGold() << " Vang.\n";
    return true;
}

bool Shop::sellItem(int inventoryIndex, Hero& hero) {
    auto invItem = hero.getInventory().getItemPtr(inventoryIndex);
    if (!invItem) {
        std::cout << "[Shop] Khong tim thay vat pham trong tui do!\n";
        return false;
    }

    int sellValue = 25;
    for (const auto& g : goods) {
        if (g.item && g.item->getId() == invItem->getId()) {
            sellValue = g.sellPrice;
            break;
        }
    }

    std::string itemName = invItem->getName();
    if (hero.getInventory().removeItem(inventoryIndex)) {
        hero.addGold(sellValue);
        std::cout << "[Shop] Ban da ban " << itemName << " va nhan duoc " << sellValue << " Vang. Hien co: " << hero.getGold() << " Vang.\n";
        return true;
    }
    return false;
}

void Shop::displayShop(const Hero& hero) const {
    std::cout << "\n=========================================================\n";
    std::cout << "               " << shopName << "\n";
    std::cout << "  " << description << "\n";
    std::cout << "  Tai san cua " << hero.getName() << ": " << hero.getGold() << " Vang | Suc chua: " 
              << hero.getInventory().getItemCount() << "/" << hero.getInventory().getCapacity() << "\n";
    std::cout << "=========================================================\n";

    if (goods.empty()) {
        std::cout << "  (Cua hang hien khong co hang hoa)\n";
    } else {
        for (size_t i = 0; i < goods.size(); ++i) {
            const auto& g = goods[i];
            std::cout << "  [" << (i + 1) << "] " << g.item->getName()
                      << " - Gia mua: " << g.buyPrice << " Vang (Ban: " << g.sellPrice << " Vang)";
            if (g.stock >= 0) {
                std::cout << " [SL: " << g.stock << "]";
            }
            std::cout << "\n      " << g.item->getDescription() << "\n";
        }
    }
    std::cout << "========================================================\n";
}
