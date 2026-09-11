/**
 * @file Minion.cpp
 * @brief Implementation of Minion base class, subclasses, and MinionFactory.
 */

#include "Minion.h"
#include <fstream>
#include <iostream>
#include <algorithm>

std::string minionTypeToString(MinionType type) {
    switch (type) {
        case MinionType::GOBLIN: return "GOBLIN";
        case MinionType::SKELETON: return "SKELETON";
        case MinionType::ORC: return "ORC";
        case MinionType::DARK_KNIGHT: return "DARK_KNIGHT";
        default: return "CUSTOM";
    }
}

MinionType stringToMinionType(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "GOBLIN") return MinionType::GOBLIN;
    if (s == "SKELETON") return MinionType::SKELETON;
    if (s == "ORC") return MinionType::ORC;
    if (s == "DARK_KNIGHT" || s == "DARKKNIGHT") return MinionType::DARK_KNIGHT;
    return MinionType::UNKNOWN;
}

// ==================== Minion Base Class ====================

Minion::Minion(const std::string& id,
               const std::string& name,
               MinionType minionType,
               int hp,
               int attack,
               int defense,
               int expReward,
               int goldReward,
               const std::string& description,
               const std::string& specialSkill)
    : Enemy(name, EnemyType::MINION, hp, attack, defense, expReward, goldReward),
      id(id),
      minionType(minionType),
      description(description),
      specialSkillName(specialSkill),
      actionCounter(0) {}

int Minion::chooseAction() {
    actionCounter++;
    // Default minion behavior: use special skill every 3rd turn if available
    if (!specialSkillName.empty() && actionCounter % 3 == 0) {
        return 2;
    }
    return 1;
}

void Minion::displayStats() const {
    std::cout << "--- " << name << " [" << minionTypeToString(minionType) << "] ---\n";
    if (!description.empty()) {
        std::cout << description << "\n";
    }
    std::cout << "HP: " << hp << "/" << maxHp
              << " | ATK: " << attack
              << " | DEF: " << defense << "\n";
    if (!specialSkillName.empty()) {
        std::cout << "Special Skill: " << specialSkillName;
    }
    std::cout << " | EXP: " << expReward
              << " | Gold: " << goldReward << "\n";
}

std::string Minion::getSpecialSkillName() const {
    return specialSkillName;
}

std::string Minion::getId() const { return id; }
MinionType Minion::getMinionType() const { return minionType; }
std::string Minion::getDescription() const { return description; }

// ==================== Subclasses ====================

// 1. Goblin
Goblin::Goblin(const std::string& id,
               const std::string& name,
               int hp,
               int attack,
               int defense,
               int expReward,
               int goldReward,
               const std::string& description,
               const std::string& specialSkill)
    : Minion(id, name, MinionType::GOBLIN, hp, attack, defense, expReward, goldReward, description, specialSkill) {}

int Goblin::chooseAction() {
    actionCounter++;
    // Goblin is nimble and attacks rapidly with Quick Strike on even turns
    if (actionCounter % 2 == 0) {
        return 2;
    }
    return 1;
}

// 2. Skeleton
Skeleton::Skeleton(const std::string& id,
                   const std::string& name,
                   int hp,
                   int attack,
                   int defense,
                   int expReward,
                   int goldReward,
                   const std::string& description,
                   const std::string& specialSkill)
    : Minion(id, name, MinionType::SKELETON, hp, attack, defense, expReward, goldReward, description, specialSkill) {}

int Skeleton::chooseAction() {
    actionCounter++;
    // Skeleton prepares and pierces with bone strike on every 3rd turn
    if (actionCounter % 3 == 0) {
        return 2;
    }
    return 1;
}

// 3. Orc
Orc::Orc(const std::string& id,
         const std::string& name,
         int hp,
         int attack,
         int defense,
         int expReward,
         int goldReward,
         const std::string& description,
         const std::string& specialSkill)
    : Minion(id, name, MinionType::ORC, hp, attack, defense, expReward, goldReward, description, specialSkill) {}

int Orc::chooseAction() {
    actionCounter++;
    // If enraged (HP <= 50%), Orc uses Heavy Slam more frequently
    if (hp <= maxHp / 2) {
        if (actionCounter % 2 == 0) {
            return 2;
        }
    } else {
        if (actionCounter % 4 == 0) {
            return 2;
        }
    }
    return 1;
}

// 4. Dark Knight
DarkKnight::DarkKnight(const std::string& id,
                       const std::string& name,
                       int hp,
                       int attack,
                       int defense,
                       int expReward,
                       int goldReward,
                       const std::string& description,
                       const std::string& specialSkill)
    : Minion(id, name, MinionType::DARK_KNIGHT, hp, attack, defense, expReward, goldReward, description, specialSkill) {}

int DarkKnight::chooseAction() {
    actionCounter++;
    // Alternates strategic heavy attacks
    if (actionCounter % 2 == 0) {
        return 2;
    }
    return 1;
}

// ==================== MinionFactory ====================

std::shared_ptr<Minion> MinionFactory::createFromJsonObject(const std::string& id, const nlohmann::json& j) {
    std::string name = j.value("name", id);
    std::string typeStr = j.value("minionType", "");
    MinionType mType = stringToMinionType(typeStr);
    if (mType == MinionType::UNKNOWN) {
        mType = stringToMinionType(id);
    }

    int hp = j.value("hp", 50);
    int attack = j.value("attack", 10);
    int defense = j.value("defense", 5);
    int expReward = j.value("expReward", 30);
    int goldReward = j.value("goldReward", 10);
    std::string desc = j.value("description", "");
    std::string skill = j.value("specialSkill", "");

    switch (mType) {
        case MinionType::GOBLIN:
            return std::make_shared<Goblin>(id, name, hp, attack, defense, expReward, goldReward, desc, skill);
        case MinionType::SKELETON:
            return std::make_shared<Skeleton>(id, name, hp, attack, defense, expReward, goldReward, desc, skill);
        case MinionType::ORC:
            return std::make_shared<Orc>(id, name, hp, attack, defense, expReward, goldReward, desc, skill);
        case MinionType::DARK_KNIGHT:
            return std::make_shared<DarkKnight>(id, name, hp, attack, defense, expReward, goldReward, desc, skill);
        default:
            return std::make_shared<Minion>(id, name, mType, hp, attack, defense, expReward, goldReward, desc, skill);
    }
}

std::shared_ptr<Minion> MinionFactory::createFromJson(const std::string& id, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[MinionFactory] Failed to open " << filepath << ". Using default stats.\n";
        // Fallback default instances based on id
        if (id == "goblin") return std::make_shared<Goblin>();
        if (id == "skeleton") return std::make_shared<Skeleton>();
        if (id == "orc") return std::make_shared<Orc>();
        if (id == "dark_knight") return std::make_shared<DarkKnight>();
        return nullptr;
    }

    try {
        nlohmann::json data;
        file >> data;
        if (data.contains(id)) {
            return createFromJsonObject(id, data[id]);
        } else {
            std::cerr << "[MinionFactory] Minion ID '" << id << "' not found in " << filepath << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[MinionFactory] JSON parsing error: " << e.what() << "\n";
    }

    return nullptr;
}

std::vector<std::shared_ptr<Minion>> MinionFactory::loadAllFromJson(const std::string& filepath) {
    std::vector<std::shared_ptr<Minion>> minions;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[MinionFactory] Could not open file: " << filepath << "\n";
        return minions;
    }

    try {
        nlohmann::json data;
        file >> data;
        for (auto& [key, value] : data.items()) {
            std::string type = value.value("type", "MINION");
            if (type == "BOSS") {
                continue;
            }
            minions.push_back(createFromJsonObject(key, value));
        }
    } catch (const std::exception& e) {
        std::cerr << "[MinionFactory] JSON parse error in " << filepath << ": " << e.what() << "\n";
    }

    return minions;
}

nlohmann::json MinionFactory::minionToJson(const Minion& minion) {
    nlohmann::json j;
    j["id"] = minion.getId();
    j["name"] = minion.getName();
    j["type"] = "MINION";
    j["minionType"] = minionTypeToString(minion.getMinionType());
    j["hp"] = minion.getMaxHp();
    j["attack"] = minion.getAttack();
    j["defense"] = minion.getDefense();
    j["expReward"] = minion.getExpReward();
    j["goldReward"] = minion.getGoldReward();
    j["description"] = minion.getDescription();
    j["specialSkill"] = minion.getSpecialSkillName();
    return j;
}

bool MinionFactory::saveMinionConfig(const std::string& filepath, const std::vector<std::shared_ptr<Minion>>& minions) {
    nlohmann::json root;
    for (const auto& minion : minions) {
        if (minion) {
            root[minion->getId()] = minionToJson(*minion);
        }
    }

    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[MinionFactory] Could not open file for writing: " << filepath << "\n";
        return false;
    }

    file << root.dump(2) << std::endl;
    return true;
}
