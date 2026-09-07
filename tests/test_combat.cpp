#include "CombatEngine.h"
#include <cassert>
#include <iostream>

int main() {
    // 1. Damage formula calculation unit tests
    Hero testHero("Tester", HeroClass::WARRIOR);
    Enemy testEnemy("Dummy", EnemyType::MINION, 100, 10, 10, 10, 5);
    CombatEngine testEngine(testHero, testEnemy);

    // Test: Target defense 10, Armor Pen 10 -> Effective Armor 0 -> 20 - 0 = 20 dmg
    DamageResult res1 = testEngine.calculateDamage(20, 0.0f, 0.0f, 10, 10, false);
    assert(res1.damage == 20);

    // Test: Target defense 25, Armor Pen 5 -> Effective Armor 20 -> 15 - 20 = -5 -> Min 1 dmg
    DamageResult res2 = testEngine.calculateDamage(15, 0.0f, 0.0f, 5, 25, false);
    assert(res2.damage == 1);

    // Test: Ignore Armor (Mage) -> Target defense 50 ignored -> 25 dmg
    DamageResult res3 = testEngine.calculateDamage(25, 0.0f, 0.0f, 0, 50, true);
    assert(res3.damage == 25);

    // 2. Battle turn simulation
    // Hero with 25 ATK, 0 Pen, 0 Crit against Slime (30 HP, 2 DEF)
    Hero hero("Knight", HeroClass::WARRIOR, 100, 25, 5, 0, 0.0f, 0.0f);
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
