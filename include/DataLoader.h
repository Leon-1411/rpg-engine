#pragma once

#include "StoryGraph.h"
#include "Item.h"
#include "Enemy.h"
#include <string>
#include <vector>

class DataLoader {
public:
    // Load story nodes into StoryGraph
    static bool loadStory(const std::string& filePath, StoryGraph& storyGraph);

    // Load items list from JSON file
    static std::vector<Item> loadItems(const std::string& filePath);

    // Load enemies list from JSON file
    static std::vector<Enemy> loadEnemies(const std::string& filePath);
};
