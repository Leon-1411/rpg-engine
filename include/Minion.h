#pragma once

#include "Enemy.h"
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <vector>

enum class MinionType {
    WILD_MERCENARY,
    DEMON_SCOUT,
    DEMON_BERSERKER,
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

// ==================== 3 Canon Minions (Fractured Crown Lore) ====================

// 1. Wild Mercenary: Nomad hunter of Free People, ambushing enemies with Wild Ambush
class WildMercenary : public Minion {
public:
    explicit WildMercenary(const std::string& id = "Wild_Mercenary",
                           const std::string& name = "Wild Mercenary",
                           int hp = 90,
                           int attack = 20,
                           int defense = 8,
                           int expReward = 75,
                           int goldReward = 25,
                           const std::string& description = "Lính đánh thuê và thợ săn du mục của Bộ Tộc Tự Do, canh giữ ranh giới Rừng Rậm Huyết Nguyệt.",
                           const std::string& specialSkill = "Wild Ambush");

    int chooseAction() override;
};

// 2. Demon Scout: Agile shadow demon scouting borders with Shadow Dart
class DemonScout : public Minion {
public:
    explicit DemonScout(const std::string& id = "Demon_Scout",
                        const std::string& name = "Demon Scout",
                        int hp = 85,
                        int attack = 22,
                        int defense = 6,
                        int expReward = 80,
                        int goldReward = 30,
                        const std::string& description = "Trinh sát quỷ nhanh nhẹn của Ma tộc tuần tra vùng biên giới Trăng Máu.",
                        const std::string& specialSkill = "Shadow Dart");

    int chooseAction() override;
};

// 3. Demon Berserker: Savage frontline demon fighter entering Demonic Frenzy
class DemonBerserker : public Minion {
public:
    explicit DemonBerserker(const std::string& id = "Demon_Berserker",
                            const std::string& name = "Demon Berserker",
                            int hp = 140,
                            int attack = 30,
                            int defense = 14,
                            int expReward = 130,
                            int goldReward = 50,
                            const std::string& description = "Chiến binh quỷ cuồng nộ canh giữ lối vào Ma Điện khi người chơi đột kích trực diện.",
                            const std::string& specialSkill = "Demonic Frenzy");

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
