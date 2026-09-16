/**
 * @file BossMonster.cpp
 * @brief Implementation of BossMonster class and BossFactory with 3-skill cooldown system.
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
      actionCounter(0),
      activeSkillIndex(-1) {
    initializeDefaultSkills(specialSkill);
}

void BossMonster::initializeDefaultSkills(const std::string& primarySkill) {
    skills.clear();
    std::string ultimateSkill = primarySkill.empty() ? "Infernal Cataclysm" : primarySkill;

    if (ultimateSkill == "Infernal Cataclysm") {
        // Default Dragon Lord skill set
        skills.emplace_back("Flame Breath", "Scorches foes with an expansive cone of fire.", 2, 1.3, 2);
        skills.emplace_back("Dragon Roar", "Emits a concussive shockwave that shatters defenses.", 3, 1.6, 3);
        skills.emplace_back("Infernal Cataclysm", "Rains molten apocalyptic meteors upon the battlefield.", 5, 2.2, 5);
    } else {
        // Themed skills based on the provided primary skill name
        skills.emplace_back(ultimateSkill + " Strike", "A swift, concentrated blast of focused energy.", 2, 1.3, 2);
        skills.emplace_back(ultimateSkill + " Shockwave", "Releases a crushing shockwave across the surroundings.", 3, 1.6, 3);
        skills.emplace_back(ultimateSkill, "Unleashes the full apocalyptic potential of " + ultimateSkill + ".", 5, 2.2, 5);
    }
}

int BossMonster::chooseAction() {
    actionCounter++;
    checkEnrage();

    if (enraged) {
        // In enraged state, boss has slight healing capability
        regenerate();
    }

    // Progress cooldowns for all skills
    // In enraged mode, the boss's fury accelerates cooldown recovery
    updateCooldowns(enraged ? 2 : 1);

    // AI selection: prioritize Ultimate (index 2) -> Medium (index 1) -> Light (index 0)
    int chosenIndex = -1;
    for (int i = static_cast<int>(skills.size()) - 1; i >= 0; --i) {
        if (skills[i].isReady()) {
            chosenIndex = i;
            break;
        }
    }

    if (chosenIndex != -1) {
        skills[chosenIndex].trigger();
        activeSkillIndex = chosenIndex;
        return 2; // Unleash special skill
    }

    // All skills are currently on cooldown -> execute normal attack
    activeSkillIndex = -1;
    return 1;
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

        // Enrage refreshes ultimate skill cooldown to immediately threaten the party
        if (!skills.empty()) {
            skills.back().resetCooldown();
        }

        std::cout << "\n>>> [BOSS ENRAGE TRIGGERED] " << name
                  << "'s HP dropped below 30% (" << hp << "/" << maxHp << ")!\n"
                  << ">>> " << name << " enters Berserk mode: ATK increased to "
                  << attack << " (+50%), DEF increased to " << defense << " (+50%)!\n"
                  << ">>> " << name << "'s ultimate skill [" << skills.back().name << "] cooldown reset!\n\n";
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
    std::cout << "Heal Amount (Enraged Regen): " << healAmount << " HP/turn\n";
    std::cout << "EXP: " << expReward << " | Gold: " << goldReward << "\n";

    if (!skills.empty()) {
        std::cout << "Special Skills (with Cooldowns):\n";
        for (size_t i = 0; i < skills.size(); ++i) {
            std::cout << "  [" << (i + 1) << "] " << skills[i].name
                      << " (CD: " << skills[i].cooldown << " turns";
            if (skills[i].isReady()) {
                std::cout << " - READY)";
            } else {
                std::cout << " - " << skills[i].currentCooldown << " turns remaining)";
            }
            std::cout << " [x" << skills[i].damageMultiplier << " DMG] | " << skills[i].description << "\n";
        }
    } else if (!specialSkillName.empty()) {
        std::cout << "Special Skill: " << specialSkillName << "\n";
    }
}

std::string BossMonster::getSpecialSkillName() const {
    if (activeSkillIndex >= 0 && activeSkillIndex < static_cast<int>(skills.size())) {
        return skills[activeSkillIndex].name;
    }
    if (!skills.empty()) {
        return skills.back().name;
    }
    return specialSkillName;
}

const std::vector<BossSkill>& BossMonster::getSkills() const { return skills; }
std::vector<BossSkill>& BossMonster::getSkills() { return skills; }

const BossSkill* BossMonster::getSkill(size_t index) const {
    if (index < skills.size()) return &skills[index];
    return nullptr;
}

BossSkill* BossMonster::getSkill(size_t index) {
    if (index < skills.size()) return &skills[index];
    return nullptr;
}

bool BossMonster::isSkillReady(size_t index) const {
    if (index < skills.size()) return skills[index].isReady();
    return false;
}

int BossMonster::getSkillRemainingCooldown(size_t index) const {
    if (index < skills.size()) return skills[index].currentCooldown;
    return -1;
}

int BossMonster::getSkillBaseCooldown(size_t index) const {
    if (index < skills.size()) return skills[index].cooldown;
    return -1;
}

void BossMonster::setSkillCooldown(size_t index, int turns) {
    if (index < skills.size()) {
        skills[index].currentCooldown = std::max(0, turns);
    }
}

void BossMonster::updateCooldowns(int amount) {
    for (auto& s : skills) {
        s.updateCooldown(amount);
    }
}

void BossMonster::resetCooldowns() {
    for (auto& s : skills) {
        s.resetCooldown();
    }
}

bool BossMonster::useSkill(size_t index) {
    if (index < skills.size() && skills[index].isReady()) {
        skills[index].trigger();
        activeSkillIndex = static_cast<int>(index);
        return true;
    }
    return false;
}

void BossMonster::setSkills(const std::vector<BossSkill>& newSkills) {
    skills = newSkills;
}

void BossMonster::addSkill(const BossSkill& skill) {
    skills.push_back(skill);
}

int BossMonster::getActiveSkillIndex() const {
    return activeSkillIndex;
}

const BossSkill* BossMonster::getActiveSkill() const {
    if (activeSkillIndex >= 0 && activeSkillIndex < static_cast<int>(skills.size())) {
        return &skills[activeSkillIndex];
    }
    return nullptr;
}

double BossMonster::getActiveSkillMultiplier() const {
    const BossSkill* s = getActiveSkill();
    return s ? s->damageMultiplier : 1.0;
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

    auto boss = std::make_shared<BossMonster>(id, name, hp, attack, defense, expReward, goldReward, desc, skill, healAmount);

    if (j.contains("skills") && j["skills"].is_array() && !j["skills"].empty()) {
        std::vector<BossSkill> loadedSkills;
        for (const auto& sj : j["skills"]) {
            std::string sName = sj.value("name", "Special Skill");
            std::string sDesc = sj.value("description", "");
            int sCd = sj.value("cooldown", 3);
            double sMult = sj.value("damageMultiplier", 1.4);
            int sInitCd = sj.value("currentCooldown", sCd);
            loadedSkills.emplace_back(sName, sDesc, sCd, sMult, sInitCd);
        }
        boss->setSkills(loadedSkills);
    }

    return boss;
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

    nlohmann::json skillsJson = nlohmann::json::array();
    for (const auto& skill : boss.getSkills()) {
        nlohmann::json sj;
        sj["name"] = skill.name;
        sj["description"] = skill.description;
        sj["cooldown"] = skill.cooldown;
        sj["currentCooldown"] = skill.currentCooldown;
        sj["damageMultiplier"] = skill.damageMultiplier;
        skillsJson.push_back(sj);
    }
    j["skills"] = skillsJson;

    return j;
}
