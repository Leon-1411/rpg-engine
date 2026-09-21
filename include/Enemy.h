#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "Item.h"

enum class EnemyType {
    MINION,
    BOSS
};

struct DropEntry {
    std::shared_ptr<Item> item;
    double dropChance; // 0.0 - 1.0 (1.0 = 100%)
};

class Enemy {
protected:
    std::string name;
    EnemyType type;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int expReward;
    int goldReward;
    std::vector<DropEntry> dropTable;

public:
    Enemy(const std::string& name, EnemyType type, int hp, int attack, int defense, int expReward, int goldReward);
    virtual ~Enemy() = default;

    // Actions
    virtual int chooseAction(); // Returns action type (1: Normal attack, 2: Special skill, etc.)
    virtual void takeDamage(int damage);
    bool isAlive() const;
    virtual void displayStats() const;
    virtual std::string getSpecialSkillName() const;

    // Drop table management
    void addDropItem(std::shared_ptr<Item> item, double dropChance = 1.0);
    std::vector<std::shared_ptr<Item>> rollDrops() const;
    const std::vector<DropEntry>& getDropTable() const;

    // Getters & Setters
    std::string getName() const;
    EnemyType getType() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttack() const;
    int getDefense() const;
    int getExpReward() const;
    int getGoldReward() const;

    void setHp(int value);
};

