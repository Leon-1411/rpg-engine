#pragma once

#include "Hero.h"
#include "Enemy.h"

enum class CombatState {
    ONGOING,
    HERO_VICTORY,
    ENEMY_VICTORY,
    FLED
};

class CombatEngine {
private:
    Hero& hero;
    Enemy& enemy;
    int turnCount;
    CombatState currentState;

public:
    CombatEngine(Hero& hero, Enemy& enemy);
    ~CombatEngine() = default;

    void startBattle();
    CombatState executeTurn(int actionChoice, int skillOrItemIndex = -1);
    
    int calculateDamage(int attackerAttack, int defenderDefense) const;
    void processEnemyTurn();
    
    bool isBattleOver() const;
    CombatState getState() const;
    int getTurnCount() const;
};
