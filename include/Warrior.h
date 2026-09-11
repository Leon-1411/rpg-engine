#pragma once

#include "Hero.h"

/**
 * @class Warrior
 * @brief Lớp Chiến binh: Lượng HP và DEF cao, cận chiến mạnh mẽ.
 *
 * Bộ kỹ năng:
 *  1. Power Slash: Tốn 10 MP - Đòn chém uy lực gây 200% sát thương ATK.
 *  2. Shield Block: Tốn 12 MP - Dùng khiên phản kích gây sát thương ATK + DEF * 2.
 *  3. Berserk: Tốn 15 MP - Đòn cuồng nộ liều mạng gây 300% sát thương ATK.
 */
class Warrior : public Hero {
public:
    Warrior(const std::string& name, int hp = 120, int mp = 30, int attack = 18, int defense = 8);
    virtual ~Warrior() = default;

    bool useSkill(int skillIndex, int& outDamage) override;
    void levelUp() override;
    std::string getSkillName(int skillIndex) const override;
    void displaySkills() const override;
};
