#pragma once

#include <string>
#include <iostream>

enum class EnemyType {
    MINION,
    BOSS
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

public:
    Enemy(const std::string& name, EnemyType type, int hp, int attack, int defense, int expReward, int goldReward);
    virtual ~Enemy() = default;

    // Actions
    virtual int chooseAction(); // Returns action type (1: Normal attack, 2: Special skill, etc.)
    virtual void takeDamage(int damage);
    bool isAlive() const;
    void displayStats() const;

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
