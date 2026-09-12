#include "ui/ASCIIArt.h"
#include "ui/ConsoleUI.h"
#include <iostream>

namespace ASCIIArt {

void printTitleLogo() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
   ██████╗ ██████╗  ██████╗     ███████╗███╗   ██╗ ██████╗ ██╗███╗   ██╗███████╗
   ██╔══██╗██╔══██╗██╔════╝     ██╔════╝████╗  ██║██╔════╝ ██║████╗  ██║██╔════╝
   ██████╔╝██████╔╝██║  ███╗    █████╗  ██╔██╗ ██║██║  ███╗██║██╔██╗ ██║█████╗  
   ██╔══██╗██╔═══╝ ██║   ██║    ██╔══╝  ██║╚██╗██║██║   ██║██║██║╚██╗██║██╔══╝  
   ██║  ██║██║     ╚██████╔╝    ███████╗██║ ╚████║╚██████╔╝██║██║ ╚████║███████╗
   ╚═╝  ╚═╝╚═╝      ╚═════╝     ╚══════╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝╚══════╝
)", BRIGHT_YELLOW);
    std::cout << ConsoleUI::colorize("              === Text-Based Fantasy RPG Engine (C++17) ===\n\n", BRIGHT_CYAN);
}

void printWarriorArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
      [ WARRIOR ]
          /| ________________
    O|===|* >________________>
          \|     [===]
                 (o.o)
                /( : )\
                 d b
)", BRIGHT_BLUE) << "\n";
}

void printMageArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
       [ MAGE ]
          /\
         /  \      (*)
        /____\      |
        ( 'v')     /|\
       / >( )< \  / | \
        /    \      |
       (__/\__)    ===
)", BRIGHT_MAGENTA) << "\n";
}

void printRangerArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
       [ RANGER ]
          .-.       |)
         (o.o)     /| )
         /|_|\====>>|--=>
        (_/ \_)    \| )
                    |)
)", BRIGHT_GREEN) << "\n";
}

void printGoblinArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
      [ GOBLIN SCOUT ]
         (o)__(o)
         (  ..  )
         /      \   <--- (Dagger)
        |  |  |  |
        (__|__|__)
)", YELLOW) << "\n";
}

void printBossDragonArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
            [ ANCIENT DRAGON ]
                  /===-_---~                 ~-_---===\
                 /                 ~-_         /        \
                |                  _  ~-_     /          |
                |               _-~  ~-_ ~-_-~           |
                 \__        _-~         ~-_           __/
                    ~-_  _-~    (o) (o)    ~-_     _-~
                       ~-_         v         ~-_-~
                          ~-_  (_______)  _-~
                             ~--_______--~
)", BRIGHT_RED) << "\n";
}

void printSkeletonArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
      [ SKELETON WARRIOR ]
             .---.
            /     \
           | () () |
            \  -  /
             `---'
            /| | |\   <--- (Rusty Blade)
           / |_|_| \
             |   |
             d   b
)", BRIGHT_BLACK) << "\n";
}

void printOrcArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
       [ ARMORED ORC ]
             .-"-.
           _/.-.-.\_
          (q/  -  \p)
           (   v   )   <--- (Spiked Club)
          /|-=====-|\
         / |       | \
           d       b
)", GREEN) << "\n";
}

void printHeroArt(HeroClass heroClass) {
    switch (heroClass) {
        case HeroClass::WARRIOR:
            printWarriorArt();
            break;
        case HeroClass::MAGE:
            printMageArt();
            break;
        case HeroClass::RANGER:
            printRangerArt();
            break;
        default:
            printWarriorArt();
            break;
    }
}

void printEnemyArt(const Enemy& enemy) {
    if (enemy.getType() == EnemyType::BOSS) {
        printBossDragonArt();
        return;
    }

    std::string name = enemy.getName();
    if (name.find("Skeleton") != std::string::npos || name.find("Xương") != std::string::npos) {
        printSkeletonArt();
    } else if (name.find("Orc") != std::string::npos) {
        printOrcArt();
    } else {
        printGoblinArt();
    }
}

void printBattleVersus(const Hero& hero, const Enemy& enemy) {
    std::cout << ConsoleUI::colorize("  HERO:", ConsoleUI::Colors::BRIGHT_CYAN) 
              << " " << ConsoleUI::colorize(hero.getName(), ConsoleUI::Colors::BRIGHT_WHITE) << "\n";
    printHeroArt(hero.getHeroClass());

    std::cout << ConsoleUI::colorize("                  ⚔   V E R S U S   ⚔\n", ConsoleUI::Colors::BRIGHT_YELLOW);

    std::cout << ConsoleUI::colorize("  KẺ ĐỊCH:", ConsoleUI::Colors::BRIGHT_RED) 
              << " " << ConsoleUI::colorize(enemy.getName(), ConsoleUI::Colors::BRIGHT_WHITE)
              << (enemy.getType() == EnemyType::BOSS ? ConsoleUI::colorize(" [BOSS TỐI CAO]", ConsoleUI::Colors::BRIGHT_RED) : "") << "\n";
    printEnemyArt(enemy);
}

void printBattleBanner() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
 ================================================================
                       ⚔  B A T T L E  ⚔
 ================================================================
)", BRIGHT_RED) << "\n";
}

void printVictoryBanner() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
 ****************************************************************
                      ★  V I C T O R Y  ★
                 You defeated the enemy!
 ****************************************************************
)", BRIGHT_GREEN) << "\n";
}

void printGameOverBanner() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                    ☠  G A M E   O V E R  ☠
                     You have perished...
 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
)", RED) << "\n";
}

} // namespace ASCIIArt
