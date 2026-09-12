#pragma once

#include "Hero.h"
#include "StoryGraph.h"
#include <string>
#include <vector>

struct SavedItemRecord {
    std::string id;
    std::string name;
    std::string description;
    int type; // 0: WEAPON, 1: ARMOR, 2: POTION
    int statValue;
    int quantity;
    bool isEquippedWeapon;
    bool isEquippedArmor;
};

struct GameState {
    std::string heroName;
    HeroClass heroClass;
    int level;
    int exp;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int armorPenetration;
    float critChance;
    float critDamage;
    std::string currentStoryNodeId;

    // Trạng thái mảng Inventory
    std::vector<SavedItemRecord> inventoryItems;
    int equippedWeaponIndex = -1;
    int equippedArmorIndex = -1;
};


class SaveManager {
private:
    std::string saveDirectory;
    std::string getSlotFilePath(int slot) const;

public:
    explicit SaveManager(const std::string& saveDir = "saves/");
    ~SaveManager() = default;

    bool saveGame(int slot, const Hero& hero, const StoryGraph& story);
    bool loadGame(int slot, Hero& hero, StoryGraph& story);
    bool slotExists(int slot) const;
    bool deleteSlot(int slot);
    std::vector<int> getExistingSlots() const;
    std::string getSaveDirectory() const;
};
