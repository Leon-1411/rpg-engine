#pragma once

#include "Hero.h"
#include "StoryGraph.h"
#include <string>
#include <vector>
#include <unordered_map>

struct SavedItemRecord {
    std::string id;
    std::string name;
    std::string description;
    int type = 0; // 0: WEAPON, 1: ARMOR, 2: POTION
    int statValue = 0;
    int quantity = 1;
    bool isManaPotion = false;
    bool isEquippedWeapon = false;
    bool isEquippedArmor = false;
};

struct SavedGameState {
    // Hero attributes
    std::string heroName = "Hero";
    HeroClass heroClass = HeroClass::WARRIOR;
    int level = 1;
    int exp = 0;
    int hp = 100;
    int maxHp = 100;
    int mp = 50;
    int maxMp = 50;
    int attack = 10;
    int defense = 5;
    int armorPenetration = 0;
    float critChance = 0.0f;
    float critDamage = 0.0f;
    bool ignoreArmor = false;
    int readyArrows = 0;
    int gold = 0;

    // Status effects
    int poisonTurns = 0;
    int poisonDamagePerTurn = 0;
    int regenTurns = 0;
    int regenPerTurn = 0;

    // Combat cooldowns
    std::vector<int> skillCooldowns;

    // Story progress
    std::string currentStoryNodeId;
    std::unordered_map<std::string, bool> storyFlags;

    // Inventory state
    int inventoryCapacity = 20;
    std::vector<SavedItemRecord> inventoryItems;
    int equippedWeaponIndex = -1;
    int equippedArmorIndex = -1;
};

class SaveManager {
private:
    std::string saveDirectory;

public:
    explicit SaveManager(const std::string& saveDir = "saves/");
    ~SaveManager() = default;

    std::string getSlotFilePath(int slot) const;
    std::string getSaveDirectory() const;

    // Snapshot creation and restoration
    SavedGameState createSnapshot(const Hero& hero, const StoryGraph& story) const;
    bool applySnapshot(const SavedGameState& state, Hero& hero, StoryGraph& story) const;

    // High-level GameState Save/Load
    bool saveGameState(int slot, const SavedGameState& state) const;
    bool loadGameState(int slot, SavedGameState& outState) const;

    // Core Save/Load API (Compatible with GameManager, MainMenu, tests)
    bool saveGame(int slot, const Hero& hero, const StoryGraph& story);
    bool loadGame(int slot, Hero& hero, StoryGraph& story);

    // Slot Management
    bool slotExists(int slot) const;
    bool deleteSlot(int slot);
    std::vector<int> getExistingSlots() const;
    bool isSlotCorrupted(int slot) const;
    bool getSlotPreview(int slot, SavedGameState& outState) const;
};
