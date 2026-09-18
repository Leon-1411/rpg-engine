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

static std::shared_ptr<Item> parseItemObject(const std::string& id, const json& val) {
    std::string name = val.value("name", "Unknown Item");
    std::string desc = val.value("description", "");
    std::string typeStr = val.value("type", "POTION");
    int statValue = val.value("statValue", 0);

    if (typeStr == "WEAPON") {
        int atk = val.value("attackBonus", statValue);
        return std::make_shared<Weapon>(id, name, desc, atk);
    } else if (typeStr == "ARMOR") {
        int def = val.value("defenseBonus", statValue);
        return std::make_shared<Armor>(id, name, desc, def);
    } else if (typeStr == "POTION") {
        int heal = val.value("healAmount", statValue);
        bool isMana = val.value("isMana", false);
        int qty = val.value("quantity", 1);
        return std::make_shared<Potion>(id, name, desc, heal, isMana, qty);
    } else if (typeStr == "KEY_ITEM") {
        return std::make_shared<KeyItem>(id, name, desc);
    }

    return std::make_shared<Item>(id, name, desc, ItemType::POTION, statValue);
}

std::vector<std::shared_ptr<Item>> DataLoader::loadItemPointers(const std::string& filePath) {
    std::vector<std::shared_ptr<Item>> items;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[DataLoader] Lỗi: Không thể mở file items: " << filePath << "\n";
        return items;
    }

    try {
        json j;
        file >> j;
        file.close();

        if (j.is_object()) {
            if (j.contains("items") && j["items"].is_array()) {
                for (const auto& el : j["items"]) {
                    std::string id = el.value("id", "item_unknown");
                    items.push_back(parseItemObject(id, el));
                }
            } else {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    if (it.value().is_object()) {
                        items.push_back(parseItemObject(it.key(), it.value()));
                    }
                }
            }
        } else if (j.is_array()) {
            for (const auto& el : j) {
                std::string id = el.value("id", "item_unknown");
                items.push_back(parseItemObject(id, el));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[DataLoader] Lỗi parse JSON items: " << e.what() << "\n";
    }

    return items;
}

std::shared_ptr<Item> DataLoader::loadItemById(const std::string& filePath, const std::string& itemId) {
    auto items = loadItemPointers(filePath);
    for (const auto& item : items) {
        if (item && item->getId() == itemId) {
            return item->clone();
        }
    }
    return nullptr;
}

std::vector<Item> DataLoader::loadItems(const std::string& filePath) {
    std::vector<Item> result;
    auto itemPtrs = loadItemPointers(filePath);
    for (const auto& ptr : itemPtrs) {
        if (ptr) {
            result.push_back(*ptr);
        }
    }
    return result;
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

            Enemy enemy(name, type, hp, attack, defense, expReward, goldReward);
            if (val.contains("dropItems") && val["dropItems"].is_array()) {
                std::vector<std::string> drops;
                for (const auto& itm : val["dropItems"]) {
                    drops.push_back(itm.get<std::string>());
                }
                enemy.setDropItemIds(drops);
            }
            if (val.contains("dropChance")) {
                enemy.setDropChance(val["dropChance"].get<float>());
            }
            return enemy;
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
