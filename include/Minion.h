#pragma once

#include "Enemy.h"
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <vector>

enum class MinionType {
    GOBLIN,
    SKELETON,
    ORC,
    DARK_KNIGHT,
    UNKNOWN
};

std::string minionTypeToString(MinionType type);
MinionType stringToMinionType(const std::string& str);

class Minion : public Enemy {
protected:
    std::string id;
    MinionType minionType;
    std::string description;
    std::string specialSkillName;
    int actionCounter;

public:
    Minion(const std::string& id,
           const std::string& name,
           MinionType minionType,
           int hp,
           int attack,
           int defense,
           int expReward,
           int goldReward,
           const std::string& description = "",
           const std::string& specialSkill = "");

    virtual ~Minion() = default;

    int chooseAction() override;
    void displayStats() const override;
    std::string getSpecialSkillName() const override;

    std::string getId() const;
    MinionType getMinionType() const;
    std::string getDescription() const;
};

// 1. Goblin: Agile minion with chance for Quick Strike
class Goblin : public Minion {
public:
    explicit Goblin(const std::string& id = "goblin",
                    const std::string& name = "Goblin",
                    int hp = 60,
                    int attack = 12,
                    int defense = 5,
                    int expReward = 40,
                    int goldReward = 10,
                    const std::string& description = "A small, cunning creature that strikes quickly from the shadows.",
                    const std::string& specialSkill = "Quick Strike");

    int chooseAction() override;
};

// 2. Skeleton: Undead warrior using piercing attacks
class Skeleton : public Minion {
public:
    explicit Skeleton(const std::string& id = "skeleton",
                      const std::string& name = "Skeleton Warrior",
                      int hp = 80,
                      int attack = 18,
                      int defense = 8,
                      int expReward = 65,
                      int goldReward = 20,
                      const std::string& description = "An undead soldier wielding a rusty sword with relentless determination.",
                      const std::string& specialSkill = "Bone Pierce");

    int chooseAction() override;
};

// 3. Orc: Brute attacker that goes berserk when low on HP
class Orc : public Minion {
public:
    explicit Orc(const std::string& id = "orc",
                 const std::string& name = "Orc Berserker",
                 int hp = 120,
                 int attack = 25,
                 int defense = 12,
                 int expReward = 100,
                 int goldReward = 35,
                 const std::string& description = "A muscular beast fueled by bloodlust, hitting harder when enraged.",
                 const std::string& specialSkill = "Heavy Slam");

    int chooseAction() override;
};

// 4. Dark Knight: Armored elite minion with dark magic strike
class DarkKnight : public Minion {
public:
    explicit DarkKnight(const std::string& id = "dark_knight",
                        const std::string& name = "Dark Knight",
                        int hp = 160,
                        int attack = 32,
                        int defense = 18,
                        int expReward = 150,
                        int goldReward = 60,
                        const std::string& description = "A fallen warrior encased in cursed armor, wielding dark magic.",
                        const std::string& specialSkill = "Shadow Slash");

    int chooseAction() override;
};

// Factory and JSON loader for Minions
class MinionFactory {
public:
    static std::shared_ptr<Minion> createFromJsonObject(const std::string& id, const nlohmann::json& j);
    static std::shared_ptr<Minion> createFromJson(const std::string& id, const std::string& filepath = "data/enemies.json");
    static std::vector<std::shared_ptr<Minion>> loadAllFromJson(const std::string& filepath = "data/enemies.json");
    static bool saveMinionConfig(const std::string& filepath, const std::vector<std::shared_ptr<Minion>>& minions);
    static nlohmann::json minionToJson(const Minion& minion);
};
