#pragma once

#include "Hero.h"

/**
 * @class Mage
 * @brief Lớp Pháp sư (Morrigan): Sở hữu lượng MP dồi dào, sát thương phép diện rộng và khắc chế ma thuật.
 *
 * Bộ kỹ năng:
 *  1. Fireball: Tốn 15 MP - Cầu lửa thiêu đốt gây (ATK*2 + 10) sát thương phép.
 *  2. Ice Blast: Tốn 20 MP - Hàn băng tiễn gây (ATK*2 + 20) sát thương phép.
 *  3. Meteor: Tốn 35 MP - Thiên thạch hủy diệt gây (ATK*3 + 40) sát thương cực lớn.
 */
class Mage : public Hero {
public:
    Mage(const std::string& name, int hp = 80, int mp = 100, int attack = 24, int defense = 3);
    virtual ~Mage() = default;

    bool useSkill(int skillIndex, int& outDamage, std::string& outMessage) override;
    bool useSkill(int skillIndex, int& outDamage) override;
    void levelUp() override;
    std::string getSkillName(int skillIndex) const override;
    void displaySkills() const override;
};
