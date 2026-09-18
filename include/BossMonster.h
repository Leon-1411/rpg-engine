#pragma once

#include "Enemy.h"
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

class Minion;

/**
 * @struct BossSkill
 * @brief Represents a specialized Boss combat skill equipped with individual cooldown and damage scaling.
 */
struct BossSkill {
    std::string name;
    std::string description;
    int cooldown;            ///< Base cooldown turns between uses
    double damageMultiplier; ///< Damage scaling multiplier relative to base attack
    int currentCooldown;     ///< Remaining turns until the skill can be activated again

    BossSkill(const std::string& name = "Special Skill",
              const std::string& description = "",
              int cooldown = 3,
              double damageMultiplier = 1.4,
              int initialCooldown = 0)
        : name(name),
          description(description),
          cooldown(cooldown),
          damageMultiplier(damageMultiplier),
          currentCooldown(initialCooldown) {}

    bool isReady() const {
        return currentCooldown <= 0;
    }

    void trigger() {
        currentCooldown = cooldown;
    }

    void resetCooldown() {
        currentCooldown = 0;
    }

    void updateCooldown(int turns = 1) {
        currentCooldown = std::max(0, currentCooldown - turns);
    }
};

class BossMonster : public Enemy {
protected:
    std::string id;
    std::string description;
    std::string specialSkillName;
    int baseAttack;
    int baseDefense;
    bool enraged;
    int healAmount;
    int actionCounter;

    std::vector<BossSkill> skills;
    int activeSkillIndex;

    void initializeDefaultSkills(const std::string& primarySkill);

public:
    // Default constructor: 5x average minion stats
    explicit BossMonster(const std::string& id = "dragon_lord",
                         const std::string& name = "Dragon Lord",
                         int hp = 525,
                         int attack = 105,
                         int defense = 50,
                         int expReward = 450,
                         int goldReward = 150,
                         const std::string& description = "Ancient dread sovereign with crushing power and fiery fury.",
                         const std::string& specialSkill = "Infernal Cataclysm",
                         int healAmount = 25);

    virtual ~BossMonster() = default;

    // Overridden actions
    int chooseAction() override;
    void takeDamage(int damage) override;
    void displayStats() const override;
    std::string getSpecialSkillName() const override;

    // Enrage and healing mechanics
    bool checkEnrage();
    void triggerEnrage();
    void heal(int amount);
    void regenerate();
    void resetEnrage();

    // Skill Cooldown System
    const std::vector<BossSkill>& getSkills() const;
    std::vector<BossSkill>& getSkills();
    const BossSkill* getSkill(size_t index) const;
    BossSkill* getSkill(size_t index);
    bool isSkillReady(size_t index) const;
    int getSkillRemainingCooldown(size_t index) const;
    int getSkillBaseCooldown(size_t index) const;
    void setSkillCooldown(size_t index, int turns);
    void updateCooldowns(int amount = 1);
    void resetCooldowns();
    bool useSkill(size_t index);
    void setSkills(const std::vector<BossSkill>& newSkills);
    void addSkill(const BossSkill& skill);
    int getActiveSkillIndex() const;
    const BossSkill* getActiveSkill() const;
    double getActiveSkillMultiplier() const;

    // Getters and Setters
    std::string getId() const;
    std::string getDescription() const;
    bool isEnraged() const;
    int getBaseAttack() const;
    int getBaseDefense() const;
    int getHealAmount() const;
    void setHealAmount(int amount);

    // Factory method: creates a Boss with stats exactly 5x of a given Minion
    static std::shared_ptr<BossMonster> createScaledFromMinion(
        const Minion& minion,
        const std::string& bossId = "",
        const std::string& bossName = "",
        const std::string& specialSkill = "",
        int healAmount = 0);
};

// Convenience alias
using Boss = BossMonster;

// Factory and JSON loader for Bosses
class BossFactory {
public:
    static std::shared_ptr<BossMonster> createFromJsonObject(const std::string& id, const nlohmann::json& j);
    static std::shared_ptr<BossMonster> createFromJson(const std::string& id, const std::string& filepath = "data/enemies.json");
    static std::vector<std::shared_ptr<BossMonster>> loadAllFromJson(const std::string& filepath = "data/enemies.json");
    static nlohmann::json bossToJson(const BossMonster& boss);
};
