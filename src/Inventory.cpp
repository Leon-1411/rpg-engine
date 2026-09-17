/**
 * @file Inventory.cpp
 * @brief Implement Inventory class methods with polymorphic item storage,
 *        potion stacking, and equipment management.
 * @author Sang
 */

#include "Inventory.h"
#include "Hero.h"
#include <iostream>
#include <algorithm>

Inventory::Inventory(int capacity)
    : capacity(capacity), equippedWeaponIndex(-1), equippedArmorIndex(-1) {}

void Inventory::clear() {
    items.clear();
    equippedWeaponIndex = -1;
    equippedArmorIndex = -1;
}

bool Inventory::addItem(const Item& item) {
    if (item.getType() == ItemType::POTION) {
        // Dùng dynamic_cast để đọc qty và isMana thực tế từ Potion subclass
        const Potion* potionPtr = dynamic_cast<const Potion*>(&item);
        int  qtyToAdd = potionPtr ? potionPtr->getQuantity() : 1;
        bool isMana   = potionPtr ? potionPtr->isMana()      : false;

        // Kiểm tra xem đã có potion cùng id trong túi để gộp (stack) chưa
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i]->getId() == item.getId()) {
                auto existingPotion = std::dynamic_pointer_cast<Potion>(items[i]);
                if (existingPotion) {
                    existingPotion->addQuantity(qtyToAdd);
                    return true;
                }
            }
        }
        // Nếu chưa có, kiểm tra sức chứa trước khi thêm mới
        if (isFull()) {
            std::cout << "[Inventory Error] Không thể thêm '" << item.getName()
                      << "': Túi đồ đã đầy (tối đa " << capacity << " món)!\n";
            return false;
        }
        items.push_back(std::make_shared<Potion>(
            item.getId(), item.getName(), item.getDescription(),
            item.getStatValue(), isMana, qtyToAdd   // giữ đúng qty và isMana
        ));
        return true;
    }

    if (isFull()) {
        std::cout << "[Inventory Error] Không thể thêm '" << item.getName()
                  << "': Túi đồ đã đầy (tối đa " << capacity << " món)!\n";
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

    if (isFull()) {
        std::cout << "[Inventory Error] Không thể thêm '" << item->getName()
                  << "': Túi đồ đã đầy (tối đa " << capacity << " món)!\n";
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

// ==========================================
// Xóa item theo ID
// - Nếu là Potion: giảm qty đi 1, chỉ xóa slot khi qty == 0
// - Nếu là Weapon/Armor: xóa slot người cố nhất có ID đó
// ==========================================
bool Inventory::removeItem(const std::string& itemId) {
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        if (items[i]->getId() == itemId) {
            if (items[i]->getType() == ItemType::POTION) {
                auto potion = std::dynamic_pointer_cast<Potion>(items[i]);
                if (potion && potion->getQuantity() > 1) {
                    // Còn nhiều: chỉ giảm qty
                    potion->consumeOne();
                    return true;
                }
            }
            // Weapon/Armor hoặc Potion cuối cùng: xóa slot
            return removeItem(i);
        }
    }
    return false; // Không tìm thấy
}

// Dùng item theo ID (tìm và gọi useItem thường)
bool Inventory::useItemById(const std::string& itemId, Hero& hero) {
    int idx = findItemIndex(itemId);
    if (idx == -1) return false;
    return useItem(idx, hero);
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

bool Inventory::isFull() const {
    return static_cast<int>(items.size()) >= capacity;
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

// ==========================================
// Query helpers
// ==========================================

bool Inventory::hasItem(const std::string& itemId) const {
    return findItemIndex(itemId) != -1;
}

int Inventory::findItemIndex(const std::string& itemId) const {
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        if (items[i]->getId() == itemId) {
            return i;
        }
    }
    return -1;
}

// Trả về quantity nếu là Potion, hoặc 1 nếu là Weapon/Armor, 0 nếu không có
int Inventory::getItemCountById(const std::string& itemId) const {
    int idx = findItemIndex(itemId);
    if (idx == -1) return 0;
    if (items[idx]->getType() == ItemType::POTION) {
        auto potion = std::dynamic_pointer_cast<Potion>(items[idx]);
        return potion ? potion->getQuantity() : 1;
    }
    return 1;
}

std::shared_ptr<Item> Inventory::getItemById(const std::string& itemId) const {
    int idx = findItemIndex(itemId);
    if (idx == -1) return nullptr;
    return items[idx];
}
