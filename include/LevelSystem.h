#pragma once

#include "Hero.h"

/**
 * @struct StatGrowth
 * @brief Định nghĩa lượng chỉ số gia tăng khi Hero lên cấp.
 */
struct StatGrowth {
    int hpGrowth;
    int mpGrowth;
    int atkGrowth;
    int defGrowth;
};

/**
 * @class LevelSystem
 * @brief Hệ thống quản lý công thức EXP, thăng cấp và tăng trưởng chỉ số.
 */
class LevelSystem {
public:
    /**
     * @brief Tính lượng EXP cần thiết để thăng từ currentLevel lên currentLevel + 1.
     * Công thức bậc hai: EXP(L) = 50 * L^2 + 50 * L
     * @param currentLevel Cấp độ hiện tại của Hero (>= 1).
     * @return Lượng EXP cần thiết để lên cấp kế tiếp.
     */
    static int getExpRequiredForLevel(int currentLevel);

    /**
     * @brief Tính tổng lượng EXP tích lũy cần thiết từ cấp 1 đến targetLevel.
     * @param targetLevel Cấp độ đích.
     * @return Tổng số EXP tích lũy.
     */
    static int getTotalExpForLevel(int targetLevel);

    /**
     * @brief Lấy bảng tỷ lệ tăng trưởng chỉ số theo hệ Hero.
     * @param heroClass Hệ của Hero (WARRIOR, MAGE, RANGER).
     * @return StatGrowth tương ứng.
     */
    static StatGrowth getGrowthForClass(HeroClass heroClass);

    /**
     * @brief Kiểm tra xem Hero hiện tại có đủ EXP để lên cấp hay chưa.
     * @param hero Đối tượng Hero cần kiểm tra.
     * @return true nếu exp >= expToNextLevel, ngược lại false.
     */
    static bool canLevelUp(const Hero& hero);

    /**
     * @brief Cộng EXP cho Hero và xử lý thăng cấp nếu đủ điều kiện.
     * Hỗ trợ lên nhiều cấp liên tục nếu nhận lượng lớn EXP, bảo lưu EXP dư.
     * @param hero Tham chiếu Hero nhận EXP.
     * @param amount Lượng EXP nhận được.
     * @return true nếu có ít nhất 1 lần lên cấp, ngược lại false.
     */
    static bool addExp(Hero& hero, int amount);

    /**
     * @brief Thực hiện thăng cấp cho Hero:
     * - Tăng level lên 1.
     * - Tăng chỉ số (maxHp, maxMp, attack, defense) dựa theo hệ Hero.
     * - Hồi phục 100% HP và MP.
     * @param hero Tham chiếu Hero được thăng cấp.
     */
    static void levelUp(Hero& hero);

    /**
     * @brief Tính toán lượng EXP nhận được khi tiêu diệt quái vật.
     * Công thức: BaseEXP * (1 + max(0, monsterLevel - heroLevel) * 0.1)
     * @param monsterLevel Cấp độ quái vật.
     * @param heroLevel Cấp độ hiện tại của Hero.
     * @param baseExp Lượng EXP gốc của quái vật.
     * @return Lượng EXP thực tế Hero nhận được.
     */
    static int calculateMonsterExpReward(int monsterLevel, int heroLevel, int baseExp);
};
