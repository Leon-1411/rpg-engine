#pragma once

#include <algorithm>

/**
 * @class DamageCalculator
 * @brief Chịu trách nhiệm tập trung cho mọi công thức tính toán sát thương trong game (PROJECT_SPEC mục 7).
 */
class DamageCalculator {
public:
    /**
     * @brief Tính sát thương thực tế sau khi đã trừ giáp và xét trạng thái thủ (Defend).
     * @param attackerAttack Lực tấn công cơ bản của bên đánh
     * @param defenderDefense Chỉ số phòng thủ của bên nhận
     * @param isDefending Trạng thái thủ (giảm 50% sát thương nhận vào)
     * @param skillMultiplier Hệ số kỹ năng (mặc định 1.0)
     * @return Lượng sát thương thực tế (tối thiểu 1)
     */
    static int calculateEffectiveDamage(int attackerAttack, int defenderDefense, bool isDefending = false, double skillMultiplier = 1.0) {
        int rawDamage = static_cast<int>(attackerAttack * skillMultiplier);
        int defenseBonus = isDefending ? (defenderDefense * 2) : defenderDefense;
        int damage = rawDamage - defenseBonus;
        if (isDefending && damage > 1) {
            damage = std::max(1, damage / 2);
        }
        return std::max(1, damage);
    }

    /**
     * @brief Tính lượng sát thương thô (raw power) có nhân hệ số kỹ năng.
     */
    static int calculateRawSkillDamage(int attackerAttack, double skillMultiplier) {
        return std::max(1, static_cast<int>(attackerAttack * skillMultiplier));
    }
};
