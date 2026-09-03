#pragma once

#include <string>
#include <vector>
#include <iostream>

enum class HeroClass {
    WARRIOR,
    MAGE,
    RANGER
};

class Hero {
protected:
    std::string name;
    HeroClass heroClass;
    int level;
    int exp;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    int attack;
    int defense;

public:
    Hero(const std::string& name, HeroClass heroClass, int hp, int mp, int attack, int defense);
    virtual ~Hero() = default;

    // Core actions
    virtual int normalAttack();
    virtual bool useSkill(int skillIndex, int& outDamage);
    virtual void takeDamage(int damage);
    virtual void heal(int amount);
    virtual void restoreMp(int amount);
    virtual void addExp(int amount);

    // Status checks
    bool isAlive() const;
    void displayStats() const;

    // Getters & Setters
    std::string getName() const;
    HeroClass getHeroClass() const;
    int getLevel() const;
    int getExp() const;
    int getHp() const;
    int getMaxHp() const;
    int getMp() const;
    int getMaxMp() const;
    int getAttack() const;
    int getDefense() const;

    void setHp(int value);
    void setMp(int value);
    void setLevel(int value);
    void setExp(int value);
};
