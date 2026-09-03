#include "CombatEngine.h"
#include <cassert>
#include <iostream>

int main() {
    Hero hero("Knight", HeroClass::WARRIOR, 100, 20, 25, 5);
    Enemy enemy("Slime", EnemyType::MINION, 30, 8, 2, 10, 5);

    CombatEngine engine(hero, enemy);
    engine.startBattle();
    
    assert(engine.getState() == CombatState::ONGOING);

    // Hero attacks slime (25 - 2 = 23 dmg -> Slime HP 7)
    engine.executeTurn(1);
    assert(enemy.getHp() == 7);

    // Hero attacks again (Slime dies)
    engine.executeTurn(1);
    assert(enemy.getHp() == 0);
    assert(engine.getState() == CombatState::HERO_VICTORY);

    std::cout << "[PASS] CombatEngine unit tests successful!\n";
    return 0;
}
