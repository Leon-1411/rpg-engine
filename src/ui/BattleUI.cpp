#include "ui/BattleUI.h"
#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
<<<<<<< HEAD
#include "CombatEngine.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

static std::string heroClassToString(HeroClass c) {
    switch (c) {
        case HeroClass::WARRIOR: return "Chiến Binh (Warrior)";
        case HeroClass::MAGE:    return "Pháp Sư (Mage)";
        case HeroClass::RANGER:  return "Xạ Thủ (Ranger)";
        default:                 return "Hiệp Sĩ";
=======
#include <iostream>
#include <iomanip>

static std::string heroClassToString(HeroClass c) {
    switch (c) {
        case HeroClass::WARRIOR: return "Warrior";
        case HeroClass::MAGE:    return "Mage";
        case HeroClass::RANGER:  return "Ranger";
        default:                 return "Unknown";
>>>>>>> origin/main
    }
}

void BattleUI::renderBattleScreen(const Hero& hero, const Enemy& enemy, const std::string& battleMessage) {
    ConsoleUI::clearScreen();
    ASCIIArt::printBattleBanner();

<<<<<<< HEAD
    // Display Hero, VS, and Enemy/Boss ASCII Art
    ASCIIArt::printBattleVersus(hero, enemy);
=======
    // Enemy art
    if (enemy.getType() == EnemyType::BOSS) {
        ASCIIArt::printBossDragonArt();
    } else {
        ASCIIArt::printGoblinArt();
    }
>>>>>>> origin/main

    ConsoleUI::printDivider('=', 64, ConsoleUI::Colors::BRIGHT_RED);

    // Hero Status (Left side) & Enemy Status (Right side)
    std::string heroHeader = hero.getName() + " (" + heroClassToString(hero.getHeroClass()) + " Lv." + std::to_string(hero.getLevel()) + ")";
<<<<<<< HEAD
    std::string enemyHeader = enemy.getName() + (enemy.getType() == EnemyType::BOSS ? " [BOSS TỐI CAO]" : " [Minion]");
=======
    std::string enemyHeader = enemy.getName() + (enemy.getType() == EnemyType::BOSS ? " [BOSS]" : " [Minion]");
>>>>>>> origin/main

    std::cout << "  " 
              << ConsoleUI::colorize(heroHeader, ConsoleUI::Colors::BRIGHT_GREEN)
              << std::string(std::max(2, 34 - static_cast<int>(heroHeader.length())), ' ')
              << ConsoleUI::colorize(enemyHeader, ConsoleUI::Colors::BRIGHT_RED)
              << "\n";

<<<<<<< HEAD
    // HP Bars: Rendered in BRIGHT_RED for both Hero and Enemy/Boss
    std::string heroHpBar = ConsoleUI::formatProgressBar(hero.getHp(), hero.getMaxHp(), 14, ConsoleUI::Colors::BRIGHT_RED);
=======
    // HP Bars
    std::string heroHpBar = ConsoleUI::formatProgressBar(hero.getHp(), hero.getMaxHp(), 14);
>>>>>>> origin/main
    std::string enemyHpBar = ConsoleUI::formatProgressBar(enemy.getHp(), enemy.getMaxHp(), 14, ConsoleUI::Colors::BRIGHT_RED);

    std::cout << "  HP: " << heroHpBar
              << std::string(std::max(2, 30 - 24), ' ')
              << "HP: " << enemyHpBar << "\n";

<<<<<<< HEAD
    // MP Bar: Rendered in BRIGHT_BLUE for Hero
=======
    // MP Bar for Hero
>>>>>>> origin/main
    std::string heroMpBar = ConsoleUI::formatProgressBar(hero.getMp(), hero.getMaxMp(), 14, ConsoleUI::Colors::BRIGHT_BLUE);
    std::cout << "  MP: " << heroMpBar << "\n";

    ConsoleUI::printDivider('-', 64, ConsoleUI::Colors::DIM);

    // Event/Log message
    if (!battleMessage.empty()) {
        std::cout << "\n  " << ConsoleUI::colorize("▶ ", ConsoleUI::Colors::BRIGHT_YELLOW)
                  << battleMessage << "\n\n";
        ConsoleUI::printDivider('-', 64, ConsoleUI::Colors::DIM);
    }

    // Battle Actions Menu
    std::cout << "\n  " << ConsoleUI::colorize("1. Tấn công (Attack)", ConsoleUI::Colors::BRIGHT_RED)
              << "    " << ConsoleUI::colorize("2. Kỹ năng (Skill)", ConsoleUI::Colors::BRIGHT_BLUE)
<<<<<<< HEAD
              << "    " << ConsoleUI::colorize("3. Dược phẩm (Item)", ConsoleUI::Colors::BRIGHT_YELLOW) << "\n"
=======
              << "    " << ConsoleUI::colorize("3. Vật phẩm (Item)", ConsoleUI::Colors::BRIGHT_YELLOW) << "\n"
>>>>>>> origin/main
              << "  " << ConsoleUI::colorize("4. Phòng thủ (Defend)", ConsoleUI::Colors::CYAN)
              << "   " << ConsoleUI::colorize("5. Bỏ chạy (Run)", ConsoleUI::Colors::DIM) << "\n\n";
}

<<<<<<< HEAD
BattleAction BattleUI::getPlayerAction(std::istream& in) {
    int choice = ConsoleUI::getIntInput(1, 5, "Chọn hành động chiến đấu [1-5]: ", in);
    return static_cast<BattleAction>(choice);
}

CombatState BattleUI::runBattle(Hero& hero, Enemy& enemy, std::istream& in) {
    CombatEngine engine(hero, enemy);
    engine.startBattle();

    std::string lastMessage = "Một kẻ địch đã xuất hiện: " + enemy.getName() + "!";
    if (enemy.getType() == EnemyType::BOSS) {
        lastMessage = "CẢNH BÁO NGUY HIỂM: TRẬN CHIẾN BOSS TỐI CAO - " + enemy.getName() + " BẮT ĐẦU!";
    }

    while (!engine.isBattleOver()) {
        renderBattleScreen(hero, enemy, lastMessage);
        BattleAction action = getPlayerAction(in);

        bool isDefending = false;
        if (action == BattleAction::ATTACK) {
            int dmg = engine.calculateDamage(hero.getAttack(), enemy.getDefense());
            enemy.takeDamage(hero.getAttack());
            lastMessage = hero.getName() + " vung vũ khí tấn công " + enemy.getName() + " gây " + std::to_string(dmg) + " sát thương!";
        } else if (action == BattleAction::SKILL) {
            int skillDmg = 0;
            if (hero.useSkill(1, skillDmg)) {
                int dmg = engine.calculateDamage(skillDmg, enemy.getDefense());
                enemy.takeDamage(skillDmg);
                lastMessage = hero.getName() + " thi triển KỸ NĂNG TẤT SÁT, oanh tạc " + enemy.getName() + " với " + std::to_string(dmg) + " sát thương cực đại!";
            } else {
                lastMessage = "Không đủ điểm Mana (MP) để kích hoạt kỹ năng đặc biệt!";
                continue;
            }
        } else if (action == BattleAction::ITEM) {
            hero.heal(35);
            hero.restoreMp(25);
            lastMessage = hero.getName() + " sử dụng Bình Dược Phẩm, hồi phục 35 HP (đỏ) và 25 MP (xanh)!";
        } else if (action == BattleAction::DEFEND) {
            isDefending = true;
            lastMessage = hero.getName() + " lập phòng tuyến kiên cố, giảm 50% sát thương nhận vào trong lượt này!";
        } else if (action == BattleAction::RUN) {
            if (enemy.getType() == EnemyType::BOSS) {
                lastMessage = "Không thể đào tẩu khỏi trận chiến định mệnh với Trùm Cuối (Boss)!";
                continue;
            } else {
                renderBattleScreen(hero, enemy, hero.getName() + " đã nhanh nhẹn rút lui an toàn khỏi trận chiến!");
                ConsoleUI::pause();
                return CombatState::FLED;
            }
        }

        // Check if enemy defeated
        if (!enemy.isAlive()) {
            renderBattleScreen(hero, enemy, enemy.getName() + " đã bị tiêu diệt hoàn toàn!");
            hero.addExp(enemy.getExpReward());
            showVictory(enemy);
            return CombatState::HERO_VICTORY;
        }

        // Enemy turn
        int enemyDmg = engine.calculateDamage(enemy.getAttack(), hero.getDefense());
        if (isDefending) {
            enemyDmg = std::max(1, enemyDmg / 2);
            hero.takeDamage(enemy.getAttack() / 2);
        } else {
            hero.takeDamage(enemy.getAttack());
        }
        lastMessage += "\n  " + enemy.getName() + " phản kích dồn dập, gây " + std::to_string(enemyDmg) + " sát thương lên " + hero.getName() + "!";

        if (!hero.isAlive()) {
            renderBattleScreen(hero, enemy, hero.getName() + " đã kiệt sức và ngã xuống trên chiến trường...");
            showDefeat();
            return CombatState::ENEMY_VICTORY;
        }
    }

    return engine.getState();
}

=======
BattleAction BattleUI::getPlayerAction() {
    int choice = ConsoleUI::getIntInput(1, 5, "Chọn hành động của bạn [1-5]: ");
    return static_cast<BattleAction>(choice);
}

>>>>>>> origin/main
void BattleUI::printCombatLog(const std::string& message) {
    std::cout << "  " << ConsoleUI::colorize("» ", ConsoleUI::Colors::CYAN) << message << "\n";
}

void BattleUI::showVictory(const Enemy& enemy) {
    std::cout << "\n";
    ASCIIArt::printVictoryBanner();
    std::cout << "  " << ConsoleUI::colorize("+ Nhận được: ", ConsoleUI::Colors::BRIGHT_YELLOW)
              << ConsoleUI::colorize(std::to_string(enemy.getExpReward()) + " EXP", ConsoleUI::Colors::BRIGHT_GREEN)
              << " và "
              << ConsoleUI::colorize(std::to_string(enemy.getGoldReward()) + " Vàng", ConsoleUI::Colors::BRIGHT_YELLOW)
              << "!\n";
    ConsoleUI::pause();
}

void BattleUI::showDefeat() {
    std::cout << "\n";
    ASCIIArt::printGameOverBanner();
<<<<<<< HEAD
    ConsoleUI::pause("Trò chơi kết thúc. Nhấn Enter để quay lại...");
=======
    ConsoleUI::pause("Trò chơi kết thúc. Nhấn Enter...");
>>>>>>> origin/main
}
