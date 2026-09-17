#pragma once

#include "Hero.h"
#include "Enemy.h"
#include "Inventory.h"
#include "DamageCalculator.h"
#include <string>
#include <vector>

enum class CombatState {
    ONGOING,
    HERO_VICTORY,
    ENEMY_VICTORY,
    FLED
};

struct CombatTurnResult {
    CombatState state;
    int heroDamageDealt;
    int enemyDamageDealt;
    std::vector<std::string> messages;
};

class CombatEngine {
private:
    Hero& hero;
    Enemy& enemy;
    Inventory* inventory;
    int turnCount;
    CombatState currentState;
    bool isHeroDefending;
    bool isEnemyDefending;
    std::vector<std::string> turnLogs;

public:
    CombatEngine(Hero& hero, Enemy& enemy, Inventory* inv = nullptr);
    ~CombatEngine() = default;

    void startBattle();
    CombatState executeTurn(int actionChoice, int skillOrItemIndex = 1);
    
    int calculateDamage(int attackerAttack, int defenderDefense) const;
    void processEnemyTurn();
    
    bool isBattleOver() const;
    CombatState getState() const;
    int getTurnCount() const;

    const std::vector<std::string>& getTurnLogs() const;
    void clearLogs();
};
