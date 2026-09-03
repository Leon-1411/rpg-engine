/**
 * @file Inventory.cpp
 * @brief Implement Inventory class methods.
 * @author Sang
 */

#include "Inventory.h"
#include "Hero.h"

Inventory::Inventory(int capacity)
    : capacity(capacity), equippedWeaponIndex(-1), equippedArmorIndex(-1) {}

bool Inventory::addItem(const Item& item) {
    if (static_cast<int>(items.size()) >= capacity) {
        return false;
    }
    items.push_back(item);
    return true;
}

bool Inventory::removeItem(int index) {
    if (index < 0 || index >= static_cast<int>(items.size())) {
        return false;
    }
    items.erase(items.begin() + index);
    if (equippedWeaponIndex == index) equippedWeaponIndex = -1;
    else if (equippedWeaponIndex > index) equippedWeaponIndex--;

    if (equippedArmorIndex == index) equippedArmorIndex = -1;
    else if (equippedArmorIndex > index) equippedArmorIndex--;

    return true;
}

bool Inventory::useItem(int index, Hero& hero) {
    if (index < 0 || index >= static_cast<int>(items.size())) {
        return false;
    }
    const Item& item = items[index];
    if (item.getType() == ItemType::POTION) {
        hero.heal(item.getStatValue());
        removeItem(index);
        return true;
    }
    return false;
}

bool Inventory::equipWeapon(int index) {
    if (index < 0 || index >= static_cast<int>(items.size())) return false;
    if (items[index].getType() == ItemType::WEAPON) {
        equippedWeaponIndex = index;
        return true;
    }
    return false;
}

bool Inventory::equipArmor(int index) {
    if (index < 0 || index >= static_cast<int>(items.size())) return false;
    if (items[index].getType() == ItemType::ARMOR) {
        equippedArmorIndex = index;
        return true;
    }
    return false;
}

void Inventory::listItems() const {
    std::cout << "--- Inventory (" << items.size() << "/" << capacity << ") ---\n";
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << i + 1 << ". ";
        items[i].displayInfo();
    }
}

int Inventory::getItemCount() const {
    return static_cast<int>(items.size());
}

Item Inventory::getItem(int index) const {
    return items.at(index);
}

int Inventory::getEquippedWeaponBonus() const {
    if (equippedWeaponIndex >= 0 && equippedWeaponIndex < static_cast<int>(items.size())) {
        return items[equippedWeaponIndex].getStatValue();
    }
    return 0;
}

int Inventory::getEquippedArmorBonus() const {
    if (equippedArmorIndex >= 0 && equippedArmorIndex < static_cast<int>(items.size())) {
        return items[equippedArmorIndex].getStatValue();
    }
    return 0;
}
