/**
 * @file Hero.cpp
 * @brief Implement Hero class methods with 3 specialized classes and turn-based cooldowns.
 * @author Quý & Antigravity
 */

#include "Hero.h"
#include <algorithm>
#include <cstdlib>

Hero::Hero(const std::string& name, HeroClass heroClass)
    : name(name), heroClass(heroClass), level(1), exp(0),
      isParrying(false), isBlocking(false), isEvading(false), skillLockTurns(0) {
    
    skillCooldowns = {0, 0, 0};

    switch (heroClass) {
        case HeroClass::WARRIOR:
            // High HP, High DEF, Low ATK, Med Pen, Low Crit, Med Crit Dmg
            maxHp = hp = 150;
            attack = 7;
            defense = 25;
            armorPenetration = 10;
            critChance = 0.10f;
            critDamage = 0.25f;
            ignoreArmor = false;
            readyArrows = 0;
            maxCooldowns = {1, 1, 5};
            break;

        case HeroClass::RANGER:
            // Med HP, Med DEF, Med ATK, Low Pen, Med Crit, High Crit Dmg
            maxHp = hp = 100;
            attack = 15;
            defense = 10;
            armorPenetration = 5;
            critChance = 0.25f;
            critDamage = 0.50f;
            ignoreArmor = false;
            readyArrows = 2; // Initial arrows
            maxCooldowns = {2, 1, 5};
            break;

        case HeroClass::MAGE:
            // Med HP, Low DEF, High ATK, Ignore Armor, 0 Crit
            maxHp = hp = 100;
            attack = 25;
            defense = 5;
            armorPenetration = 0;
            critChance = 0.0f;
            critDamage = 0.0f;
            ignoreArmor = true;
            readyArrows = 0;
            maxCooldowns = {1, 3, 5};
            break;
    }
}

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int attack, int defense,
           int armorPen, float critChance, float critDamage, bool ignoreArmor)
    : name(name), heroClass(heroClass), level(1), exp(0), hp(hp), maxHp(hp),
      attack(attack), defense(defense), armorPenetration(armorPen),
      critChance(critChance), critDamage(critDamage), ignoreArmor(ignoreArmor),
      readyArrows(heroClass == HeroClass::RANGER ? 2 : 0),
      skillCooldowns({0, 0, 0}),
      isParrying(false), isBlocking(false), isEvading(false), skillLockTurns(0) {
    
    switch (heroClass) {
        case HeroClass::WARRIOR: maxCooldowns = {1, 1, 5}; break;
        case HeroClass::RANGER:  maxCooldowns = {2, 1, 5}; break;
        case HeroClass::MAGE:    maxCooldowns = {1, 3, 5}; break;
    }
}

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int /*mp*/, int attack, int defense)
    : Hero(name, heroClass, hp, attack, defense) {}

int Hero::normalAttack() {
    return attack;
}

bool Hero::useSkill(int skillIndex, int& outDamage, std::string& outMessage) {
    if (skillLockTurns > 0) {
        outMessage = "Skills are locked due to mistimed parry! (" + std::to_string(skillLockTurns) + " turn(s) remaining)";
        return false;
    }

    if (skillIndex < 1 || skillIndex > 3) {
        outMessage = "Invalid skill index!";
        return false;
    }

    int idx = skillIndex - 1;
    if (skillCooldowns[idx] > 0) {
        outMessage = "Skill " + std::to_string(skillIndex) + " is on cooldown! (" +
                     std::to_string(skillCooldowns[idx]) + " turn(s) left)";
        return false;
    }

    outDamage = 0;

    if (heroClass == HeroClass::WARRIOR) {
        if (skillIndex == 1) { // Sword Slash
            outDamage = static_cast<int>(attack * 1.8f);
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " performs a devastating Sword Slash!";
            return true;
        } else if (skillIndex == 2) { // Parry
            isParrying = true;
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " enters Parry Stance! (Will counterattack if enemy attacks; penalized if enemy idles)";
            return true;
        } else if (skillIndex == 3) { // Block
            isBlocking = true;
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " raises heavy shield into Block Stance! (Nullifies incoming attack)";
            return true;
        }
    } else if (heroClass == HeroClass::RANGER) {
        if (skillIndex == 1) { // Take Aim / Evade
            isEvading = true;
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " takes an elusive Stance! (Will evade attack and load +5 Ready Arrows)";
            return true;
        } else if (skillIndex == 2) { // Aimed Shot
            if (readyArrows < 1) {
                outMessage = "Not enough Ready Arrows! Need at least 1 arrow.";
                return false;
            }
            readyArrows -= 1;
            outDamage = static_cast<int>(attack * 1.5f);
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " fires an Aimed Shot! (1 arrow consumed, " + std::to_string(readyArrows) + " remaining)";
            return true;
        } else if (skillIndex == 3) { // Arrow Barrage
            if (readyArrows < 2) {
                outMessage = "Not enough Ready Arrows! Need at least 2 arrows to unleash Arrow Barrage.";
                return false;
            }
            int arrowsFired = readyArrows;
            readyArrows = 0;
            skillCooldowns[idx] = maxCooldowns[idx];

            int hits = 0;
            for (int i = 0; i < arrowsFired; ++i) {
                if ((std::rand() % 100) >= 30) { // 30% miss chance
                    hits++;
                }
            }
            outDamage = hits * attack;
            outMessage = name + " unleashes Arrow Barrage with " + std::to_string(arrowsFired) +
                         " arrows (" + std::to_string(hits) + " hit, " + std::to_string(arrowsFired - hits) + " missed)!";
            return true;
        }
    } else if (heroClass == HeroClass::MAGE) {
        if (skillIndex == 1) { // Energy Ray
            outDamage = static_cast<int>(attack * 1.2f);
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " channels an Energy Ray from the staff!";
            return true;
        } else if (skillIndex == 2) { // Healing Potion
            heal(30);
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " consumes a Healing Potion restoring 30 HP! (HP: " +
                         std::to_string(hp) + "/" + std::to_string(maxHp) + ")";
            return true;
        } else if (skillIndex == 3) { // Poison Flask
            outDamage = -3; // Flag for Poison Flask effect
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " hurls a Poison Flask inflicting deadly venom for 5 turns!";
            return true;
        }
    }

    return false;
}

void Hero::takeDamage(int damage) {
    hp = std::max(0, hp - damage);
}

void Hero::heal(int amount) {
    hp = std::min(maxHp, hp + amount);
}

void Hero::addExp(int amount) {
    exp += amount;
    if (exp >= level * 100) {
        exp -= level * 100;
        levelUp();
    }
}

void Hero::levelUp() {
    level++;
    maxHp += 15;
    attack += 3;
    defense += 2;
    hp = maxHp;
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
}

void Hero::lockSkills(int turns) {
    skillLockTurns = turns;
}

bool Hero::isAlive() const {
    return hp > 0;
}

void Hero::displayStats() const {
    std::cout << "--- " << name << " [" << getHeroClassName() << "] (Level " << level << ") ---\n"
              << "HP: " << hp << "/" << maxHp << " | ATK: " << attack << " | DEF: " << defense << "\n"
              << "Penetration: " << (ignoreArmor ? "IGNORE ALL" : std::to_string(armorPenetration))
              << " | Crit: " << static_cast<int>(critChance * 100) << "% (+"
              << static_cast<int>(critDamage * 100) << "%)";
    if (heroClass == HeroClass::RANGER) {
        std::cout << " | Ready Arrows: " << readyArrows;
    }
    std::cout << "\nSkills CD: [1] " << skillCooldowns[0] << " turns | [2] " << skillCooldowns[1]
              << " turns | [3] " << skillCooldowns[2] << " turns\n";
}

std::string Hero::getName() const { return name; }
HeroClass Hero::getHeroClass() const { return heroClass; }

std::string Hero::getHeroClassName() const {
    switch (heroClass) {
        case HeroClass::WARRIOR: return "Warrior";
        case HeroClass::MAGE:    return "Mage";
        case HeroClass::RANGER:  return "Ranger";
    }
    return "Unknown";
}

int Hero::getLevel() const { return level; }
int Hero::getExp() const { return exp; }
int Hero::getHp() const { return hp; }
int Hero::getMaxHp() const { return maxHp; }
int Hero::getAttack() const { return attack; }
int Hero::getDefense() const { return defense; }
int Hero::getArmorPenetration() const { return armorPenetration; }
float Hero::getCritChance() const { return critChance; }
float Hero::getCritDamage() const { return critDamage; }
bool Hero::isIgnoreArmor() const { return ignoreArmor; }

int Hero::getReadyArrows() const { return readyArrows; }
void Hero::addReadyArrows(int amount) { readyArrows += amount; }
void Hero::setReadyArrows(int value) { readyArrows = std::max(0, value); }

bool Hero::getIsParrying() const { return isParrying; }
void Hero::setIsParrying(bool value) { isParrying = value; }
bool Hero::getIsBlocking() const { return isBlocking; }
void Hero::setIsBlocking(bool value) { isBlocking = value; }
bool Hero::getIsEvading() const { return isEvading; }
void Hero::setIsEvading(bool value) { isEvading = value; }
int Hero::getSkillLockTurns() const { return skillLockTurns; }

void Hero::setHp(int value) { hp = std::clamp(value, 0, maxHp); }
void Hero::setLevel(int value) { level = value; }
void Hero::setExp(int value) { exp = value; }
