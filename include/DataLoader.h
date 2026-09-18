#pragma once

#include "StoryGraph.h"
#include "Item.h"
#include "Enemy.h"
#include <string>
#include <vector>
#include <memory>

class DataLoader {
public:
    // Load story nodes into StoryGraph
    static bool loadStory(const std::string& filePath, StoryGraph& storyGraph);

    // Load items list as polymorphic shared pointers from JSON file
    static std::vector<std::shared_ptr<Item>> loadItemPointers(const std::string& filePath);

    // Load single item by ID from JSON file
    static std::shared_ptr<Item> loadItemById(const std::string& filePath, const std::string& itemId);

    // Load items list from JSON file
    static std::vector<Item> loadItems(const std::string& filePath);

    // Load enemies list from JSON file
    static std::vector<Enemy> loadEnemies(const std::string& filePath);
};

