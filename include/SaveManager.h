#pragma once

#include "Hero.h"
#include "StoryGraph.h"
#include <string>
#include <vector>

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
    std::string currentStoryNodeId;
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
