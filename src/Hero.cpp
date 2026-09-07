/**
 * @file Hero.cpp
 * @brief Implement Hero class methods.
 * @author Quý
 */

#include "Hero.h"
#include "LevelSystem.h"
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
    LevelSystem::addExp(*this, amount);
}

void Hero::levelUp() {
    LevelSystem::levelUp(*this);
}

bool Hero::isAlive() const {
    return hp > 0;
}

void Hero::displayStats() const {
    std::cout << "--- " << name << " (Level " << level << ") ---\n"
              << "HP: " << hp << "/" << maxHp << " | MP: " << mp << "/" << maxMp << "\n"
              << "ATK: " << attack << " | DEF: " << defense 
              << " | EXP: " << exp << "/" << getExpToNextLevel() << "\n";
}

std::string Hero::getName() const { return name; }
HeroClass Hero::getHeroClass() const { return heroClass; }
int Hero::getLevel() const { return level; }
int Hero::getExp() const { return exp; }
int Hero::getExpToNextLevel() const { return LevelSystem::getExpRequiredForLevel(level); }
int Hero::getHp() const { return hp; }
int Hero::getMaxHp() const { return maxHp; }
int Hero::getMp() const { return mp; }
int Hero::getMaxMp() const { return maxMp; }
int Hero::getAttack() const { return attack; }
int Hero::getDefense() const { return defense; }

void Hero::setHp(int value) { hp = std::clamp(value, 0, maxHp); }
void Hero::setMp(int value) { mp = std::clamp(value, 0, maxMp); }
void Hero::setLevel(int value) { level = value; }
void Hero::setExp(int value) { exp = value; }

void Hero::setMaxHp(int value) { maxHp = std::max(1, value); hp = std::min(hp, maxHp); }
void Hero::setMaxMp(int value) { maxMp = std::max(0, value); mp = std::min(mp, maxMp); }
void Hero::setAttack(int value) { attack = std::max(0, value); }
void Hero::setDefense(int value) { defense = std::max(0, value); }

void Hero::increaseMaxHp(int amount) { maxHp += amount; }
void Hero::increaseMaxMp(int amount) { maxMp += amount; }
void Hero::increaseAttack(int amount) { attack += amount; }
void Hero::increaseDefense(int amount) { defense += amount; }
