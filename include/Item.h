#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

enum class ItemType {
    WEAPON,
    ARMOR,
    POTION
};

inline std::string itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::WEAPON: return "WEAPON";
        case ItemType::ARMOR: return "ARMOR";
        case ItemType::POTION: return "POTION";
        default: return "POTION";
    }
}

inline ItemType stringToItemType(const std::string& str) {
    if (str == "WEAPON") return ItemType::WEAPON;
    if (str == "ARMOR") return ItemType::ARMOR;
    return ItemType::POTION;
}

class Item {
private:
    std::string id;
    std::string name;
    std::string description;
    ItemType type;
    int statValue; // Attack bonus for Weapon, Defense for Armor, HP/MP restore for Potion

public:
    Item(const std::string& id, const std::string& name, const std::string& description, ItemType type, int statValue);
    ~Item() = default;

    std::string getId() const;
    std::string getName() const;
    std::string getDescription() const;
    ItemType getType() const;
    int getStatValue() const;

    void displayInfo() const;
};

class ItemFactory {
public:
    static Item createFromJson(const std::string& id, const std::string& filepath = "data/items.json");
    static std::vector<Item> loadAllFromJson(const std::string& filepath = "data/items.json");
};
