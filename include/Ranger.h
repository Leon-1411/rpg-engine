#pragma once

#include "Hero.h"

/**
 * @class Ranger
 * @brief Lớp Xạ thủ (Lyra): Nhanh nhẹn, Crit/Dodge cao, do thám đường tắt.
 *
 * Bộ kỹ năng:
 *  1. Double Shot: Tốn 12 MP - Bắn 2 mũi tên liên tiếp gây 180% sát thương ATK.
 *  2. Poison Arrow: Tốn 18 MP - Tên độc gây (ATK*2 + 10) sát thương và gài độc DoT.
 *  3. Rain of Arrows: Tốn 25 MP - Bão tên áp đảo gây 300% sát thương ATK.
 */
class Ranger : public Hero {
public:
    Ranger(const std::string& name, int hp = 95, int mp = 50, int attack = 20, int defense = 5);
    virtual ~Ranger() = default;

    bool useSkill(int skillIndex, int& outDamage, std::string& outMessage) override;
    bool useSkill(int skillIndex, int& outDamage) override;
    void levelUp() override;
    std::string getSkillName(int skillIndex) const override;
    void displaySkills() const override;
};
