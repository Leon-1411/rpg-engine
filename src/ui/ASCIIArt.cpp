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
