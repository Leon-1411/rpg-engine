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
    int attack;
    int defense;
    int armorPenetration;
    float critChance;
    float critDamage;
    bool ignoreArmor;

    // Combat-specific resources and states
    int readyArrows;                  // Ranger's ready arrows
    std::vector<int> skillCooldowns;  // Current CD for skill 1, 2, 3
    std::vector<int> maxCooldowns;    // Max CD for skill 1, 2, 3
    bool isParrying;                  // Warrior skill 2
    bool isBlocking;                  // Warrior skill 3
    bool isEvading;                   // Ranger skill 1
    int skillLockTurns;               // Turn penalty if parry fails

public:
    // Auto-setup based on class design
    Hero(const std::string& name, HeroClass heroClass);

    // Legacy constructor matching main.cpp: (name, heroClass, hp, mp, attack, defense)
    Hero(const std::string& name, HeroClass heroClass, int hp, int mp, int attack, int defense);

    // Full custom setup
    Hero(const std::string& name, HeroClass heroClass, int hp, int attack, int defense,
         int armorPen = 10, float critChance = 0.25f, float critDamage = 0.25f, bool ignoreArmor = false);

    virtual ~Hero() = default;

    // Core actions
    virtual int normalAttack();
    virtual bool useSkill(int skillIndex, int& outDamage, std::string& outMessage);
    virtual void takeDamage(int damage);
    virtual void heal(int amount);
    virtual void addExp(int amount);
    virtual void levelUp();

    // Cooldown & stance helpers
    void reduceCooldowns();
    bool isSkillReady(int skillIndex) const;
    int getSkillCooldown(int skillIndex) const;
    void resetCombatStances();
    void lockSkills(int turns);

    // Status checks
    bool isAlive() const;
    void displayStats() const;

    // Getters & Setters
    std::string getName() const;
    HeroClass getHeroClass() const;
    std::string getHeroClassName() const;
    int getLevel() const;
    int getExp() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttack() const;
    int getDefense() const;
    int getArmorPenetration() const;
    float getCritChance() const;
    float getCritDamage() const;
    bool isIgnoreArmor() const;

    int getReadyArrows() const;
    void addReadyArrows(int amount);
    void setReadyArrows(int value);

    bool getIsParrying() const;
    void setIsParrying(bool value);
    bool getIsBlocking() const;
    void setIsBlocking(bool value);
    bool getIsEvading() const;
    void setIsEvading(bool value);
    int getSkillLockTurns() const;

    void setHp(int value);
    void setLevel(int value);
    void setExp(int value);
};
