/**
 * @file Hero.cpp
 * @brief Implement Hero base class methods, polymorphic factory, and combat stats.
 * @author Quý & Antigravity
 */

#include "Hero.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "LevelSystem.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>

std::shared_ptr<Hero> Hero::createHero(HeroClass heroClass, const std::string& name,
                                       int hp, int mp, int attack, int defense) {
    if (hp > 0 && mp > 0 && attack > 0 && defense > 0) {
        switch (heroClass) {
            case HeroClass::WARRIOR:
                return std::make_shared<Warrior>(name, hp, mp, attack, defense);
            case HeroClass::MAGE:
                return std::make_shared<Mage>(name, hp, mp, attack, defense);
            case HeroClass::RANGER:
                return std::make_shared<Ranger>(name, hp, mp, attack, defense);
        }
    }

    switch (heroClass) {
        case HeroClass::WARRIOR:
            return std::make_shared<Warrior>(name);
        case HeroClass::MAGE:
            return std::make_shared<Mage>(name);
        case HeroClass::RANGER:
            return std::make_shared<Ranger>(name);
    }
    return std::make_shared<Warrior>(name);
}

Hero::Hero(const std::string& name, HeroClass heroClass)
    : name(name), heroClass(heroClass), level(1), exp(0), gold(0), statPoints(0),
      isParrying(false), isBlocking(false), isEvading(false), isDefending(false), skillLockTurns(0),
      poisonTurns(0), poisonDamagePerTurn(0), regenTurns(0), regenPerTurn(0) {
    
    skillCooldowns = {0, 0, 0};

    switch (heroClass) {
        case HeroClass::WARRIOR:
            // High HP, High DEF, Melee Power
            maxHp = hp = 120;
            maxMp = mp = 30;
            attack = 18;
            defense = 8;
            armorPenetration = 10;
            critChance = 0.10f;
            critDamage = 0.25f;
            ignoreArmor = false;
            readyArrows = 0;
            maxCooldowns = {0, 0, 0};
            break;

        case HeroClass::RANGER:
            // High Agility, High Crit, Ranged Tactics
            maxHp = hp = 95;
            maxMp = mp = 50;
            attack = 20;
            defense = 5;
            armorPenetration = 5;
            critChance = 0.25f;
            critDamage = 0.50f;
            ignoreArmor = false;
            readyArrows = 2; // Initial ready arrows
            maxCooldowns = {0, 0, 0};
            break;

        case HeroClass::MAGE:
            // High MP, High Magic ATK, Ignores Armor
            maxHp = hp = 80;
            maxMp = mp = 100;
            attack = 24;
            defense = 3;
            armorPenetration = 0;
            critChance = 0.0f;
            critDamage = 0.0f;
            ignoreArmor = true;
            readyArrows = 0;
            maxCooldowns = {0, 0, 0};
            break;
    }
}

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int attack, int defense)
    : Hero(name, heroClass, hp, 50, attack, defense, 10, 0.25f, 0.25f, false) {}

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int attack, int defense,
           int armorPen, float critChance, float critDamage, bool ignoreArmor)
    : name(name), heroClass(heroClass), level(1), exp(0), gold(0), statPoints(0), hp(hp), maxHp(hp), mp(50), maxMp(50),
      attack(attack), defense(defense), armorPenetration(armorPen),
      critChance(critChance), critDamage(critDamage), ignoreArmor(ignoreArmor),
      readyArrows(heroClass == HeroClass::RANGER ? 2 : 0),
      skillCooldowns{0, 0, 0}, maxCooldowns{0, 0, 0},
      isParrying(false), isBlocking(false), isEvading(false), isDefending(false), skillLockTurns(0),
      poisonTurns(0), poisonDamagePerTurn(0), regenTurns(0), regenPerTurn(0) {}

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int mp, int attack, int defense,
           int armorPen, float critChance, float critDamage, bool ignoreArmor)
    : name(name), heroClass(heroClass), level(1), exp(0), gold(0), statPoints(0), hp(hp), maxHp(hp), mp(mp), maxMp(mp),
      attack(attack), defense(defense), armorPenetration(armorPen),
      critChance(critChance), critDamage(critDamage), ignoreArmor(ignoreArmor),
      readyArrows(heroClass == HeroClass::RANGER ? 2 : 0),
      skillCooldowns{0, 0, 0}, maxCooldowns{0, 0, 0},
      isParrying(false), isBlocking(false), isEvading(false), isDefending(false), skillLockTurns(0),
      poisonTurns(0), poisonDamagePerTurn(0), regenTurns(0), regenPerTurn(0) {}

int Hero::normalAttack() {
    return getEffectiveAttack();
}

bool Hero::useSkill(int skillIndex, int& outDamage) {
    std::string msg;
    return useSkill(skillIndex, outDamage, msg);
}

bool Hero::useSkill(int skillIndex, int& outDamage, std::string& outMessage) {
    outDamage = 0;
    outMessage = "Hero has no base skills implemented.";
    return false;
}

void Hero::takeDamage(int damage) {
    if (damage <= 0) return;
    hp = std::max(0, hp - damage);
}

void Hero::takeDirectDamage(int damage) {
    if (damage <= 0) return;
    hp = std::max(0, hp - damage);
}

void Hero::heal(int amount) {
    if (amount <= 0) return;
    hp = std::min(maxHp, hp + amount);
}

void Hero::restoreMp(int amount) {
    if (amount <= 0) return;
    mp = std::min(maxMp, mp + amount);
}

bool Hero::addExp(int amount) {
    return LevelSystem::addExp(*this, amount);
}

void Hero::levelUp() {
    LevelSystem::levelUp(*this);
}

void Hero::reduceCooldowns() {
    for (size_t i = 0; i < skillCooldowns.size(); ++i) {
        if (skillCooldowns[i] > 0) {
            skillCooldowns[i]--;
        }
    }
    if (skillLockTurns > 0) {
        skillLockTurns--;
    }
}

bool Hero::isSkillReady(int skillIndex) const {
    if (skillLockTurns > 0) return false;
    if (skillIndex < 1 || skillIndex > 3) return false;
    return skillCooldowns[skillIndex - 1] == 0;
}

int Hero::getSkillCooldown(int skillIndex) const {
    if (skillIndex < 1 || skillIndex > 3) return 0;
    return skillCooldowns[skillIndex - 1];
}

void Hero::resetCombatStances() {
    isParrying = false;
    isBlocking = false;
    isEvading = false;
    isDefending = false;
}

void Hero::lockSkills(int turns) {
    skillLockTurns = turns;
}


void Hero::applyPoison(int turns, int damagePerTurn) {
    poisonTurns = turns;
    poisonDamagePerTurn = damagePerTurn;
}

int Hero::takePoisonDamage() {
    if (poisonTurns > 0) {
        int dmg = poisonDamagePerTurn;
        takeDirectDamage(dmg);
        poisonTurns--;
        if (poisonTurns == 0) {
            poisonDamagePerTurn = 0;
        }
        return dmg;
    }
    return 0;
}

bool Hero::isPoisoned() const {
    return poisonTurns > 0;
}

int Hero::getPoisonTurns() const {
    return poisonTurns;
}

int Hero::getPoisonDamagePerTurn() const {
    return poisonDamagePerTurn;
}

void Hero::applyRegen(int turns, int healPerTurn) {
    regenTurns = turns;
    regenPerTurn = healPerTurn;
}

int Hero::processRegen() {
    if (regenTurns > 0) {
        int amt = regenPerTurn;
        heal(amt);
        regenTurns--;
        if (regenTurns == 0) {
            regenPerTurn = 0;
        }
        return amt;
    }
    return 0;
}

bool Hero::hasRegen() const {
    return regenTurns > 0;
}

int Hero::getRegenTurns() const {
    return regenTurns;
}

int Hero::getRegenPerTurn() const {
    return regenPerTurn;
}

void Hero::clearStatusEffects() {
    poisonTurns = 0;
    poisonDamagePerTurn = 0;
    regenTurns = 0;
    regenPerTurn = 0;
    resetCombatStances();
}

bool Hero::isAlive() const {
    return hp > 0;
}

std::string Hero::getSkillName(int skillIndex) const {
    return "Skill " + std::to_string(skillIndex);
}

void Hero::displaySkills() const {
    std::cout << "[Skills for " << getHeroClassName() << "]\n";
    for (int i = 1; i <= 3; ++i) {
        std::cout << "  " << i << ". " << getSkillName(i) << " (CD: " << maxCooldowns[i-1] << " turns)\n";
    }
}

void Hero::displayStats() const {
    std::cout << "\n========================================\n"
              << "          THÔNG TIN ANH HÙNG            \n"
              << "========================================\n"
              << " Tên      : " << name << "\n"
              << " Lớp (Job): " << getHeroClassName() << "\n"
              << " Cấp độ   : Level " << level << "\n"
              << " EXP      : " << exp << " / " << getExpToNextLevel() << "\n"
              << " Máu (HP) : " << hp << " / " << maxHp << "\n"
              << " Năng Lượng (MP): " << mp << " / " << maxMp << "\n"
              << " Tấn công : " << getEffectiveAttack() << " (Cơ bản: " << attack << ")\n"
              << " Phòng thủ: " << getEffectiveDefense() << " (Cơ bản: " << defense << ")\n"
              << " Vàng     : " << gold << " G\n"
              << " Tiềm năng: " << statPoints << " điểm chưa phân bổ\n";
    if (heroClass == HeroClass::RANGER) {
        std::cout << " Mũi tên  : " << readyArrows << " tên sẵn sàng\n";
    }
    std::cout << "========================================\n";
}

std::string Hero::getName() const { return name; }
HeroClass Hero::getHeroClass() const { return heroClass; }

std::string Hero::getHeroClassName() const {
    switch (heroClass) {
        case HeroClass::WARRIOR: return "Warrior";
        case HeroClass::RANGER:  return "Ranger";
        case HeroClass::MAGE:    return "Mage";
        default:                 return "Unknown";
    }
}

int Hero::getLevel() const { return level; }
int Hero::getExp() const { return exp; }
int Hero::getExpToNextLevel() const { return LevelSystem::getExpRequiredForLevel(level); }
int Hero::getHp() const { return hp; }
int Hero::getMaxHp() const { return maxHp; }
int Hero::getMp() const { return mp; }
int Hero::getMaxMp() const { return maxMp; }
int Hero::getAttack() const { return attack; }
int Hero::getDefense() const { return defense; }
int Hero::getArmorPenetration() const { return armorPenetration; }
float Hero::getCritChance() const { return critChance; }
float Hero::getCritDamage() const { return critDamage; }
bool Hero::isIgnoreArmor() const { return ignoreArmor; }

int Hero::getReadyArrows() const { return readyArrows; }
void Hero::addReadyArrows(int amount) { readyArrows = std::max(0, readyArrows + amount); }
void Hero::setReadyArrows(int value) { readyArrows = std::max(0, value); }

bool Hero::getIsParrying() const { return isParrying; }
void Hero::setIsParrying(bool value) { isParrying = value; }
bool Hero::getIsBlocking() const { return isBlocking; }
void Hero::setIsBlocking(bool value) { isBlocking = value; }
bool Hero::getIsEvading() const { return isEvading; }
void Hero::setIsEvading(bool value) { isEvading = value; }
bool Hero::getIsDefending() const { return isDefending; }
void Hero::setIsDefending(bool value) { isDefending = value; }
int Hero::getSkillLockTurns() const { return skillLockTurns; }

Inventory& Hero::getInventory() { return inventory; }
const Inventory& Hero::getInventory() const { return inventory; }

int Hero::getEffectiveAttack() const {
    return attack + inventory.getEquippedWeaponBonus();
}

int Hero::getEffectiveDefense() const {
    return defense + inventory.getEquippedArmorBonus();
}

void Hero::setHp(int value) { hp = std::min(maxHp, std::max(0, value)); }
void Hero::setMaxHp(int value) { maxHp = std::max(1, value); hp = std::min(hp, maxHp); }
void Hero::setMp(int value) { mp = std::min(maxMp, std::max(0, value)); }
void Hero::setMaxMp(int value) { maxMp = std::max(0, value); mp = std::min(mp, maxMp); }
void Hero::setLevel(int value) { level = std::max(1, value); }
void Hero::setExp(int value) { exp = std::max(0, value); }
void Hero::setAttack(int value) { attack = std::max(0, value); }
void Hero::setDefense(int value) { defense = std::max(0, value); }

void Hero::setName(const std::string& newName) { name = newName; }
void Hero::setHeroClass(HeroClass newClass) { heroClass = newClass; }
void Hero::setArmorPenetration(int value) { armorPenetration = value; }
void Hero::setCritChance(float value) { critChance = value; }
void Hero::setCritDamage(float value) { critDamage = value; }
void Hero::setIgnoreArmor(bool value) { ignoreArmor = value; }

int Hero::getGold() const { return gold; }
void Hero::setGold(int amount) { gold = std::max(0, amount); }
void Hero::addGold(int amount) { if (amount > 0) gold += amount; }
void Hero::removeGold(int amount) { if (amount > 0) gold = std::max(0, gold - amount); }
bool Hero::spendGold(int amount) {
    if (amount <= 0) return true;
    if (gold >= amount) {
        gold -= amount;
        return true;
    }
    return false;
}

const std::vector<int>& Hero::getSkillCooldowns() const { return skillCooldowns; }
void Hero::setSkillCooldowns(const std::vector<int>& cds) { skillCooldowns = cds; }

void Hero::increaseMaxHp(int amount) { maxHp += amount; hp += amount; }
void Hero::increaseMaxMp(int amount) { maxMp += amount; mp += amount; }
void Hero::increaseAttack(int amount) { attack += amount; }
void Hero::increaseDefense(int amount) { defense += amount; }

int Hero::getStatPoints() const { return statPoints; }
void Hero::setStatPoints(int points) { statPoints = std::max(0, points); }
void Hero::addStatPoints(int points) { if (points > 0) statPoints += points; }

bool Hero::allocateStrength(int points) {
    if (points <= 0 || statPoints < points) return false;
    statPoints -= points;
    attack += points * 2;
    return true;
}

bool Hero::allocateIntelligence(int points) {
    if (points <= 0 || statPoints < points) return false;
    statPoints -= points;
    maxMp += points * 10;
    mp += points * 10;
    return true;
}

bool Hero::allocateVitality(int points) {
    if (points <= 0 || statPoints < points) return false;
    statPoints -= points;
    maxHp += points * 15;
    hp += points * 15;
    defense += points * 1;
    return true;
}
