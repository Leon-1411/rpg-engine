#pragma once

#include "Hero.h"
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
    int armorPenetration;
    float critChance;
    float critDamage;
    int expReward;
    int goldReward;

    // Poison DoT status
    int poisonTurns;
    int poisonDamagePerTurn;

    // Poisonous monster traits (inflicts poison on hit)
    bool isPoisonous;
    int poisonInflictTurns;
    int poisonInflictDmg;

    // Regeneration status
    int regenTurns;
    int regenPerTurn;

public:
    Enemy(const std::string& name, EnemyType type, int hp, int attack, int defense,
          int expReward, int goldReward, int armorPen = 5, float critChance = 0.10f, float critDamage = 0.25f);
    virtual ~Enemy() = default;

    // Actions
    virtual int chooseAction(); // Fallback action
    virtual int chooseAction(HeroClass targetClass); // Action considering target class miss rate
    virtual int chooseAction(HeroClass targetClass, int turnCount); // Pattern action considering turn count
    virtual void takeDamage(int damage);
    virtual void heal(int amount);
    bool isAlive() const;
    virtual void displayStats() const;
    virtual std::string getSpecialSkillName() const;

    // Poison mechanisms (Suffering poison)
    void applyPoison(int turns, int damagePerTurn);
    int takePoisonDamage();
    bool isPoisoned() const;
    int getPoisonTurns() const;

    // Poisonous monster traits (Inflicting poison on target)
    void setPoisonous(bool value, int turns = 3, int dmg = 5);
    bool getIsPoisonous() const;
    int getPoisonInflictTurns() const;
    int getPoisonInflictDmg() const;

    // Regeneration mechanisms
    void applyRegen(int turns, int healPerTurn);
    int processRegen();
    bool hasRegen() const;
    int getRegenTurns() const;

    // Getters & Setters
    std::string getName() const;
    EnemyType getType() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttack() const;
    int getDefense() const;
    int getArmorPenetration() const;
    float getCritChance() const;
    float getCritDamage() const;
    int getExpReward() const;
    int getGoldReward() const;

    void setHp(int value);
};
