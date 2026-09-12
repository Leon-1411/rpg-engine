#pragma once

#include <string>
#include <vector>
#include "Inventory.h"

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
    int mp;
    int maxMp;
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
    bool isDefending;                 // Active Defend action (halves incoming damage)
    int skillLockTurns;               // Turn penalty if parry fails

    // Status Effects (Poison DoT & Regeneration HoT)
    int poisonTurns;
    int poisonDamagePerTurn;
    int regenTurns;
    int regenPerTurn;

    Inventory inventory;

public:
    // Auto-setup based on class design
    Hero(const std::string& name, HeroClass heroClass);

    // Full custom setup
    Hero(const std::string& name, HeroClass heroClass, int hp, int attack, int defense);
    Hero(const std::string& name, HeroClass heroClass, int hp, int attack, int defense,
         int armorPen, float critChance, float critDamage, bool ignoreArmor = false);
    Hero(const std::string& name, HeroClass heroClass, int hp, int mp, int attack, int defense,
         int armorPen = 10, float critChance = 0.25f, float critDamage = 0.25f, bool ignoreArmor = false);

    virtual ~Hero() = default;

    // Core actions
    virtual int normalAttack();
    virtual bool useSkill(int skillIndex, int& outDamage, std::string& outMessage);
    virtual bool useSkill(int skillIndex, int& outDamage);
    virtual void takeDamage(int damage);
    virtual void heal(int amount);
    virtual void restoreMp(int amount);
    virtual bool addExp(int amount);
    virtual void levelUp();

    // Cooldown & stance helpers
    void reduceCooldowns();
    bool isSkillReady(int skillIndex) const;
    int getSkillCooldown(int skillIndex) const;
    void resetCombatStances();
    void lockSkills(int turns);

    // Status Effects
    void applyPoison(int turns, int damagePerTurn);
    int takePoisonDamage();
    bool isPoisoned() const;
    int getPoisonTurns() const;
    int getPoisonDamagePerTurn() const;

    void applyRegen(int turns, int healPerTurn);
    int processRegen();
    bool hasRegen() const;
    int getRegenTurns() const;
    int getRegenPerTurn() const;

    void clearStatusEffects();

    // Status checks
    bool isAlive() const;
    void displayStats() const;
    virtual std::string getSkillName(int skillIndex) const;
    virtual void displaySkills() const;

    // Getters & Setters
    std::string getName() const;
    HeroClass getHeroClass() const;
    std::string getHeroClassName() const;
    int getLevel() const;
    int getExp() const;
    int getExpToNextLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getMp() const;
    int getMaxMp() const;
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
    bool getIsDefending() const;
    void setIsDefending(bool value);
    int getSkillLockTurns() const;

    // Inventory & Equipment bonuses
    Inventory& getInventory();
    const Inventory& getInventory() const;
    int getEffectiveAttack() const;
    int getEffectiveDefense() const;

    void setHp(int value);
    void setMaxHp(int value);
    void setMp(int value);
    void setMaxMp(int value);
    void setLevel(int value);
    void setExp(int value);
    void setMaxHp(int value);
    void setMaxMp(int value);
    void setAttack(int value);
    void setDefense(int value);

    void increaseMaxHp(int amount);
    void increaseMaxMp(int amount);
    void increaseAttack(int amount);
    void increaseDefense(int amount);
};

