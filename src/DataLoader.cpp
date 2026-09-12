/**
 * @file DataLoader.cpp
 * @brief Implement JSON parsing loaders for story, items, and enemies.
 */

#include "DataLoader.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool DataLoader::loadStory(const std::string& filePath, StoryGraph& storyGraph) {
    return storyGraph.loadStoryGraph(filePath);
}

std::vector<Item> DataLoader::loadItems(const std::string& filePath) {
    std::vector<Item> items;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[DataLoader] Lỗi: Không thể mở file items: " << filePath << "\n";
        return items;
    }

    try {
        json j;
        file >> j;
        file.close();

        auto parseItemObj = [](const std::string& id, const json& val) -> Item {
            std::string name = val.value("name", "Unknown Item");
            std::string desc = val.value("description", "");
            std::string typeStr = val.value("type", "POTION");
            int statValue = val.value("statValue", 0);

            ItemType type = ItemType::POTION;
            if (typeStr == "WEAPON") type = ItemType::WEAPON;
            else if (typeStr == "ARMOR") type = ItemType::ARMOR;
            else if (typeStr == "POTION") type = ItemType::POTION;

            return Item(id, name, desc, type, statValue);
        };

        if (j.is_object()) {
            if (j.contains("items") && j["items"].is_array()) {
                for (const auto& el : j["items"]) {
                    std::string id = el.value("id", "item_unknown");
                    items.push_back(parseItemObj(id, el));
                }
            } else {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    if (it.value().is_object()) {
                        items.push_back(parseItemObj(it.key(), it.value()));
                    }
                }
            }
        } else if (j.is_array()) {
            for (const auto& el : j) {
                std::string id = el.value("id", "item_unknown");
                items.push_back(parseItemObj(id, el));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[DataLoader] Lỗi parse JSON items: " << e.what() << "\n";
    }

    return items;
}

std::vector<Enemy> DataLoader::loadEnemies(const std::string& filePath) {
    std::vector<Enemy> enemies;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[DataLoader] Lỗi: Không thể mở file enemies: " << filePath << "\n";
        return enemies;
    }

    try {
        json j;
        file >> j;
        file.close();

        auto parseEnemyObj = [](const json& val) -> Enemy {
            std::string name = val.value("name", "Unnamed Enemy");
            std::string typeStr = val.value("type", "MINION");
            EnemyType type = (typeStr == "BOSS") ? EnemyType::BOSS : EnemyType::MINION;

            int hp = val.value("hp", 50);
            int attack = val.value("attack", 10);
            int defense = val.value("defense", 2);
            int expReward = val.value("expReward", 25);
            int goldReward = val.value("goldReward", 10);

            return Enemy(name, type, hp, attack, defense, expReward, goldReward);
        };

        if (j.is_object()) {
            if (j.contains("enemies") && j["enemies"].is_array()) {
                for (const auto& el : j["enemies"]) {
                    enemies.push_back(parseEnemyObj(el));
                }
            } else {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    if (it.value().is_object()) {
                        enemies.push_back(parseEnemyObj(it.value()));
                    }
                }
            }
        } else if (j.is_array()) {
            for (const auto& el : j) {
                enemies.push_back(parseEnemyObj(el));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[DataLoader] Lỗi parse JSON enemies: " << e.what() << "\n";
    }

    return enemies;
}
