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
    bool removeItem(int index);                        // Xóa theo index
    bool removeItem(const std::string& itemId);        // Xóa theo ID (ưu tiên potion giảm qty)
    bool useItem(int index, Hero& hero);
    bool useItemById(const std::string& itemId, Hero& hero); // Dùng item theo ID

    // Equipment Management
    bool equipWeapon(int index);
    void unequipWeapon();
    bool equipArmor(int index);
    void unequipArmor();

    // Query & Display
    void listItems() const;
    int getItemCount() const;
    int getCapacity() const;
    bool isFull() const;                                     // Kiểm tra túi đồ đã đạt giới hạn tối đa chưa
    bool hasItem(const std::string& itemId) const;           // Kiểm tra tồn tại
    int findItemIndex(const std::string& itemId) const;      // Tìm index theo ID (-1 nếu không thấy)
    int getItemCountById(const std::string& itemId) const;   // Đếm qty potion hoặc 1 cho item khác
    Item getItem(int index) const; // Legacy compatibility
    std::shared_ptr<Item> getItemPtr(int index) const;
    std::shared_ptr<Item> getItemById(const std::string& itemId) const; // Lấy ptr theo ID

    int getEquippedWeaponIndex() const;
    int getEquippedArmorIndex() const;
    std::shared_ptr<Weapon> getEquippedWeapon() const;
    std::shared_ptr<Armor> getEquippedArmor() const;
    int getEquippedWeaponBonus() const;
    int getEquippedArmorBonus() const;

    const std::vector<std::shared_ptr<Item>>& getItems() const;
};
