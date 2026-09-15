/**
 * @file Hero.cpp
 * @brief Implement Hero class methods with 3 specialized classes and turn-based cooldowns.
 * @author Quý & Antigravity
 */

#include "Hero.h"
#include "LevelSystem.h"
#include <algorithm>
#include <cstdlib>

Hero::Hero(const std::string& name, HeroClass heroClass)
    : name(name), heroClass(heroClass), level(1), exp(0), gold(0),
      isParrying(false), isBlocking(false), isEvading(false), isDefending(false), skillLockTurns(0),
      poisonTurns(0), poisonDamagePerTurn(0), regenTurns(0), regenPerTurn(0) {
    
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

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int attack, int defense)
    : Hero(name, heroClass, hp, 50, attack, defense, 10, 0.25f, 0.25f, false) {}

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int attack, int defense,
           int armorPen, float critChance, float critDamage, bool ignoreArmor)
    : name(name), heroClass(heroClass), level(1), exp(0), gold(0), hp(hp), maxHp(hp), mp(50), maxMp(50),
      attack(attack), defense(defense), armorPenetration(armorPen),
      critChance(critChance), critDamage(critDamage), ignoreArmor(ignoreArmor),
      readyArrows(heroClass == HeroClass::RANGER ? 2 : 0),
      skillCooldowns({0, 0, 0}),
      isParrying(false), isBlocking(false), isEvading(false), isDefending(false), skillLockTurns(0),
      poisonTurns(0), poisonDamagePerTurn(0), regenTurns(0), regenPerTurn(0) {
    
    switch (heroClass) {
        case HeroClass::WARRIOR: maxCooldowns = {1, 1, 5}; break;
        case HeroClass::RANGER:  maxCooldowns = {2, 1, 5}; break;
        case HeroClass::MAGE:    maxCooldowns = {1, 3, 5}; break;
    }
}

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int mp, int attack, int defense,
           int armorPen, float critChance, float critDamage, bool ignoreArmor)
    : name(name), heroClass(heroClass), level(1), exp(0), gold(0), hp(hp), maxHp(hp), mp(mp), maxMp(mp),
      attack(attack), defense(defense), armorPenetration(armorPen),
      critChance(critChance), critDamage(critDamage), ignoreArmor(ignoreArmor),
      readyArrows(heroClass == HeroClass::RANGER ? 2 : 0),
      skillCooldowns({0, 0, 0}),
      isParrying(false), isBlocking(false), isEvading(false), isDefending(false), skillLockTurns(0),
      poisonTurns(0), poisonDamagePerTurn(0), regenTurns(0), regenPerTurn(0) {
    
    switch (heroClass) {
        case HeroClass::WARRIOR: maxCooldowns = {1, 1, 5}; break;
        case HeroClass::RANGER:  maxCooldowns = {2, 1, 5}; break;
        case HeroClass::MAGE:    maxCooldowns = {1, 3, 5}; break;
    }
}

bool Hero::useSkill(int skillIndex, int& outDamage) {
    std::string msg;
    return useSkill(skillIndex, outDamage, msg);
}


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
            heal(25);
            applyRegen(3, 8); // Applies Regeneration HoT: +8 HP/turn for 3 turns
            skillCooldowns[idx] = maxCooldowns[idx];
            outMessage = name + " consumes a Healing Potion restoring 25 HP and gaining Regeneration (+8 HP/turn for 3 turns)! (HP: " +
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
    if (damage <= 0) return;
    // Dùng getEffectiveDefense() để cộng dồn bonus Armor đang trang bị
    int effectiveDamage = std::max(1, damage - getEffectiveDefense());
    hp = std::max(0, hp - effectiveDamage);
}

void Hero::heal(int amount) {
    if (amount <= 0) return;
    hp = std::min(maxHp, hp + amount);
}

void Hero::restoreMp(int amount) {
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

std::string Hero::getSkillName(int skillIndex) const {
    if (heroClass == HeroClass::WARRIOR) {
        if (skillIndex == 1) return "Sword Slash";
        if (skillIndex == 2) return "Parry";
        if (skillIndex == 3) return "Shield Block";
    } else if (heroClass == HeroClass::RANGER) {
        if (skillIndex == 1) return "Evade & Reload";
        if (skillIndex == 2) return "Aimed Shot";
        if (skillIndex == 3) return "Arrow Barrage";
    } else if (heroClass == HeroClass::MAGE) {
        if (skillIndex == 1) return "Energy Ray";
        if (skillIndex == 2) return "Healing Potion";
        if (skillIndex == 3) return "Poison Flask";
    }
    return "Unknown";
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

bool Hero::isAlive() const {
    return hp > 0;
}

void Hero::displayStats() const {
    std::cout << "--- " << name << " [" << getHeroClassName() << "] (Level " << level << ") ---\n"
              << "HP: " << hp << "/" << maxHp << " | MP: " << mp << "/" << maxMp << " | ATK: " << attack << " | DEF: " << defense 
              << " | EXP: " << exp << "/" << getExpToNextLevel() << "\n"
              << "Penetration: " << (ignoreArmor ? "IGNORE ALL" : std::to_string(armorPenetration))
              << " | Crit: " << static_cast<int>(critChance * 100) << "% (+"
              << static_cast<int>(critDamage * 100) << "%)";
    if (heroClass == HeroClass::RANGER) {
        std::cout << " | Ready Arrows: " << readyArrows;
    }
    std::cout << "\nSkills CD: [1] " << skillCooldowns[0] << " turns | [2] " << skillCooldowns[1]
              << " turns | [3] " << skillCooldowns[2] << " turns\n";
}

void Hero::displaySkills() const {
    std::cout << "[Skills for " << getHeroClassName() << "]\n";
    for (int i = 1; i <= 3; ++i) {
        std::cout << "  " << i << ". " << getSkillName(i) << " (CD: " << maxCooldowns[i-1] << " turns)\n";
    }
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
int Hero::getExpToNextLevel() const { return LevelSystem::getExpRequiredForLevel(level); }
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
bool Hero::getIsDefending() const { return isDefending; }
void Hero::setIsDefending(bool value) { isDefending = value; }
int Hero::getSkillLockTurns() const { return skillLockTurns; }

Inventory& Hero::getInventory() {
    return inventory;
}

const Inventory& Hero::getInventory() const {
    return inventory;
}

int Hero::getEffectiveAttack() const {
    return attack + inventory.getEquippedWeaponBonus();
}

int Hero::getEffectiveDefense() const {
    return defense + inventory.getEquippedArmorBonus();
}

void Hero::setHp(int value) { hp = std::max(0, std::min(value, maxHp)); }
void Hero::setLevel(int value) { level = value; }
void Hero::setExp(int value) { exp = value; }

void Hero::applyPoison(int turns, int damagePerTurn) {
    if (turns <= 0 || damagePerTurn <= 0) return;
    poisonTurns = turns;
    poisonDamagePerTurn = damagePerTurn;
}

int Hero::takePoisonDamage() {
    if (poisonTurns <= 0) return 0;
    int dmg = poisonDamagePerTurn;
    takeDamage(dmg);
    poisonTurns--;
    return dmg;
}

bool Hero::isPoisoned() const {
    return poisonTurns > 0 && hp > 0;
}

int Hero::getPoisonTurns() const {
    return poisonTurns;
}

int Hero::getPoisonDamagePerTurn() const {
    return poisonDamagePerTurn;
}

void Hero::applyRegen(int turns, int healPerTurn) {
    if (turns <= 0 || healPerTurn <= 0) return;
    regenTurns = turns;
    regenPerTurn = healPerTurn;
}

int Hero::processRegen() {
    if (regenTurns <= 0 || hp <= 0) return 0;
    int before = hp;
    heal(regenPerTurn);
    int actualHealed = hp - before;
    regenTurns--;
    return actualHealed;
}

bool Hero::hasRegen() const {
    return regenTurns > 0 && hp > 0;
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
}

int Hero::getMp() const { return mp; }
int Hero::getMaxMp() const { return maxMp; }
void Hero::setMp(int value) { mp = std::max(0, std::min(value, maxMp)); }
void Hero::setMaxHp(int value) { maxHp = std::max(1, value); hp = std::min(hp, maxHp); }
void Hero::setMaxMp(int value) { maxMp = std::max(0, value); mp = std::min(mp, maxMp); }
void Hero::setAttack(int value) { attack = std::max(0, value); }
void Hero::setDefense(int value) { defense = std::max(0, value); }

void Hero::increaseMaxHp(int amount) { maxHp += amount; }
void Hero::increaseMaxMp(int amount) { maxMp += amount; }
void Hero::increaseAttack(int amount) { attack += amount; }
void Hero::increaseDefense(int amount) { defense += amount; }

void Hero::setName(const std::string& newName) { name = newName; }
void Hero::setHeroClass(HeroClass newClass) {
    heroClass = newClass;
    switch (heroClass) {
        case HeroClass::WARRIOR: maxCooldowns = {1, 1, 5}; break;
        case HeroClass::RANGER:  maxCooldowns = {2, 1, 5}; break;
        case HeroClass::MAGE:    maxCooldowns = {1, 3, 5}; break;
    }
}
void Hero::setArmorPenetration(int value) { armorPenetration = value; }
void Hero::setCritChance(float value) { critChance = value; }
void Hero::setCritDamage(float value) { critDamage = value; }
void Hero::setIgnoreArmor(bool value) { ignoreArmor = value; }

int Hero::getGold() const { return gold; }
void Hero::setGold(int amount) { gold = std::max(0, amount); }
void Hero::addGold(int amount) { gold = std::max(0, gold + amount); }

const std::vector<int>& Hero::getSkillCooldowns() const { return skillCooldowns; }
void Hero::setSkillCooldowns(const std::vector<int>& cds) { skillCooldowns = cds; }
