#pragma once

#include "Item.h"
#include <vector>
#include <memory>
#include <iostream>

class Hero; // Forward declaration

class Inventory {
private:
    std::vector<std::shared_ptr<Item>> items;
    int capacity;
    int equippedWeaponIndex;
    int equippedArmorIndex;

public:
    explicit Inventory(int capacity = 20);
    ~Inventory() = default;

    // Item Management
    bool addItem(const Item& item);
    bool addItem(std::shared_ptr<Item> item);
    bool removeItem(int index);
    bool useItem(int index, Hero& hero);

    // Equipment Management
    bool equipWeapon(int index);
    void unequipWeapon();
    bool equipArmor(int index);
    void unequipArmor();

    // Query & Display
    void listItems() const;
    int getItemCount() const;
    int getCapacity() const;
    Item getItem(int index) const; // Legacy compatibility
    std::shared_ptr<Item> getItemPtr(int index) const;

    int getEquippedWeaponIndex() const;
    int getEquippedArmorIndex() const;
    std::shared_ptr<Weapon> getEquippedWeapon() const;
    std::shared_ptr<Armor> getEquippedArmor() const;
    int getEquippedWeaponBonus() const;
    int getEquippedArmorBonus() const;

    const std::vector<std::shared_ptr<Item>>& getItems() const;
};
