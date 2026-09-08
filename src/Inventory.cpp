/**
 * @file Inventory.cpp
 * @brief Implement Inventory class methods with polymorphic item storage,
 *        potion stacking, and equipment management.
 * @author Sang
 */

#include "Inventory.h"
#include "Hero.h"

Inventory::Inventory(int capacity)
    : capacity(capacity), equippedWeaponIndex(-1), equippedArmorIndex(-1) {}

bool Inventory::addItem(const Item& item) {
    if (item.getType() == ItemType::POTION) {
        // Kiểm tra xem đã có potion cùng id trong túi để gộp (stack) chưa
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i]->getId() == item.getId()) {
                auto potion = std::dynamic_pointer_cast<Potion>(items[i]);
                if (potion) {
                    potion->addQuantity(1);
                    return true;
                }
            }
        }
        // Nếu chưa có, kiểm tra sức chứa trước khi thêm mới
        if (static_cast<int>(items.size()) >= capacity) {
            return false;
        }
        items.push_back(std::make_shared<Potion>(
            item.getId(), item.getName(), item.getDescription(), item.getStatValue(), false, 1
        ));
        return true;
    }

    if (static_cast<int>(items.size()) >= capacity) {
        return false;
    }
    items.push_back(item.clone());
    return true;
}

bool Inventory::addItem(std::shared_ptr<Item> item) {
    if (!item) return false;

    if (item->getType() == ItemType::POTION) {
        auto newPotion = std::dynamic_pointer_cast<Potion>(item);
        int qtyToAdd = newPotion ? newPotion->getQuantity() : 1;

        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i]->getId() == item->getId()) {
                auto existingPotion = std::dynamic_pointer_cast<Potion>(items[i]);
                if (existingPotion) {
                    existingPotion->addQuantity(qtyToAdd);
                    return true;
                }
            }
        }
    }

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

    // Cập nhật lại chỉ số vũ khí trang bị
    if (equippedWeaponIndex == index) {
        equippedWeaponIndex = -1;
    } else if (equippedWeaponIndex > index) {
        equippedWeaponIndex--;
    }

    // Cập nhật lại chỉ số giáp trang bị
    if (equippedArmorIndex == index) {
        equippedArmorIndex = -1;
    } else if (equippedArmorIndex > index) {
        equippedArmorIndex--;
    }

    return true;
}

bool Inventory::useItem(int index, Hero& hero) {
    if (index < 0 || index >= static_cast<int>(items.size())) {
        return false;
    }

    auto item = items[index];
    if (item->getType() == ItemType::POTION) {
        auto potion = std::dynamic_pointer_cast<Potion>(item);
        if (potion) {
            potion->apply(hero);
            // Giảm số lượng; nếu hết (consumeOne trả về false) thì xóa khỏi túi
            if (!potion->consumeOne()) {
                removeItem(index);
            }
            return true;
        } else {
            hero.heal(item->getStatValue());
            removeItem(index);
            return true;
        }
    }
    return false;
}

bool Inventory::equipWeapon(int index) {
    if (index < 0 || index >= static_cast<int>(items.size())) return false;
    if (items[index]->getType() == ItemType::WEAPON) {
        equippedWeaponIndex = index;
        return true;
    }
    return false;
}

void Inventory::unequipWeapon() {
    equippedWeaponIndex = -1;
}

bool Inventory::equipArmor(int index) {
    if (index < 0 || index >= static_cast<int>(items.size())) return false;
    if (items[index]->getType() == ItemType::ARMOR) {
        equippedArmorIndex = index;
        return true;
    }
    return false;
}

void Inventory::unequipArmor() {
    equippedArmorIndex = -1;
}

void Inventory::listItems() const {
    std::cout << "--- Inventory (" << items.size() << "/" << capacity << ") ---\n";
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << i + 1 << ". ";
        items[i]->displayInfo();
        if (static_cast<int>(i) == equippedWeaponIndex) {
            std::cout << "    ==> [EQUIPPED WEAPON]\n";
        } else if (static_cast<int>(i) == equippedArmorIndex) {
            std::cout << "    ==> [EQUIPPED ARMOR]\n";
        }
    }
}

int Inventory::getItemCount() const {
    return static_cast<int>(items.size());
}

int Inventory::getCapacity() const {
    return capacity;
}

Item Inventory::getItem(int index) const {
    return *(items.at(index));
}

std::shared_ptr<Item> Inventory::getItemPtr(int index) const {
    if (index >= 0 && index < static_cast<int>(items.size())) {
        return items[index];
    }
    return nullptr;
}

int Inventory::getEquippedWeaponIndex() const {
    return equippedWeaponIndex;
}

int Inventory::getEquippedArmorIndex() const {
    return equippedArmorIndex;
}

std::shared_ptr<Weapon> Inventory::getEquippedWeapon() const {
    if (equippedWeaponIndex >= 0 && equippedWeaponIndex < static_cast<int>(items.size())) {
        return std::dynamic_pointer_cast<Weapon>(items[equippedWeaponIndex]);
    }
    return nullptr;
}

std::shared_ptr<Armor> Inventory::getEquippedArmor() const {
    if (equippedArmorIndex >= 0 && equippedArmorIndex < static_cast<int>(items.size())) {
        return std::dynamic_pointer_cast<Armor>(items[equippedArmorIndex]);
    }
    return nullptr;
}

int Inventory::getEquippedWeaponBonus() const {
    if (equippedWeaponIndex >= 0 && equippedWeaponIndex < static_cast<int>(items.size())) {
        return items[equippedWeaponIndex]->getStatValue();
    }
    return 0;
}

int Inventory::getEquippedArmorBonus() const {
    if (equippedArmorIndex >= 0 && equippedArmorIndex < static_cast<int>(items.size())) {
        return items[equippedArmorIndex]->getStatValue();
    }
    return 0;
}

const std::vector<std::shared_ptr<Item>>& Inventory::getItems() const {
    return items;
}
