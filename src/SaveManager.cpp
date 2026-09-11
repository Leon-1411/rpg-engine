/**
 * @file SaveManager.cpp
 * @brief Implement SaveManager class methods with nlohmann/json serialization & Inventory persistence.
 * @author Phong
 */

#include "SaveManager.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
using json = nlohmann::json;

SaveManager::SaveManager(const std::string& saveDir) : saveDirectory(saveDir) {
    std::error_code ec;
    if (!fs::exists(saveDirectory, ec)) {
        fs::create_directories(saveDirectory, ec);
    }
}

std::string SaveManager::getSlotFilePath(int slot) const {
    fs::path dir(saveDirectory);
    return (dir / ("slot" + std::to_string(slot) + ".json")).string();
}

std::string SaveManager::getSaveDirectory() const {
    return saveDirectory;
}

bool SaveManager::saveGame(int slot, const Hero& hero, const StoryGraph& story) {
bool SaveManager::saveGame(int slot, const Hero& hero, const StoryGraph& story) {
    std::error_code ec;
    if (!fs::exists(saveDirectory, ec)) {
        fs::create_directories(saveDirectory, ec);
    }

    std::string filePath = getSlotFilePath(slot);

    try {
        json j;
        j["heroName"] = hero.getName();
        j["heroClass"] = static_cast<int>(hero.getHeroClass());
        j["level"] = hero.getLevel();
        j["exp"] = hero.getExp();
        j["hp"] = hero.getHp();
        j["maxHp"] = hero.getMaxHp();
        j["mp"] = hero.getMp();
        j["maxMp"] = hero.getMaxMp();
        j["attack"] = hero.getAttack();
        j["defense"] = hero.getDefense();
        j["currentStoryNodeId"] = story.getCurrentNode().id;

        // Save Inventory items & equipment indices
        const Inventory& inv = hero.getInventory();
        j["equippedWeaponIndex"] = inv.getEquippedWeaponIndex();
        j["equippedArmorIndex"] = inv.getEquippedArmorIndex();

        json itemsJson = json::array();
        const auto& items = inv.getItems();
        for (size_t i = 0; i < items.size(); ++i) {
            json itemJ;
            itemJ["id"] = items[i]->getId();
            itemJ["name"] = items[i]->getName();
            itemJ["description"] = items[i]->getDescription();
            itemJ["type"] = static_cast<int>(items[i]->getType());
            itemJ["statValue"] = items[i]->getStatValue();
            auto potion = std::dynamic_pointer_cast<Potion>(items[i]);
            itemJ["quantity"] = potion ? potion->getQuantity() : 1;
            itemJ["isEquippedWeapon"] = (static_cast<int>(i) == inv.getEquippedWeaponIndex());
            itemJ["isEquippedArmor"] = (static_cast<int>(i) == inv.getEquippedArmorIndex());
            itemsJson.push_back(itemJ);
        }
        j["inventory"] = itemsJson;

        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            std::cerr << "[SaveManager] Error: Could not open file for writing: " << filePath << "\n";
            return false;
        }

        outFile << j.dump(4);
        outFile.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception during save: " << e.what() << "\n";
        return false;
    }
}

bool SaveManager::loadGame(int slot, Hero& hero, StoryGraph& story) {
    if (!slotExists(slot)) {
        std::cerr << "[SaveManager] Error: Slot " << slot << " does not exist.\n";
        return false;
    }

    std::string filePath = getSlotFilePath(slot);

    try {
        std::ifstream inFile(filePath);
        if (!inFile.is_open()) {
            std::cerr << "[SaveManager] Error: Could not open file for reading: " << filePath << "\n";
            return false;
        }

        json j;
        inFile >> j;
        inFile.close();

        // Restore hero attributes
        if (j.contains("level")) hero.setLevel(j["level"].get<int>());
        if (j.contains("exp")) hero.setExp(j["exp"].get<int>());
        if (j.contains("hp")) hero.setHp(j["hp"].get<int>());
        if (j.contains("mp")) hero.setMp(j["mp"].get<int>());

        // Restore story progress
        if (j.contains("currentStoryNodeId")) {
            std::string nodeId = j["currentStoryNodeId"].get<std::string>();
            story.moveToNode(nodeId);
        }

        // Restore inventory items
        if (j.contains("inventory") && j["inventory"].is_array()) {
            Inventory& inv = hero.getInventory();
            inv.clear();
            for (const auto& itemJ : j["inventory"]) {
                std::string id = itemJ.value("id", "");
                std::string name = itemJ.value("name", "");
                std::string desc = itemJ.value("description", "");
                ItemType type = static_cast<ItemType>(itemJ.value("type", 0));
                int statVal = itemJ.value("statValue", 0);
                int qty = itemJ.value("quantity", 1);

                if (type == ItemType::POTION) {
                    Item potion(id, name, desc, type, statVal, qty);
                    inv.addItem(potion);
                } else {
                    Item equip(id, name, desc, type, statVal);
                    inv.addItem(equip);
                }
            }
            if (j.contains("equippedWeaponIndex")) {
                int wIdx = j["equippedWeaponIndex"].get<int>();
                if (wIdx >= 0) inv.equipWeapon(wIdx);
            }
            if (j.contains("equippedArmorIndex")) {
                int aIdx = j["equippedArmorIndex"].get<int>();
                if (aIdx >= 0) inv.equipArmor(aIdx);
            }
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception during load: " << e.what() << "\n";
        return false;
    }
}
}

bool SaveManager::slotExists(int slot) const {
    std::error_code ec;
    return fs::exists(getSlotFilePath(slot), ec);
}

bool SaveManager::deleteSlot(int slot) {
    if (!slotExists(slot)) {
        return false;
    }
    std::error_code ec;
    return fs::remove(getSlotFilePath(slot), ec);
}

std::vector<int> SaveManager::getExistingSlots() const {
    std::vector<int> slots;
    for (int i = 1; i <= 9; ++i) {
        if (slotExists(i)) {
            slots.push_back(i);
        }
    }
    return slots;
}
