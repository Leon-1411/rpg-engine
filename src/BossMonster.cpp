/**
 * @file BossMonster.cpp
 * @brief Implementation of BossMonster class and BossFactory.
 */

#include "BossMonster.h"
#include "Minion.h"
#include <fstream>
#include <iostream>
#include <algorithm>

BossMonster::BossMonster(const std::string& id,
                         const std::string& name,
                         int hp,
                         int attack,
                         int defense,
                         int expReward,
                         int goldReward,
                         const std::string& description,
                         const std::string& specialSkill,
                         int healAmount)
    : Enemy(name, EnemyType::BOSS, hp, attack, defense, expReward, goldReward),
      id(id),
      description(description),
      specialSkillName(specialSkill),
      baseAttack(attack),
      baseDefense(defense),
      enraged(false),
      healAmount(healAmount),
      actionCounter(0) {}

int BossMonster::chooseAction() {
    actionCounter++;
    checkEnrage();

    if (enraged) {
        // In enraged state, boss has slight healing capability
        regenerate();

        // Enraged combat pattern: higher skill frequency
        if (actionCounter % 2 == 0) {
            return 2; // Unleash empowered special skill
        }
        return 1;
    } else {
        // Standard combat pattern: skill every 3rd turn
        if (!specialSkillName.empty() && actionCounter % 3 == 0) {
            return 2;
        }
        return 1;
    }
}

void BossMonster::takeDamage(int damage) {
    Enemy::takeDamage(damage);
    if (isAlive()) {
        checkEnrage();
    }
}

bool BossMonster::checkEnrage() {
    if (!enraged && hp > 0 && hp < static_cast<int>(maxHp * 0.30)) {
        triggerEnrage();
        return true;
    }
    return enraged;
}

void BossMonster::triggerEnrage() {
    if (!enraged) {
        enraged = true;
        attack = static_cast<int>(baseAttack * 1.5);
        defense = static_cast<int>(baseDefense * 1.5);

        std::cout << "\n>>> [BOSS ENRAGE TRIGGERED] " << name
                  << "'s HP dropped below 30% (" << hp << "/" << maxHp << ")!\n"
                  << ">>> " << name << " enters Berserk mode: ATK increased to "
                  << attack << " (+50%), DEF increased to " << defense << " (+50%)!\n\n";
    }
}

void BossMonster::heal(int amount) {
    if (!isAlive() || amount <= 0) return;
    hp = std::min(maxHp, hp + amount);
}

void BossMonster::regenerate() {
    if (!isAlive()) return;
    int prevHp = hp;
    heal(healAmount);
    int recovered = hp - prevHp;
    if (recovered > 0) {
        std::cout << ">>> [Boss Recovery] " << name << " activates slight recovery and heals for "
                  << recovered << " HP! (" << hp << "/" << maxHp << ")\n";
    }
}

void BossMonster::resetEnrage() {
    enraged = false;
    attack = baseAttack;
    defense = baseDefense;
}

void BossMonster::displayStats() const {
    std::cout << "=== [BOSS] " << name << (enraged ? " (ENRAGED)" : "") << " ===\n";
    if (!description.empty()) {
        std::cout << description << "\n";
    }
    std::cout << "HP: " << hp << "/" << maxHp
              << " | ATK: " << attack << (enraged ? " (+50% Berserk)" : "")
              << " | DEF: " << defense << (enraged ? " (+50% Berserk)" : "") << "\n";
    if (!specialSkillName.empty()) {
        std::cout << "Special Skill: " << specialSkillName << "\n";
    }
    std::cout << "Heal Amount (Enraged Regen): " << healAmount << " HP/turn\n";
    std::cout << "EXP: " << expReward << " | Gold: " << goldReward << "\n";
}

std::string BossMonster::getSpecialSkillName() const {
    return specialSkillName;
}

std::string BossMonster::getId() const { return id; }
std::string BossMonster::getDescription() const { return description; }
bool BossMonster::isEnraged() const { return enraged; }
int BossMonster::getBaseAttack() const { return baseAttack; }
int BossMonster::getBaseDefense() const { return baseDefense; }
int BossMonster::getHealAmount() const { return healAmount; }
void BossMonster::setHealAmount(int amount) { healAmount = amount; }

std::shared_ptr<BossMonster> BossMonster::createScaledFromMinion(
    const Minion& minion,
    const std::string& bossId,
    const std::string& bossName,
    const std::string& specialSkill,
    int healAmount) {
    std::string id = bossId.empty() ? ("boss_" + minion.getId()) : bossId;
    std::string name = bossName.empty() ? (minion.getName() + " Overlord") : bossName;
    std::string skill = specialSkill.empty() ? (minion.getSpecialSkillName() + " Cataclysm") : specialSkill;

    // Boss stats are configured to be 5 times stronger than the minion
    int hp = minion.getMaxHp() * 5;
    int attack = minion.getAttack() * 5;
    int defense = minion.getDefense() * 5;
    int exp = minion.getExpReward() * 5;
    int gold = minion.getGoldReward() * 5;
    int heal = (healAmount > 0) ? healAmount : std::max(5, static_cast<int>(hp * 0.05));

    std::string desc = "A terrifying Boss entity possessing 5x the strength of " + minion.getName() + ".";

    return std::make_shared<BossMonster>(id, name, hp, attack, defense, exp, gold, desc, skill, heal);
}

// ==================== BossFactory ====================

std::shared_ptr<BossMonster> BossFactory::createFromJsonObject(const std::string& id, const nlohmann::json& j) {
    std::string name = j.value("name", id);
    int hp = j.value("hp", 525);
    int attack = j.value("attack", 105);
    int defense = j.value("defense", 50);
    int expReward = j.value("expReward", 450);
    int goldReward = j.value("goldReward", 150);
    std::string desc = j.value("description", "");
    std::string skill = j.value("specialSkill", "Infernal Cataclysm");
    int healAmount = j.value("healAmount", 25);

    return std::make_shared<BossMonster>(id, name, hp, attack, defense, expReward, goldReward, desc, skill, healAmount);
}

std::shared_ptr<BossMonster> BossFactory::createFromJson(const std::string& id, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[BossFactory] Failed to open " << filepath << ". Using default stats.\n";
        return std::make_shared<BossMonster>();
    }

    try {
        nlohmann::json data;
        file >> data;
        if (data.contains(id)) {
            return createFromJsonObject(id, data[id]);
        } else {
            std::cerr << "[BossFactory] Boss ID '" << id << "' not found in " << filepath << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[BossFactory] JSON parsing error: " << e.what() << "\n";
    }

    return nullptr;
}

std::vector<std::shared_ptr<BossMonster>> BossFactory::loadAllFromJson(const std::string& filepath) {
    std::vector<std::shared_ptr<BossMonster>> bosses;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[BossFactory] Could not open file: " << filepath << "\n";
        return bosses;
    }

    try {
        nlohmann::json data;
        file >> data;
        for (auto& [key, value] : data.items()) {
            std::string type = value.value("type", "");
            if (type == "BOSS") {
                bosses.push_back(createFromJsonObject(key, value));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[BossFactory] JSON parse error in " << filepath << ": " << e.what() << "\n";
    }

    return bosses;
}

nlohmann::json BossFactory::bossToJson(const BossMonster& boss) {
    nlohmann::json j;
    j["id"] = boss.getId();
    j["name"] = boss.getName();
    j["type"] = "BOSS";
    j["hp"] = boss.getMaxHp();
    j["attack"] = boss.getBaseAttack();
    j["defense"] = boss.getBaseDefense();
    j["expReward"] = boss.getExpReward();
    j["goldReward"] = boss.getGoldReward();
    j["description"] = boss.getDescription();
    j["specialSkill"] = boss.getSpecialSkillName();
    j["healAmount"] = boss.getHealAmount();
    return j;
}
