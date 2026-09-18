/**
 * @file LevelSystem.cpp
 * @brief Hiện thực logic cho LevelSystem: tính toán EXP, thăng cấp, tăng chỉ số và phân bổ điểm tiềm năng.
 */

#include "LevelSystem.h"
#include <algorithm>
#include <iostream>

int LevelSystem::getExpRequiredForLevel(int currentLevel) {
    if (currentLevel < 1) {
        currentLevel = 1;
    }
    // Công thức đa thức bậc hai: EXP(L) = 50 * L^2 + 50 * L
    return 50 * currentLevel * currentLevel + 50 * currentLevel;
}

int LevelSystem::getTotalExpForLevel(int targetLevel) {
    if (targetLevel <= 1) {
        return 0;
    }
    int total = 0;
    for (int lvl = 1; lvl < targetLevel; ++lvl) {
        total += getExpRequiredForLevel(lvl);
    }
    return total;
}

StatGrowth LevelSystem::getGrowthForClass(HeroClass heroClass) {
    switch (heroClass) {
        case HeroClass::WARRIOR:
            // Ưu tiên HP và DEF cao
            return { 25, 3, 3, 3 };
        case HeroClass::MAGE:
            // Ưu tiên MP và ATK cao
            return { 10, 20, 5, 1 };
        case HeroClass::RANGER:
            // Chỉ số cân bằng, thiên về ATK nhanh
            return { 15, 8, 4, 2 };
        default:
            return { 15, 5, 3, 1 };
    }
}

bool LevelSystem::canLevelUp(const Hero& hero) {
    return hero.getExp() >= getExpRequiredForLevel(hero.getLevel());
}

bool LevelSystem::addExp(Hero& hero, int amount) {
    if (amount <= 0) {
        return false;
    }

    int currentExp = hero.getExp() + amount;
    bool leveledUp = false;

    while (true) {
        int required = getExpRequiredForLevel(hero.getLevel());
        if (currentExp >= required) {
            currentExp -= required;
            hero.setExp(currentExp);
            hero.levelUp();
            leveledUp = true;
        } else {
            break;
        }
    }

    hero.setExp(currentExp);
    return leveledUp;
}

void LevelSystem::levelUp(Hero& hero) {
    hero.setLevel(hero.getLevel() + 1);

    StatGrowth growth = getGrowthForClass(hero.getHeroClass());
    hero.increaseMaxHp(growth.hpGrowth);
    hero.increaseMaxMp(growth.mpGrowth);
    hero.increaseAttack(growth.atkGrowth);
    hero.increaseDefense(growth.defGrowth);

    // Hồi phục 100% máu và mana khi lên cấp
    hero.setHp(hero.getMaxHp());
    hero.setMp(hero.getMaxMp());

    // Thưởng 3 Điểm Tiềm Năng để người chơi tự do cộng điểm
    hero.addStatPoints(3);
}

void LevelSystem::promptStatAllocation(Hero& hero, std::istream& in, std::ostream& out) {
    while (hero.getStatPoints() > 0) {
        out << "\n==================================================\n"
            << "          PHÂN BỔ ĐIỂM THUỘC TÍNH (LEVEL UP)      \n"
            << "==================================================\n"
            << " Anh hùng: " << hero.getName() << " [" << hero.getHeroClassName() << " Lv." << hero.getLevel() << "]\n"
            << " Điểm tiềm năng khả dụng: [ " << hero.getStatPoints() << " ] điểm\n"
            << "--------------------------------------------------\n"
            << " 1. Sức Mạnh (Strength)       : +2 Tấn công (ATK) / điểm\n"
            << " 2. Phép Thuật (Intelligence) : +10 Năng lượng (MP) / điểm\n"
            << " 3. Thể Lực / Giáp (Vitality) : +15 Máu (HP) & +1 Giáp (DEF) / điểm\n"
            << " 4. Để dành điểm (Lưu lại phân bổ sau)\n"
            << "--------------------------------------------------\n"
            << "Chọn thuộc tính muốn tăng [1-4]: ";

        int choice = 0;
        if (!(in >> choice)) {
            break;
        }

        if (choice == 1) {
            hero.allocateStrength(1);
            out << "-> Đã tăng +2 Tấn công! (ATK hiện tại: " << hero.getAttack() << ")\n";
        } else if (choice == 2) {
            hero.allocateIntelligence(1);
            out << "-> Đã tăng +10 Năng lượng! (MP hiện tại: " << hero.getMaxMp() << ")\n";
        } else if (choice == 3) {
            hero.allocateVitality(1);
            out << "-> Đã tăng +15 Máu & +1 Giáp! (HP: " << hero.getMaxHp() << ", DEF: " << hero.getDefense() << ")\n";
        } else if (choice == 4) {
            out << "-> Đã lưu lại " << hero.getStatPoints() << " điểm tiềm năng.\n";
            break;
        } else {
            out << "[LỖI] Vui lòng nhập từ 1 đến 4!\n";
        }
    }
}

int LevelSystem::calculateMonsterExpReward(int monsterLevel, int heroLevel, int baseExp) {
    if (baseExp <= 0) {
        return 0;
    }
    int levelDifference = std::max(0, monsterLevel - heroLevel);
    double bonusMultiplier = 1.0 + (levelDifference * 0.1);
    return static_cast<int>(baseExp * bonusMultiplier);
}
