#pragma once

#include <string>
#include <vector>
#include <iostream>

enum class HeroClass {
    WARRIOR,
    MAGE,
    RANGER
};

inline std::string heroClassToString(HeroClass c) {
    switch (c) {
        case HeroClass::WARRIOR: return "WARRIOR";
        case HeroClass::MAGE: return "MAGE";
        case HeroClass::RANGER: return "RANGER";
        default: return "WARRIOR";
    }
}

inline HeroClass stringToHeroClass(const std::string& str) {
    if (str == "MAGE") return HeroClass::MAGE;
    if (str == "RANGER") return HeroClass::RANGER;
    return HeroClass::WARRIOR;
}

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
    int gold;

public:
    Hero(const std::string& name, HeroClass heroClass, int hp, int mp, int attack, int defense);
    virtual ~Hero() = default;

    // Core actions
    virtual int normalAttack();
    virtual bool useSkill(int skillIndex, int& outDamage);
    virtual void takeDamage(int damage);
    virtual void heal(int amount);
    virtual void restoreMp(int amount);
    virtual bool addExp(int amount);
    virtual void levelUp();

    // Status checks
    bool isAlive() const;
    void displayStats() const;
    virtual std::string getSkillName(int skillIndex) const;
    virtual void displaySkills() const;

    // Getters & Setters
    std::string getName() const;
    HeroClass getHeroClass() const;
    int getLevel() const;
    int getExp() const;
    int getExpToNextLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getMp() const;
    int getMaxMp() const;
    int getAttack() const;
    int getDefense() const;
    int getGold() const;

    void setHp(int value);
    void setMp(int value);
    void setLevel(int value);
    void setExp(int value);
    void setMaxHp(int value);
    void setMaxMp(int value);
    void setAttack(int value);
    void setDefense(int value);
    void setGold(int value);
    void addGold(int amount);

    void increaseMaxHp(int amount);
    void increaseMaxMp(int amount);
    void increaseAttack(int amount);
    void increaseDefense(int amount);
};
