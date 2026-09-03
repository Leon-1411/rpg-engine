#pragma once

#include <string>
#include <iostream>

enum class ItemType {
    WEAPON,
    ARMOR,
    POTION
};

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
