/**
 * @file Item.cpp
 * @brief Implement Item class methods and ItemFactory JSON loading.
 */

#include "Item.h"
#include <fstream>
#include <iostream>

Item::Item(const std::string& id, const std::string& name, const std::string& description, ItemType type, int statValue)
    : id(id), name(name), description(description), type(type), statValue(statValue) {}

std::string Item::getId() const { return id; }
std::string Item::getName() const { return name; }
std::string Item::getDescription() const { return description; }
ItemType Item::getType() const { return type; }
int Item::getStatValue() const { return statValue; }

void Item::displayInfo() const {
    std::cout << "[" << name << "] (" << itemTypeToString(type) << ")\n"
              << "  Mô tả: " << description << "\n"
              << "  Chỉ số: +" << statValue << "\n";
}

Item ItemFactory::createFromJson(const std::string& id, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return Item(id, "Unknown Item", "Item file not found", ItemType::POTION, 0);
    }

    try {
        nlohmann::json data;
        file >> data;
        if (data.contains(id)) {
            const auto& j = data[id];
            std::string name = j.value("name", id);
            std::string desc = j.value("description", "");
            ItemType type = stringToItemType(j.value("type", "POTION"));
            int statValue = j.value("statValue", 0);
            return Item(id, name, desc, type, statValue);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ItemFactory] Error parsing " << filepath << ": " << e.what() << "\n";
    }

    return Item(id, "Unknown Item", "Item ID not found", ItemType::POTION, 0);
}

std::vector<Item> ItemFactory::loadAllFromJson(const std::string& filepath) {
    std::vector<Item> items;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return items;
    }

    try {
        nlohmann::json data;
        file >> data;
        for (auto& [id, j] : data.items()) {
            std::string name = j.value("name", id);
            std::string desc = j.value("description", "");
            ItemType type = stringToItemType(j.value("type", "POTION"));
            int statValue = j.value("statValue", 0);
            items.emplace_back(id, name, desc, type, statValue);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ItemFactory] Error parsing " << filepath << ": " << e.what() << "\n";
    }

    return items;
}
