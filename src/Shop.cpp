#include "Shop.h"
#include <iostream>

Shop::Shop(const std::string& name, const std::string& desc)
    : shopName(name), description(desc) {}

void Shop::addItem(std::shared_ptr<Item> item, int buyPrice, int sellPrice, int stock) {
    if (item) {
        goods.emplace_back(item, buyPrice, sellPrice, stock);
    }
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

    hero.addGold(-gItem.buyPrice);
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
