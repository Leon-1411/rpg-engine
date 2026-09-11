#pragma once

#include "Hero.h"
#include "Enemy.h"
#include <string>

enum class BattleAction {
    ATTACK = 1,
    SKILL = 2,
    ITEM = 3,
    DEFEND = 4,
    RUN = 5
};

class BattleUI {
public:
    BattleUI() = default;
    ~BattleUI() = default;

    // Renders the battle HUD with Hero stats/bars, Enemy stats/bars, and action log
    void renderBattleScreen(const Hero& hero, const Enemy& enemy, const std::string& battleMessage = "");

    // Prompts player for battle action
    BattleAction getPlayerAction();

    // Prints a combat turn log event
    void printCombatLog(const std::string& message);

    // Displays victory banner with EXP/Gold rewards
    void showVictory(const Enemy& enemy);

    // Displays defeat / game over screen
    void showDefeat();
};
