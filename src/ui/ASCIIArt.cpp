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

void printWildMercenaryArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
      [ WILD MERCENARY ]
           (\_/)
          ( . .)       <--- (Dual Daggers)
         c(") (")>==--
          / | \
         (_/ \_)
)", BRIGHT_YELLOW) << "\n";
}

void printDemonScoutArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
       [ DEMON SCOUT ]
          ^___^
         ( 'o' )  )~~   <--- (Shadow Darts)
         /(   )\//
          d   b
)", BRIGHT_MAGENTA) << "\n";
}

void printDemonBerserkerArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
      [ DEMON BERSERKER ]
          \m/     \m/
          (  >皿<  )   <--- (Blood Great-Axe)
         / |======| \====[]
        /  |      |  \
          (________)
)", BRIGHT_RED) << "\n";
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

void printDemonMalakorArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
        ( \.-./ )
         / _ _ \
        ( (o).(o) )
         )  _  (       [ DEMON KING MALAKOR ]
        /       \      Hellfire Sovereign & Shadow Master
       / /     \ \
      ( (       ) )
       \ \_._._/ /
)", BRIGHT_RED) << "\n";
}

void printGeneralAldricArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
          [===]
         ( \./ )
         /|===|\
        /_|===|_\      [ GENERAL ALDRIC ]
       (  |===|  )     The Iron Shield of Eldoria
       /  |===|  \
         (_____)
)", BRIGHT_YELLOW) << "\n";
}

void printCoreGuardianArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
         .-------.
        /  (o o)  \
       |  [ CORE ] |   [ THE CORE GUARDIAN ]
        \  \===/  /    Ancient Arcane Automaton
       .-'-------'-.
      /  |       |  \
     (___|_______|___)
)", BRIGHT_CYAN) << "\n";
}

void printArchmageMorvathArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
           /\
          /  \
         /____\
        (  o.o )       [ ARCHMAGE MORVATH ]
        /| === |\      Supreme Tyrant of the Mage Council
       / | ___ | \
         (_____)
)", BRIGHT_MAGENTA) << "\n";
}

void printMultiFactionArt() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
         \  |  /
        --- * ---      [ MULTI-FACTION BATTLE ]
         /  |  \       Kingdom vs Demon vs Mages vs Free People
        / \   / \
       /___\ /___\
)", BRIGHT_WHITE) << "\n";
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
    std::string name = enemy.getName();
    if (name.find("Malakor") != std::string::npos || name.find("Demon_King") != std::string::npos) {
        printDemonMalakorArt();
        return;
    }
    if (name.find("Demon Berserker") != std::string::npos || name.find("Demon_Berserker") != std::string::npos) {
        printDemonBerserkerArt();
        return;
    }
    if (name.find("Demon Scout") != std::string::npos || name.find("Demon_Scout") != std::string::npos) {
        printDemonScoutArt();
        return;
    }
    if (name.find("Wild Mercenary") != std::string::npos || name.find("Wild_Mercenary") != std::string::npos || name.find("Mercenary") != std::string::npos) {
        printWildMercenaryArt();
        return;
    }
    if (name.find("Aldric") != std::string::npos) {
        printGeneralAldricArt();
        return;
    }
    if (name.find("Core Guardian") != std::string::npos || name.find("Core_Guardian") != std::string::npos) {
        printCoreGuardianArt();
        return;
    }
    if (name.find("Morvath") != std::string::npos || name.find("Arcane") != std::string::npos) {
        printArchmageMorvathArt();
        return;
    }
    if (name.find("Multi") != std::string::npos || name.find("Faction") != std::string::npos) {
        printMultiFactionArt();
        return;
    }

    if (enemy.getType() == EnemyType::BOSS) {
        printBossDragonArt();
        return;
    }

    if (name.find("Skeleton") != std::string::npos || name.find("Xuong") != std::string::npos) {
        printSkeletonArt();
    } else if (name.find("Orc") != std::string::npos) {
        printOrcArt();
    } else if (name.find("Demon") != std::string::npos) {
        printDemonScoutArt();
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
)", BRIGHT_YELLOW) << "\n";
}

void printGameOverBanner() {
    using namespace ConsoleUI::Colors;
    std::cout << ConsoleUI::colorize(
R"(
 ################################################################
                      ☠  G A M E   O V E R  ☠
                         You were defeated...
 ################################################################
)", BRIGHT_RED) << "\n";
}

} // namespace ASCIIArt
