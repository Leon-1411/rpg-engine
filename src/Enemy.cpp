/**
 * @file Enemy.cpp
 * @brief Implement Enemy class methods with class-dependent miss chance and DoT status.
 * @author Nhật & Antigravity
 */

#include "Enemy.h"
#include <algorithm>
#include <cstdlib>

Enemy::Enemy(const std::string& name, EnemyType type, int hp, int attack, int defense,
             int expReward, int goldReward, int armorPen, float critChance, float critDamage)
    : name(name), type(type), hp(hp), maxHp(hp), attack(attack), defense(defense),
      armorPenetration(armorPen), critChance(critChance), critDamage(critDamage),
      expReward(expReward), goldReward(goldReward),
      poisonTurns(0), poisonDamagePerTurn(0) {}

int Enemy::chooseAction() {
    return 1;
}

int Enemy::chooseAction(HeroClass targetClass) {
    int missRate = 10; // Default 10% (against Mage)
    if (targetClass == HeroClass::WARRIOR) {
        missRate = 45; // 45% High miss/hesitate against Warrior
    } else if (targetClass == HeroClass::RANGER) {
        missRate = 25; // 25% Med miss/hesitate against Ranger
    }

    int roll = std::rand() % 100;
    if (roll < missRate) {
        return 0; // 0 = Hesitate / Miss attack
    }
    return 1; // 1 = Normal attack
}

void Enemy::takeDamage(int damage) {
    hp = std::max(0, hp - damage);
}

bool Enemy::isAlive() const {
    return hp > 0;
}

void Enemy::applyPoison(int turns, int damagePerTurn) {
    poisonTurns = turns;
    poisonDamagePerTurn = damagePerTurn;
}

int Enemy::takePoisonDamage() {
    if (poisonTurns <= 0) return 0;
    int dmg = poisonDamagePerTurn;
    takeDamage(dmg);
    poisonTurns--;
    return dmg;
}

bool Enemy::isPoisoned() const {
    return poisonTurns > 0 && hp > 0;
}

int Enemy::getPoisonTurns() const {
    return poisonTurns;
}

void Enemy::displayStats() const {
    std::cout << "--- " << name << " ---\n"
              << "HP: " << hp << "/" << maxHp << " | ATK: " << attack << " | DEF: " << defense << "\n"
              << "Penetration: " << armorPenetration
              << " | Crit: " << static_cast<int>(critChance * 100) << "% (+"
              << static_cast<int>(critDamage * 100) << "%)\n";
    if (poisonTurns > 0) {
        std::cout << "[STATUS] Poisoned for " << poisonTurns << " more turn(s) (" << poisonDamagePerTurn << " dmg/turn)\n";
    }
}

std::string Enemy::getName() const { return name; }
EnemyType Enemy::getType() const { return type; }
int Enemy::getHp() const { return hp; }
int Enemy::getMaxHp() const { return maxHp; }
int Enemy::getAttack() const { return attack; }
int Enemy::getDefense() const { return defense; }
int Enemy::getArmorPenetration() const { return armorPenetration; }
float Enemy::getCritChance() const { return critChance; }
float Enemy::getCritDamage() const { return critDamage; }
int Enemy::getExpReward() const { return expReward; }
int Enemy::getGoldReward() const { return goldReward; }

void Enemy::setHp(int value) { hp = std::clamp(value, 0, maxHp); }
