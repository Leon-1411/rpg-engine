#pragma once

#include "Hero.h"
#include "Enemy.h"

enum class CombatState {
    ONGOING,
    HERO_VICTORY,
    ENEMY_VICTORY,
    FLED
};

struct DamageResult {
    int damage;
    bool isCrit;
};

class CombatEngine {
private:
    Hero& hero;
    Enemy& enemy;
    int turnCount;
    CombatState currentState;

    void processPoisonTick();

public:
    CombatEngine(Hero& hero, Enemy& enemy);
    ~CombatEngine() = default;

    void startBattle();
    CombatState executeTurn(int actionChoice, int skillOrItemIndex = -1);
    
    // Comprehensive damage calculation formula
    DamageResult calculateDamage(int attackerAttack, float critChance, float critDmg,
                                 int armorPen, int defenderDefense, bool ignoreArmor = false) const;
    
    // Simple fallback overload
    int calculateDamage(int attackerAttack, int defenderDefense) const;

    void processEnemyTurn();
    
    bool isBattleOver() const;
    CombatState getState() const;
    int getTurnCount() const;
};
