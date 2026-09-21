#pragma once

#include "Hero.h"

/**
 * @class Warrior
 * @brief Lớp Chiến binh (Arthur): Lượng HP và DEF cao, cận chiến vững chắc.
 *
 * Bộ kỹ năng:
 *  1. Power Slash: Tốn 10 MP - Đòn chém uy lực gây 200% sát thương ATK.
 *  2. Shield Block: Tốn 12 MP - Dùng khiên phản kích gây (ATK + DEF*2) damage và kích hoạt thế thủ.
 *  3. Berserk: Tốn 15 MP - Đòn cuồng nộ liều mạng gây 300% sát thương ATK.
 */
class Warrior : public Hero {
public:
    Warrior(const std::string& name, int hp = 120, int mp = 30, int attack = 18, int defense = 8);
    virtual ~Warrior() = default;
    using Hero::useSkill;

    bool useSkill(int skillIndex, int& outDamage, std::string& outMessage) override;
    bool useSkill(int skillIndex, int& outDamage) override;
    void levelUp() override;
    std::string getSkillName(int skillIndex) const override;
    void displaySkills() const override;
};
