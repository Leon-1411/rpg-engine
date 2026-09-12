#pragma once

#include "Hero.h"
#include "Enemy.h"
#include "Inventory.h"
#include <iostream>
#include <string>

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
    Inventory* inventory;
    int turnCount;
    CombatState currentState;

    void processHeroStatusEffects();
    void processEnemyStatusEffects();
    void processStatusEffects();

public:
    CombatEngine(Hero& hero, Enemy& enemy, Inventory* inventory = nullptr);
    ~CombatEngine() = default;

    void setInventory(Inventory* inv);

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

    // UI and Interactive Console Battle Helpers
    static std::string renderBar(int current, int max, int length = 15);
    void displayBattleStatus(std::ostream& out = std::cout) const;
    void runInteractiveBattle(std::istream& in = std::cin, std::ostream& out = std::cout);
};
