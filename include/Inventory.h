#pragma once

#include "Item.h"
#include <vector>
#include <memory>
#include <iostream>

class Inventory {
private:
    std::vector<Item> items;
    int capacity;
    int equippedWeaponIndex;
    int equippedArmorIndex;

public:
    explicit Inventory(int capacity = 20);
    ~Inventory() = default;

    bool addItem(const Item& item);
    bool removeItem(int index);
    bool useItem(int index, class Hero& hero);
    
    bool equipWeapon(int index);
    bool equipArmor(int index);

    void listItems() const;
    int getItemCount() const;
    Item getItem(int index) const;

    int getEquippedWeaponBonus() const;
    int getEquippedArmorBonus() const;
};
