#include "LevelSystem.h"
#include "Hero.h"
#include <cassert>
#include <iostream>

void testExpFormulas() {
    // EXP(L) = 50 * L^2 + 50 * L
    assert(LevelSystem::getExpRequiredForLevel(1) == 100);
    assert(LevelSystem::getExpRequiredForLevel(2) == 300);
    assert(LevelSystem::getExpRequiredForLevel(3) == 600);
    assert(LevelSystem::getExpRequiredForLevel(4) == 1000);

    // Total accumulated EXP
    assert(LevelSystem::getTotalExpForLevel(1) == 0);
    assert(LevelSystem::getTotalExpForLevel(2) == 100);
    assert(LevelSystem::getTotalExpForLevel(3) == 400); // 100 + 300
    assert(LevelSystem::getTotalExpForLevel(4) == 1000); // 100 + 300 + 600
}

void testAddExpAndSingleLevelUp() {
    Hero warrior("Galahad", HeroClass::WARRIOR, 100, 30, 20, 10);
    assert(warrior.getLevel() == 1);
    assert(warrior.getExp() == 0);
    assert(warrior.getExpToNextLevel() == 100);

    // Thêm EXP chưa đủ thăng cấp
    bool leveled = LevelSystem::addExp(warrior, 60);
    assert(!leveled);
    assert(warrior.getLevel() == 1);
    assert(warrior.getExp() == 60);
    assert(!LevelSystem::canLevelUp(warrior));

    // Thêm tiếp 40 EXP -> vừa đủ 100 EXP -> Lên Cấp 2
    leveled = LevelSystem::addExp(warrior, 40);
    assert(leveled);
    assert(warrior.getLevel() == 2);
    assert(warrior.getExp() == 0);
    assert(warrior.getExpToNextLevel() == 300);

    // Kiểm tra tăng trưởng chỉ số theo WARRIOR (HP +25, MP +3, ATK +3, DEF +3)
    assert(warrior.getMaxHp() == 125);
    assert(warrior.getMaxMp() == 33);
    assert(warrior.getAttack() == 23);
    assert(warrior.getDefense() == 13);
    assert(warrior.getHp() == 125); // Khôi phục đầy đủ
    assert(warrior.getMp() == 33);
}

void testMultiLevelJumpWithCarryOver() {
    Hero mage("Merlin", HeroClass::MAGE, 70, 80, 10, 2);
    assert(mage.getLevel() == 1);

    // Lvl 1 -> 2 cần 100 EXP
    // Lvl 2 -> 3 cần 300 EXP
    // Tổng để lên Lvl 3 là 400 EXP. Thưởng 450 EXP -> Lên Lvl 3 và dư 50 EXP.
    bool leveled = LevelSystem::addExp(mage, 450);
    assert(leveled);
    assert(mage.getLevel() == 3);
    assert(mage.getExp() == 50);
    assert(mage.getExpToNextLevel() == 600); // Cần 600 để lên Lvl 4

    // MAGE lên 2 cấp: (HP +10, MP +20, ATK +5, DEF +1) * 2
    assert(mage.getMaxHp() == 70 + 20);
    assert(mage.getMaxMp() == 80 + 40);
    assert(mage.getAttack() == 10 + 10);
    assert(mage.getDefense() == 2 + 2);
}

void testMonsterExpReward() {
    // Quái ngang cấp hoặc thấp hơn: nhận base EXP
    assert(LevelSystem::calculateMonsterExpReward(2, 2, 50) == 50);
    assert(LevelSystem::calculateMonsterExpReward(1, 3, 50) == 50);

    // Quái cao hơn 3 cấp (chênh lệch 3 -> thưởng 30%): 50 * 1.3 = 65
    assert(LevelSystem::calculateMonsterExpReward(5, 2, 50) == 65);

    // EXP gốc âm hoặc bằng 0
    assert(LevelSystem::calculateMonsterExpReward(5, 2, 0) == 0);
}

int main() {
    testExpFormulas();
    testAddExpAndSingleLevelUp();
    testMultiLevelJumpWithCarryOver();
    testMonsterExpReward();

    std::cout << "[PASS] LevelSystem unit tests successful!\n";
    return 0;
}
