/**
 * @file Enemy.cpp
 * @brief Implement Enemy class methods.
 * @author Nhật
 */

#include "Enemy.h"
#include <algorithm>

Enemy::Enemy(const std::string& name, EnemyType type, int hp, int attack, int defense, int expReward, int goldReward)
    : name(name), type(type), hp(hp), maxHp(hp), attack(attack), defense(defense), expReward(expReward), goldReward(goldReward) {}

int Enemy::chooseAction() {
    // Basic AI: returns 1 for normal attack
    return 1;
}

void Enemy::takeDamage(int damage) {
    int effectiveDamage = std::max(1, damage - defense);
    hp = std::max(0, hp - effectiveDamage);
}

bool Enemy::isAlive() const {
    return hp > 0;
}

void Enemy::displayStats() const {
    std::cout << "--- " << name << " ---\n"
              << "HP: " << hp << "/" << maxHp << " | ATK: " << attack << " | DEF: " << defense << "\n";
}

std::string Enemy::getName() const { return name; }
EnemyType Enemy::getType() const { return type; }
int Enemy::getHp() const { return hp; }
int Enemy::getMaxHp() const { return maxHp; }
int Enemy::getAttack() const { return attack; }
int Enemy::getDefense() const { return defense; }
int Enemy::getExpReward() const { return expReward; }
int Enemy::getGoldReward() const { return goldReward; }
std::string Enemy::getSpecialSkillName() const { return ""; }

void Enemy::setHp(int value) { hp = std::clamp(value, 0, maxHp); }
