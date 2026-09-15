/**
 * @file SaveManager.cpp
 * @brief Implement SaveManager class methods with nlohmann/json serialization,
 *        full Hero/Inventory/StoryGraph persistence, and multi-slot management.
 * @author Phong & Team
 */

#include "SaveManager.h"
#include "Item.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <filesystem>
#include <set>
#include <regex>

using json = nlohmann::json;
namespace fs = std::filesystem;

static void ensureDirectoryExists(const std::string& path) {
    try {
        if (!path.empty() && !fs::exists(path)) {
            fs::create_directories(path);
        }
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Warning: Failed to create directory " << path << ": " << e.what() << "\n";
    }
}

SaveManager::SaveManager(const std::string& saveDir) : saveDirectory(saveDir) {
    ensureDirectoryExists(saveDirectory);
}

std::string SaveManager::getSlotFilePath(int slot) const {
    std::string dir = saveDirectory;
    if (!dir.empty() && dir.back() != '/' && dir.back() != '\\') {
        dir += "/";
    }
    return dir + "slot" + std::to_string(slot) + ".json";
}

std::string SaveManager::getSaveDirectory() const {
    return saveDirectory;
}

SavedGameState SaveManager::createSnapshot(const Hero& hero, const StoryGraph& story) const {
    SavedGameState state;

    // 1. Hero attributes
    state.heroName = hero.getName();
    state.heroClass = hero.getHeroClass();
    state.level = hero.getLevel();
    state.exp = hero.getExp();
    state.hp = hero.getHp();
    state.maxHp = hero.getMaxHp();
    state.mp = hero.getMp();
    state.maxMp = hero.getMaxMp();
    state.attack = hero.getAttack();
    state.defense = hero.getDefense();
    state.armorPenetration = hero.getArmorPenetration();
    state.critChance = hero.getCritChance();
    state.critDamage = hero.getCritDamage();
    state.ignoreArmor = hero.isIgnoreArmor();
    state.readyArrows = hero.getReadyArrows();
    state.gold = hero.getGold();

    // Status effects
    state.poisonTurns = hero.getPoisonTurns();
    state.poisonDamagePerTurn = hero.getPoisonDamagePerTurn();
    state.regenTurns = hero.getRegenTurns();
    state.regenPerTurn = hero.getRegenPerTurn();

    // Skill cooldowns
    state.skillCooldowns = hero.getSkillCooldowns();

    // 2. Story progress
    std::string currentNode = story.getCurrentNodeId();
    if (currentNode.empty()) {
        currentNode = story.getCurrentNode().id;
    }
    state.currentStoryNodeId = currentNode;
    state.storyFlags = story.getStoryFlags();

    // 3. Inventory state
    const Inventory& inv = hero.getInventory();
    state.inventoryCapacity = inv.getCapacity();
    state.equippedWeaponIndex = inv.getEquippedWeaponIndex();
    state.equippedArmorIndex = inv.getEquippedArmorIndex();

    const auto& items = inv.getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        SavedItemRecord rec;
        rec.id = items[i]->getId();
        rec.name = items[i]->getName();
        rec.description = items[i]->getDescription();
        rec.type = static_cast<int>(items[i]->getType());
        rec.statValue = items[i]->getStatValue();

        auto potion = std::dynamic_pointer_cast<Potion>(items[i]);
        if (potion) {
            rec.quantity = potion->getQuantity();
            rec.isManaPotion = potion->isMana();
        } else {
            rec.quantity = 1;
            rec.isManaPotion = false;
        }

        rec.isEquippedWeapon = (static_cast<int>(i) == inv.getEquippedWeaponIndex());
        rec.isEquippedArmor = (static_cast<int>(i) == inv.getEquippedArmorIndex());

        state.inventoryItems.push_back(rec);
    }

    return state;
}

bool SaveManager::applySnapshot(const SavedGameState& state, Hero& hero, StoryGraph& story) const {
    try {
        // 1. Restore hero attributes
        hero.setName(state.heroName);
        hero.setHeroClass(state.heroClass);
        hero.setMaxHp(state.maxHp);
        hero.setMaxMp(state.maxMp);
        hero.setHp(state.hp);
        hero.setMp(state.mp);
        hero.setLevel(state.level);
        hero.setExp(state.exp);
        hero.setAttack(state.attack);
        hero.setDefense(state.defense);
        hero.setArmorPenetration(state.armorPenetration);
        hero.setCritChance(state.critChance);
        hero.setCritDamage(state.critDamage);
        hero.setIgnoreArmor(state.ignoreArmor);
        hero.setReadyArrows(state.readyArrows);
        hero.setGold(state.gold);

        // Status effects
        hero.clearStatusEffects();
        if (state.poisonTurns > 0 && state.poisonDamagePerTurn > 0) {
            hero.applyPoison(state.poisonTurns, state.poisonDamagePerTurn);
        }
        if (state.regenTurns > 0 && state.regenPerTurn > 0) {
            hero.applyRegen(state.regenTurns, state.regenPerTurn);
        }

        // Skill cooldowns
        if (!state.skillCooldowns.empty()) {
            hero.setSkillCooldowns(state.skillCooldowns);
        }

        // 2. Restore story progress
        if (!state.currentStoryNodeId.empty()) {
            story.setCurrentNodeId(state.currentStoryNodeId);
            story.moveToNode(state.currentStoryNodeId);
        }
        story.setStoryFlags(state.storyFlags);

        // 3. Restore inventory
        Inventory& inv = hero.getInventory();
        inv.clear();

        for (const auto& rec : state.inventoryItems) {
            ItemType type = static_cast<ItemType>(rec.type);
            if (type == ItemType::POTION) {
                inv.addItem(std::make_shared<Potion>(
                    rec.id, rec.name, rec.description, rec.statValue, rec.isManaPotion, rec.quantity
                ));
            } else if (type == ItemType::WEAPON) {
                inv.addItem(std::make_shared<Weapon>(
                    rec.id, rec.name, rec.description, rec.statValue
                ));
            } else if (type == ItemType::ARMOR) {
                inv.addItem(std::make_shared<Armor>(
                    rec.id, rec.name, rec.description, rec.statValue
                ));
            } else {
                inv.addItem(std::make_shared<Item>(
                    rec.id, rec.name, rec.description, type, rec.statValue
                ));
            }
        }

        // Restore equipment with validation
        int itemsCount = static_cast<int>(inv.getItems().size());
        if (state.equippedWeaponIndex >= 0 && state.equippedWeaponIndex < itemsCount) {
            inv.equipWeapon(state.equippedWeaponIndex);
        } else {
            for (size_t i = 0; i < state.inventoryItems.size() && i < static_cast<size_t>(itemsCount); ++i) {
                if (state.inventoryItems[i].isEquippedWeapon) {
                    inv.equipWeapon(static_cast<int>(i));
                    break;
                }
            }
        }

        if (state.equippedArmorIndex >= 0 && state.equippedArmorIndex < itemsCount) {
            inv.equipArmor(state.equippedArmorIndex);
        } else {
            for (size_t i = 0; i < state.inventoryItems.size() && i < static_cast<size_t>(itemsCount); ++i) {
                if (state.inventoryItems[i].isEquippedArmor) {
                    inv.equipArmor(static_cast<int>(i));
                    break;
                }
            }
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception applying snapshot: " << e.what() << "\n";
        return false;
    }
}

bool SaveManager::saveGameState(int slot, const SavedGameState& state) const {
    ensureDirectoryExists(saveDirectory);
    std::string filePath = getSlotFilePath(slot);

    try {
        json j;
        j["version"] = "1.0";

        // Flat root attributes for compatibility
        j["heroName"] = state.heroName;
        j["heroClass"] = static_cast<int>(state.heroClass);
        j["level"] = state.level;
        j["exp"] = state.exp;
        j["hp"] = state.hp;
        j["maxHp"] = state.maxHp;
        j["mp"] = state.mp;
        j["maxMp"] = state.maxMp;
        j["attack"] = state.attack;
        j["defense"] = state.defense;
        j["armorPenetration"] = state.armorPenetration;
        j["critChance"] = state.critChance;
        j["critDamage"] = state.critDamage;
        j["ignoreArmor"] = state.ignoreArmor;
        j["readyArrows"] = state.readyArrows;
        j["gold"] = state.gold;
        j["currentStoryNodeId"] = state.currentStoryNodeId;
        j["equippedWeaponIndex"] = state.equippedWeaponIndex;
        j["equippedArmorIndex"] = state.equippedArmorIndex;

        // Structured Hero object
        json heroJ;
        heroJ["name"] = state.heroName;
        heroJ["class"] = static_cast<int>(state.heroClass);
        heroJ["className"] = (state.heroClass == HeroClass::WARRIOR ? "WARRIOR" :
                              (state.heroClass == HeroClass::MAGE ? "MAGE" : "RANGER"));
        heroJ["level"] = state.level;
        heroJ["exp"] = state.exp;
        heroJ["hp"] = state.hp;
        heroJ["maxHp"] = state.maxHp;
        heroJ["mp"] = state.mp;
        heroJ["maxMp"] = state.maxMp;
        heroJ["attack"] = state.attack;
        heroJ["defense"] = state.defense;
        heroJ["armorPenetration"] = state.armorPenetration;
        heroJ["critChance"] = state.critChance;
        heroJ["critDamage"] = state.critDamage;
        heroJ["ignoreArmor"] = state.ignoreArmor;
        heroJ["readyArrows"] = state.readyArrows;
        heroJ["gold"] = state.gold;

        json statusJ;
        statusJ["poisonTurns"] = state.poisonTurns;
        statusJ["poisonDamagePerTurn"] = state.poisonDamagePerTurn;
        statusJ["regenTurns"] = state.regenTurns;
        statusJ["regenPerTurn"] = state.regenPerTurn;
        heroJ["statusEffects"] = statusJ;
        heroJ["skillCooldowns"] = state.skillCooldowns;
        j["hero"] = heroJ;

        // Story object
        json storyJ;
        storyJ["currentNodeId"] = state.currentStoryNodeId;
        json flagsJ = json::object();
        for (const auto& pair : state.storyFlags) {
            flagsJ[pair.first] = pair.second;
        }
        storyJ["flags"] = flagsJ;
        j["story"] = storyJ;
        j["storyFlags"] = flagsJ; // Root fallback

        // Inventory
        json invJ;
        invJ["capacity"] = state.inventoryCapacity;
        invJ["equippedWeaponIndex"] = state.equippedWeaponIndex;
        invJ["equippedArmorIndex"] = state.equippedArmorIndex;

        json itemsJson = json::array();
        for (const auto& rec : state.inventoryItems) {
            json itemJ;
            itemJ["id"] = rec.id;
            itemJ["name"] = rec.name;
            itemJ["description"] = rec.description;
            itemJ["type"] = rec.type;
            itemJ["statValue"] = rec.statValue;
            itemJ["quantity"] = rec.quantity;
            itemJ["isManaPotion"] = rec.isManaPotion;
            itemJ["isEquippedWeapon"] = rec.isEquippedWeapon;
            itemJ["isEquippedArmor"] = rec.isEquippedArmor;
            itemsJson.push_back(itemJ);
        }
        invJ["items"] = itemsJson;
        j["inventory"] = itemsJson; // Array for backwards compatibility
        j["inventoryState"] = invJ;

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

bool SaveManager::loadGameState(int slot, SavedGameState& outState) const {
    std::string filePath = getSlotFilePath(slot);
    if (!fs::exists(filePath)) {
        std::cerr << "[SaveManager] Error: File does not exist: " << filePath << "\n";
        return false;
    }

    try {
        std::ifstream inFile(filePath);
        if (!inFile.is_open()) {
            std::cerr << "[SaveManager] Error: Could not open file for reading: " << filePath << "\n";
            return false;
        }

        json j;
        inFile >> j;
        inFile.close();

        // 1. Read Hero attributes (supports both nested and flat structure)
        const json* heroPtr = j.contains("hero") && j["hero"].is_object() ? &j["hero"] : &j;

        outState.heroName = heroPtr->value("name", j.value("heroName", "Hero"));
        int classVal = heroPtr->value("class", j.value("heroClass", 0));
        outState.heroClass = static_cast<HeroClass>(classVal);
        outState.level = heroPtr->value("level", j.value("level", 1));
        outState.exp = heroPtr->value("exp", j.value("exp", 0));
        outState.maxHp = heroPtr->value("maxHp", j.value("maxHp", 100));
        outState.maxMp = heroPtr->value("maxMp", j.value("maxMp", 50));
        outState.hp = heroPtr->value("hp", j.value("hp", outState.maxHp));
        outState.mp = heroPtr->value("mp", j.value("mp", outState.maxMp));
        outState.attack = heroPtr->value("attack", j.value("attack", 10));
        outState.defense = heroPtr->value("defense", j.value("defense", 5));
        outState.armorPenetration = heroPtr->value("armorPenetration", j.value("armorPenetration", 0));
        outState.critChance = heroPtr->value("critChance", j.value("critChance", 0.0f));
        outState.critDamage = heroPtr->value("critDamage", j.value("critDamage", 0.0f));
        outState.ignoreArmor = heroPtr->value("ignoreArmor", j.value("ignoreArmor", false));
        outState.readyArrows = heroPtr->value("readyArrows", j.value("readyArrows", 0));
        outState.gold = heroPtr->value("gold", j.value("gold", 0));

        // Read status effects
        if (heroPtr->contains("statusEffects") && (*heroPtr)["statusEffects"].is_object()) {
            const auto& st = (*heroPtr)["statusEffects"];
            outState.poisonTurns = st.value("poisonTurns", 0);
            outState.poisonDamagePerTurn = st.value("poisonDamagePerTurn", 0);
            outState.regenTurns = st.value("regenTurns", 0);
            outState.regenPerTurn = st.value("regenPerTurn", 0);
        }

        // Read cooldowns
        if (heroPtr->contains("skillCooldowns") && (*heroPtr)["skillCooldowns"].is_array()) {
            outState.skillCooldowns = (*heroPtr)["skillCooldowns"].get<std::vector<int>>();
        }

        // 2. Read Story progress
        if (j.contains("story") && j["story"].is_object()) {
            outState.currentStoryNodeId = j["story"].value("currentNodeId", j.value("currentStoryNodeId", ""));
            if (j["story"].contains("flags") && j["story"]["flags"].is_object()) {
                for (auto& item : j["story"]["flags"].items()) {
                    outState.storyFlags[item.key()] = item.value().get<bool>();
                }
            }
        } else {
            outState.currentStoryNodeId = j.value("currentStoryNodeId", "");
        }

        if (j.contains("storyFlags") && j["storyFlags"].is_object()) {
            for (auto& item : j["storyFlags"].items()) {
                outState.storyFlags[item.key()] = item.value().get<bool>();
            }
        }

        // 3. Read Inventory state
        outState.equippedWeaponIndex = j.value("equippedWeaponIndex", -1);
        outState.equippedArmorIndex = j.value("equippedArmorIndex", -1);
        outState.inventoryCapacity = 20;

        const json* itemsArrayPtr = nullptr;
        if (j.contains("inventoryState") && j["inventoryState"].is_object()) {
            const auto& invSt = j["inventoryState"];
            outState.inventoryCapacity = invSt.value("capacity", 20);
            outState.equippedWeaponIndex = invSt.value("equippedWeaponIndex", outState.equippedWeaponIndex);
            outState.equippedArmorIndex = invSt.value("equippedArmorIndex", outState.equippedArmorIndex);
            if (invSt.contains("items") && invSt["items"].is_array()) {
                itemsArrayPtr = &invSt["items"];
            }
        }

        if (!itemsArrayPtr && j.contains("inventory") && j["inventory"].is_array()) {
            itemsArrayPtr = &j["inventory"];
        }

        outState.inventoryItems.clear();
        if (itemsArrayPtr) {
            for (const auto& itemJ : *itemsArrayPtr) {
                SavedItemRecord rec;
                rec.id = itemJ.value("id", "");
                rec.name = itemJ.value("name", "");
                rec.description = itemJ.value("description", "");
                rec.type = itemJ.value("type", 0);
                rec.statValue = itemJ.value("statValue", 0);
                rec.quantity = itemJ.value("quantity", 1);
                rec.isManaPotion = itemJ.value("isManaPotion", false);
                rec.isEquippedWeapon = itemJ.value("isEquippedWeapon", false);
                rec.isEquippedArmor = itemJ.value("isEquippedArmor", false);
                outState.inventoryItems.push_back(rec);
            }
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Exception during load: " << e.what() << "\n";
        return false;
    }
}

bool SaveManager::saveGame(int slot, const Hero& hero, const StoryGraph& story) {
    SavedGameState state = createSnapshot(hero, story);
    return saveGameState(slot, state);
}

bool SaveManager::loadGame(int slot, Hero& hero, StoryGraph& story) {
    if (!slotExists(slot)) {
        std::cerr << "[SaveManager] Error: Slot " << slot << " does not exist.\n";
        return false;
    }

    SavedGameState state;
    if (!loadGameState(slot, state)) {
        return false;
    }

    return applySnapshot(state, hero, story);
}

bool SaveManager::slotExists(int slot) const {
    std::string filePath = getSlotFilePath(slot);
    try {
        if (!fs::exists(filePath)) {
            return false;
        }
        return fs::file_size(filePath) > 0;
    } catch (...) {
        std::ifstream inFile(filePath);
        return inFile.good();
    }
}

bool SaveManager::deleteSlot(int slot) {
    if (!slotExists(slot)) {
        return false;
    }
    std::string filePath = getSlotFilePath(slot);
    try {
        return fs::remove(filePath);
    } catch (...) {
        return std::remove(filePath.c_str()) == 0;
    }
}

std::vector<int> SaveManager::getExistingSlots() const {
    std::set<int> slots;

    try {
        if (fs::exists(saveDirectory) && fs::is_directory(saveDirectory)) {
            std::regex slotRegex(R"(slot(\d+)\.json)", std::regex_constants::icase);
            for (const auto& entry : fs::directory_iterator(saveDirectory)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    std::smatch match;
                    if (std::regex_match(filename, match, slotRegex)) {
                        try {
                            int slotNum = std::stoi(match[1].str());
                            slots.insert(slotNum);
                        } catch (...) {}
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Error reading directory: " << e.what() << "\n";
    }

    // Fallback checks for common slot numbers 1..9
    for (int i = 1; i <= 9; ++i) {
        if (slotExists(i)) {
            slots.insert(i);
        }
    }

    return std::vector<int>(slots.begin(), slots.end());
}

bool SaveManager::isSlotCorrupted(int slot) const {
    if (!slotExists(slot)) return false;
    SavedGameState dummy;
    return !loadGameState(slot, dummy);
}

bool SaveManager::getSlotPreview(int slot, SavedGameState& outState) const {
    return loadGameState(slot, outState);
}
