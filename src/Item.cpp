/**
 * @file Item.cpp
 * @brief Implement Item class methods.
 * @author Sang
 */

#include "Item.h"

Item::Item(const std::string& id, const std::string& name, const std::string& description, ItemType type, int statValue)
    : id(id), name(name), description(description), type(type), statValue(statValue) {}

std::string Item::getId() const { return id; }
std::string Item::getName() const { return name; }
std::string Item::getDescription() const { return description; }
ItemType Item::getType() const { return type; }
int Item::getStatValue() const { return statValue; }

void Item::displayInfo() const {
    std::cout << "[" << id << "] " << name << ": " << description << " (Value: " << statValue << ")\n";
}
