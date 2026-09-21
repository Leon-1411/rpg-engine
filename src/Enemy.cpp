/**
 * @file Enemy.cpp
 * @brief Implement Enemy class methods.
 * @author Nhật
 */

#include "Enemy.h"
#include <algorithm>
#include <random>

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

void Enemy::addDropItem(std::shared_ptr<Item> item, double dropChance) {
    if (item) {
        dropTable.push_back({item, std::clamp(dropChance, 0.0, 1.0)});
    }
}

std::vector<std::shared_ptr<Item>> Enemy::rollDrops() const {
    std::vector<std::shared_ptr<Item>> dropped;
    if (dropTable.empty()) return dropped;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    for (const auto& entry : dropTable) {
        if (entry.item && dis(gen) <= entry.dropChance) {
            dropped.push_back(entry.item->clone());
        }
    }
    return dropped;
}

const std::vector<DropEntry>& Enemy::getDropTable() const {
    return dropTable;
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

