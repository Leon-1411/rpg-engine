#include "ui/BattleUI.h"
#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include <iostream>
#include <iomanip>

static std::string heroClassToString(HeroClass c) {
    switch (c) {
        case HeroClass::WARRIOR: return "Warrior";
        case HeroClass::MAGE:    return "Mage";
        case HeroClass::RANGER:  return "Ranger";
        default:                 return "Unknown";
    }
}

void BattleUI::renderBattleScreen(const Hero& hero, const Enemy& enemy, const std::string& battleMessage) {
    ConsoleUI::clearScreen();
    ASCIIArt::printBattleBanner();

    // Enemy art
    if (enemy.getType() == EnemyType::BOSS) {
        ASCIIArt::printBossDragonArt();
    } else {
        ASCIIArt::printGoblinArt();
    }

    ConsoleUI::printDivider('=', 64, ConsoleUI::Colors::BRIGHT_RED);

    // Hero Status (Left side) & Enemy Status (Right side)
    std::string heroHeader = hero.getName() + " (" + heroClassToString(hero.getHeroClass()) + " Lv." + std::to_string(hero.getLevel()) + ")";
    std::string enemyHeader = enemy.getName() + (enemy.getType() == EnemyType::BOSS ? " [BOSS]" : " [Minion]");

    std::cout << "  " 
              << ConsoleUI::colorize(heroHeader, ConsoleUI::Colors::BRIGHT_GREEN)
              << std::string(std::max(2, 34 - static_cast<int>(heroHeader.length())), ' ')
              << ConsoleUI::colorize(enemyHeader, ConsoleUI::Colors::BRIGHT_RED)
              << "\n";

    // HP Bars
    std::string heroHpBar = ConsoleUI::formatProgressBar(hero.getHp(), hero.getMaxHp(), 14);
    std::string enemyHpBar = ConsoleUI::formatProgressBar(enemy.getHp(), enemy.getMaxHp(), 14, ConsoleUI::Colors::BRIGHT_RED);

    std::cout << "  HP: " << heroHpBar
              << std::string(std::max(2, 30 - 24), ' ')
              << "HP: " << enemyHpBar << "\n";

    // MP Bar for Hero
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
              << "    " << ConsoleUI::colorize("3. Vật phẩm (Item)", ConsoleUI::Colors::BRIGHT_YELLOW) << "\n"
              << "  " << ConsoleUI::colorize("4. Phòng thủ (Defend)", ConsoleUI::Colors::CYAN)
              << "   " << ConsoleUI::colorize("5. Bỏ chạy (Run)", ConsoleUI::Colors::DIM) << "\n\n";
}

BattleAction BattleUI::getPlayerAction() {
    int choice = ConsoleUI::getIntInput(1, 5, "Chọn hành động của bạn [1-5]: ");
    return static_cast<BattleAction>(choice);
}

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
    ConsoleUI::pause("Trò chơi kết thúc. Nhấn Enter...");
}
