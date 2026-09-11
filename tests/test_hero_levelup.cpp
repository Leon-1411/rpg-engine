/**
 * @file test_hero_levelup.cpp
 * @brief Kiểm thử độc lập việc cộng chỉ số khi lên cấp cho Warrior, Mage và Ranger.
 */

#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "LevelSystem.h"
#include <cassert>
#include <iostream>
#include <iomanip>

void printDivider() {
    std::cout << "------------------------------------------------------------\n";
}

void printHeroStats(const std::string& title, const Hero& hero) {
    std::cout << std::left << std::setw(15) << title 
              << " | Lvl: " << hero.getLevel()
              << " | HP: " << std::setw(3) << hero.getHp() << "/" << std::setw(3) << hero.getMaxHp()
              << " | MP: " << std::setw(3) << hero.getMp() << "/" << std::setw(3) << hero.getMaxMp()
              << " | ATK: " << std::setw(2) << hero.getAttack()
              << " | DEF: " << std::setw(2) << hero.getDefense()
              << " | EXP: " << hero.getExp() << "/" << hero.getExpToNextLevel()
              << "\n";
}

void testWarriorLevelUp() {
    std::cout << "\n=== [TEST 1] WARRIOR LEVEL UP (HP +25, MP +3, ATK +3, DEF +3) ===\n";
    Warrior warrior("Arthur", 120, 30, 18, 8);
    printHeroStats("Trước khi lên cấp", warrior);

    assert(warrior.getLevel() == 1);
    assert(warrior.getMaxHp() == 120);
    assert(warrior.getMaxMp() == 30);
    assert(warrior.getAttack() == 18);
    assert(warrior.getDefense() == 8);

    // Gây sát thương để HP và MP giảm trước khi lên cấp
    warrior.takeDamage(30); // damage nhận: 30 - 8 = 22 -> HP còn 98
    int skillDmg = 0;
    bool skillUsed = warrior.useSkill(1, skillDmg); // Power Slash tốn 10 MP -> MP còn 20
    assert(skillUsed);
    assert(skillDmg == 36); // 18 * 2
    assert(warrior.getHp() == 98);
    assert(warrior.getMp() == 20);

    // Thực hiện levelUp
    warrior.levelUp();
    printHeroStats("Sau khi lên cấp", warrior);

    // Kiểm tra chỉ số cộng thêm chính xác theo bảng tăng trưởng Warrior
    assert(warrior.getLevel() == 2);
    assert(warrior.getMaxHp() == 120 + 25); // 145
    assert(warrior.getMaxMp() == 30 + 3);   // 33
    assert(warrior.getAttack() == 18 + 3);  // 21
    assert(warrior.getDefense() == 8 + 3);  // 11

    // Kiểm tra hồi phục 100% HP và MP khi thăng cấp
    assert(warrior.getHp() == 145);
    assert(warrior.getMp() == 33);

    // Kiểm tra bộ kỹ năng của Warrior ở cấp 2
    // Kỹ năng 2: Shield Block (tốn 12 MP, gây ATK + DEF*2 = 21 + 11*2 = 43)
    skillUsed = warrior.useSkill(2, skillDmg);
    assert(skillUsed);
    assert(skillDmg == 43);
    assert(warrior.getMp() == 21); // 33 - 12 = 21

    // Kỹ năng 3: Berserk (tốn 15 MP, gây ATK*3 = 21*3 = 63)
    skillUsed = warrior.useSkill(3, skillDmg);
    assert(skillUsed);
    assert(skillDmg == 63);
    assert(warrior.getMp() == 6);  // 21 - 15 = 6

    // Khi không đủ MP (6 < 10):
    skillUsed = warrior.useSkill(1, skillDmg);
    assert(!skillUsed);
    assert(skillDmg == 0);

    std::cout << "-> [PASS] Warrior level up & skills verified.\n";
}

void testMageLevelUp() {
    std::cout << "\n=== [TEST 2] MAGE LEVEL UP (HP +10, MP +20, ATK +5, DEF +1) ===\n";
    Mage mage("Gandalf", 70, 80, 14, 3);
    printHeroStats("Trước khi lên cấp", mage);

    assert(mage.getLevel() == 1);
    assert(mage.getMaxHp() == 70);
    assert(mage.getMaxMp() == 80);
    assert(mage.getAttack() == 14);
    assert(mage.getDefense() == 3);

    // Tiêu hao bớt HP và MP
    mage.takeDamage(20); // 20 - 3 = 17 dmg -> HP: 53
    mage.setMp(10);      // MP còn 10

    // Thực hiện levelUp
    mage.levelUp();
    printHeroStats("Sau khi lên cấp", mage);

    // Kiểm tra chỉ số cộng thêm chính xác cho Mage
    assert(mage.getLevel() == 2);
    assert(mage.getMaxHp() == 70 + 10); // 80
    assert(mage.getMaxMp() == 80 + 20); // 100
    assert(mage.getAttack() == 14 + 5); // 19
    assert(mage.getDefense() == 3 + 1); // 4

    // Hồi phục 100%
    assert(mage.getHp() == 80);
    assert(mage.getMp() == 100);

    // Kiểm tra kỹ năng Mage
    int skillDmg = 0;
    // Skill 1: Fireball (MP: 15, Dmg: ATK*2 + 10 = 19*2 + 10 = 48)
    bool ok = mage.useSkill(1, skillDmg);
    assert(ok && skillDmg == 48 && mage.getMp() == 85);

    // Skill 2: Ice Blast (MP: 20, Dmg: ATK*2 + 20 = 19*2 + 20 = 58)
    ok = mage.useSkill(2, skillDmg);
    assert(ok && skillDmg == 58 && mage.getMp() == 65);

    // Skill 3: Meteor (MP: 35, Dmg: ATK*3 + 40 = 19*3 + 40 = 97)
    ok = mage.useSkill(3, skillDmg);
    assert(ok && skillDmg == 97 && mage.getMp() == 30);

    std::cout << "-> [PASS] Mage level up & skills verified.\n";
}

void testRangerLevelUp() {
    std::cout << "\n=== [TEST 3] RANGER LEVEL UP (HP +15, MP +8, ATK +4, DEF +2) ===\n";
    Ranger ranger("Legolas", 90, 50, 16, 5);
    printHeroStats("Trước khi lên cấp", ranger);

    assert(ranger.getLevel() == 1);
    assert(ranger.getMaxHp() == 90);
    assert(ranger.getMaxMp() == 50);
    assert(ranger.getAttack() == 16);
    assert(ranger.getDefense() == 5);

    // Tiêu hao HP/MP
    ranger.takeDamage(25);
    ranger.setMp(5);

    // Thực hiện levelUp
    ranger.levelUp();
    printHeroStats("Sau khi lên cấp", ranger);

    // Kiểm tra chỉ số cộng thêm chính xác cho Ranger
    assert(ranger.getLevel() == 2);
    assert(ranger.getMaxHp() == 90 + 15); // 105
    assert(ranger.getMaxMp() == 50 + 8);  // 58
    assert(ranger.getAttack() == 16 + 4); // 20
    assert(ranger.getDefense() == 5 + 2); // 7

    // Hồi phục 100%
    assert(ranger.getHp() == 105);
    assert(ranger.getMp() == 58);

    // Kiểm tra kỹ năng Ranger
    int skillDmg = 0;
    // Skill 1: Double Shot (MP: 12, Dmg: 20 * 1.8 = 36)
    bool ok = ranger.useSkill(1, skillDmg);
    assert(ok && skillDmg == 36 && ranger.getMp() == 46);

    // Skill 2: Poison Arrow (MP: 18, Dmg: 20*2 + 10 = 50)
    ok = ranger.useSkill(2, skillDmg);
    assert(ok && skillDmg == 50 && ranger.getMp() == 28);

    // Skill 3: Rain of Arrows (MP: 25, Dmg: 20*3 = 60)
    ok = ranger.useSkill(3, skillDmg);
    assert(ok && skillDmg == 60 && ranger.getMp() == 3);

    std::cout << "-> [PASS] Ranger level up & skills verified.\n";
}

void testMultiLevelUpViaExp() {
    std::cout << "\n=== [TEST 4] MULTI-LEVEL JUMP VIA EXP ACCUMULATION ===\n";
    Warrior warrior("Hercules", 100, 30, 20, 10);
    printHeroStats("Cấp 1 ban đầu", warrior);

    // Cần 100 EXP để lên Cấp 2, 300 EXP để lên Cấp 3 (Tổng = 400 EXP)
    // Thêm 450 EXP -> Lên cấp 3, dư 50 EXP
    bool leveled = warrior.addExp(450);
    assert(leveled);
    assert(warrior.getLevel() == 3);
    assert(warrior.getExp() == 50);

    printHeroStats("Sau khi nhận 450 EXP (Lên Cấp 3)", warrior);

    // Warrior lên 2 cấp: (HP +25, MP +3, ATK +3, DEF +3) * 2
    assert(warrior.getMaxHp() == 100 + 50); // 150
    assert(warrior.getMaxMp() == 30 + 6);   // 36
    assert(warrior.getAttack() == 20 + 6);  // 26
    assert(warrior.getDefense() == 10 + 6); // 16
    assert(warrior.getHp() == 150);
    assert(warrior.getMp() == 36);

    std::cout << "-> [PASS] Multi-level jump and cumulative stats verified.\n";
}

void testPolymorphismThroughBasePointer() {
    std::cout << "\n=== [TEST 5] POLYMORPHISM THROUGH HERO BASE POINTER ===\n";
    Hero* heroes[3] = {
        new Warrior("W_Hero", 120, 30, 18, 8),
        new Mage("M_Hero", 70, 80, 14, 3),
        new Ranger("R_Hero", 90, 50, 16, 5)
    };

    for (int i = 0; i < 3; ++i) {
        heroes[i]->levelUp();
        assert(heroes[i]->getLevel() == 2);
    }

    // Verify each derived class got its specific growth
    assert(heroes[0]->getMaxHp() == 145); // Warrior: +25
    assert(heroes[1]->getMaxHp() == 80);  // Mage: +10
    assert(heroes[2]->getMaxHp() == 105); // Ranger: +15

    for (int i = 0; i < 3; ++i) {
        delete heroes[i];
    }
    std::cout << "-> [PASS] Polymorphism levelUp() works seamlessly via base pointer.\n";
}

int main() {
    std::cout << "============================================================\n";
    std::cout << "     STANDALONE TEST: HERO LEVEL UP & STAT ADDITIONS        \n";
    std::cout << "============================================================\n";

    testWarriorLevelUp();
    printDivider();
    testMageLevelUp();
    printDivider();
    testRangerLevelUp();
    printDivider();
    testMultiLevelUpViaExp();
    printDivider();
    testPolymorphismThroughBasePointer();

    std::cout << "\n============================================================\n";
    std::cout << " [ALL TESTS PASSED] Toàn bộ chỉ số cộng khi lên cấp CHÍNH XÁC!\n";
    std::cout << "============================================================\n";
    return 0;
}
