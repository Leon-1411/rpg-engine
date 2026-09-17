#pragma once

#include "Hero.h"
#include "StoryGraph.h"
#include "Inventory.h"
#include <string>
#include <vector>
#include <unordered_map>

struct GameState {
    std::string heroName;
    HeroClass heroClass;
    int level;
    int exp;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    int attack;
    int defense;
    int gold;
    std::string currentStoryNodeId;
    std::vector<std::string> inventoryItemIds;
    int equippedWeaponIndex;
    int equippedArmorIndex;
    std::unordered_map<std::string, bool> storyFlags;
};

class SaveManager {
private:
    std::string saveDirectory;
    std::string getSlotFilePath(int slot) const;

public:
    explicit SaveManager(const std::string& saveDir = "saves/");
    ~SaveManager() = default;

    bool saveGame(int slot, const Hero& hero, const StoryGraph& story, const Inventory* inventory = nullptr);
    bool loadGame(int slot, Hero& hero, StoryGraph& story, Inventory* inventory = nullptr);
    bool slotExists(int slot) const;
    bool deleteSlot(int slot);
    std::vector<int> getExistingSlots() const;
    std::string getSaveDirectory() const;
};
