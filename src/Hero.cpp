/**
 * @file Hero.cpp
 * @brief Implement Hero class methods.
 * @author Quý
 */

#include "Hero.h"
#include <algorithm>

Hero::Hero(const std::string& name, HeroClass heroClass, int hp, int mp, int attack, int defense)
    : name(name), heroClass(heroClass), level(1), exp(0), hp(hp), maxHp(hp), mp(mp), maxMp(mp), attack(attack), defense(defense) {}

int Hero::normalAttack() {
    return attack;
}

bool Hero::useSkill(int skillIndex, int& outDamage) {
    if (skillIndex == 1 && mp >= 10) {
        mp -= 10;
        outDamage = attack * 2;
        return true;
    }
    outDamage = 0;
    return false;
}

void Hero::takeDamage(int damage) {
    int effectiveDamage = std::max(1, damage - defense);
    hp = std::max(0, hp - effectiveDamage);
}

void Hero::heal(int amount) {
    hp = std::min(maxHp, hp + amount);
}

void Hero::restoreMp(int amount) {
    mp = std::min(maxMp, mp + amount);
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
    maxMp += 5;
    attack += 3;
    defense += 1;
    hp = maxHp;
    mp = maxMp;
}

bool Hero::isAlive() const {
    return hp > 0;
}

void Hero::displayStats() const {
    std::cout << "--- " << name << " (Level " << level << ") ---\n"
              << "HP: " << hp << "/" << maxHp << " | MP: " << mp << "/" << maxMp << "\n"
              << "ATK: " << attack << " | DEF: " << defense << " | EXP: " << exp << "\n";
}

std::string Hero::getName() const { return name; }
HeroClass Hero::getHeroClass() const { return heroClass; }
int Hero::getLevel() const { return level; }
int Hero::getExp() const { return exp; }
int Hero::getHp() const { return hp; }
int Hero::getMaxHp() const { return maxHp; }
int Hero::getMp() const { return mp; }
int Hero::getMaxMp() const { return maxMp; }
int Hero::getAttack() const { return attack; }
int Hero::getDefense() const { return defense; }

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

void Hero::setHp(int value) { hp = std::clamp(value, 0, maxHp); }
void Hero::setMp(int value) { mp = std::clamp(value, 0, maxMp); }
void Hero::setLevel(int value) { level = value; }
void Hero::setExp(int value) { exp = value; }

