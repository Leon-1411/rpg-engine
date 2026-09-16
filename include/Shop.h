#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Item.h"
#include "Hero.h"

struct ShopItem {
    std::shared_ptr<Item> item;
    int buyPrice;
    int sellPrice;
    int stock; // -1 for unlimited

    ShopItem(std::shared_ptr<Item> it, int buy, int sell = 0, int stk = -1)
        : item(it), buyPrice(buy), sellPrice(sell > 0 ? sell : buy / 2), stock(stk) {}
};

class Shop {
private:
    std::string shopName;
    std::string description;
    std::vector<ShopItem> goods;

public:
    explicit Shop(const std::string& name = "Cho Hoang Thanh Eldoria", const std::string& desc = "Noi cung cap trang bi va duoc pham cho cac hiep si.");
    ~Shop() = default;

    void addItem(std::shared_ptr<Item> item, int buyPrice, int sellPrice = 0, int stock = -1);
    void loadDefaultStock();
    void clear();

    const std::vector<ShopItem>& getGoods() const;
    const std::string& getName() const;
    const std::string& getDescription() const;

    bool buyItem(int shopIndex, Hero& hero);
    bool sellItem(int inventoryIndex, Hero& hero);

    void displayShop(const Hero& hero) const;
};
