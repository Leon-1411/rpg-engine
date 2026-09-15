#include "CombatEngine.h"
#include "Inventory.h"
#include "Item.h"
#include <cassert>
#include <iostream>
#include <sstream>

int main() {
    std::cout << "Running CombatEngine & Status Effect unit tests...\n";

    // =========================================================
    // 1. Edge Case: Sát thương bị âm khi giáp quá cao (Defense >> Attack)
    // =========================================================
    Hero edgeHero("HeroEdge", HeroClass::WARRIOR);
    Enemy tankEnemy("SuperTank", EnemyType::MINION, 100, 10, 999, 10, 5); // DEF = 999
    CombatEngine edgeEngine(edgeHero, tankEnemy);

    // Attack = 5, Defense = 999 -> 5 - 999 = -994 -> MUST clamp to minimum 1 damage
    DamageResult negDamageRes = edgeEngine.calculateDamage(5, 0.0f, 0.0f, 0, 999, false);
    assert(negDamageRes.damage == 1); // Never negative!
    assert(negDamageRes.damage > 0);

    // Fallback overload test: calculateDamage(5, 999) -> Min 1
    int fallbackDmg = edgeEngine.calculateDamage(5, 999);
    assert(fallbackDmg == 1); // Never negative!

    // Verify target taking damage: HP must DECREASE, NEVER increase!
    tankEnemy.takeDamage(negDamageRes.damage);
    assert(tankEnemy.getHp() == 99); // 100 - 1 = 99, never 100 + 994

    // Test: Target defense 10, Armor Pen 10 -> Effective Armor 0 -> 20 - 0 = 20 dmg
    DamageResult res1 = edgeEngine.calculateDamage(20, 0.0f, 0.0f, 10, 10, false);
    assert(res1.damage == 20);

    // Test: Armor Pen > Defense (Pen 30, Def 10) -> Effective Armor 0 -> 20 - 0 = 20 dmg (no negative armor bonus)
    DamageResult resArmorPen = edgeEngine.calculateDamage(20, 0.0f, 0.0f, 30, 10, false);
    assert(resArmorPen.damage == 20);

    // Test: Ignore Armor (Mage) -> Target defense 999 ignored -> 25 dmg
    DamageResult res3 = edgeEngine.calculateDamage(25, 0.0f, 0.0f, 0, 999, true);
    assert(res3.damage == 25);

    // Test: Crit edge cases (0% and 100% crit chance)
    DamageResult noCrit = edgeEngine.calculateDamage(20, 0.0f, 0.50f, 0, 0, true);
    assert(!noCrit.isCrit && noCrit.damage == 20);
    DamageResult fullCrit = edgeEngine.calculateDamage(20, 1.0f, 0.50f, 0, 0, true);
    assert(fullCrit.isCrit && fullCrit.damage == 30); // 20 * (1 + 0.5) = 30

    // =========================================================
    // 2. Edge Case: Máu không bị âm và Hồi máu không vượt quá Max HP
    // =========================================================
    Hero hpClampHero("ClampHero", HeroClass::WARRIOR, 100, 10, 5);
    // Take massive damage greater than current HP
    hpClampHero.takeDamage(500);
    assert(hpClampHero.getHp() == 0); // Clamped at 0, never negative
    assert(!hpClampHero.isAlive());

    // Defensive check: passing negative damage to takeDamage must not heal
    hpClampHero.setHp(50);
    hpClampHero.takeDamage(-50);
    assert(hpClampHero.getHp() == 50); // Unchanged

    // Healing clamp: healing beyond maxHp must clamp at maxHp
    hpClampHero.heal(200);
    assert(hpClampHero.getHp() == hpClampHero.getMaxHp()); // Exactly 100, not 250
    // Negative heal must not hurt
    hpClampHero.heal(-50);
    assert(hpClampHero.getHp() == hpClampHero.getMaxHp());

    // Same clamping for Enemy
    Enemy hpClampEnemy("ClampEnemy", EnemyType::MINION, 50, 10, 5, 10, 5);
    hpClampEnemy.takeDamage(200);
    assert(hpClampEnemy.getHp() == 0);
    assert(!hpClampEnemy.isAlive());
    hpClampEnemy.heal(500);
    assert(hpClampEnemy.getHp() == hpClampEnemy.getMaxHp());

    // =========================================================
    // 3. Status Effect: Mage Poison Flask (Poison DoT trên Enemy)
    // =========================================================
    Hero mageHero("Merlin", HeroClass::MAGE);
    Enemy poisonTarget("Slime", EnemyType::MINION, 30, 0, 0, 20, 10);
    CombatEngine poisonEngine(mageHero, poisonTarget);
    poisonEngine.startBattle();

    // Mage uses Skill 3: Poison Flask (applies 5 turns of 8 dmg/turn)
    poisonEngine.executeTurn(2, 3);
    assert(poisonTarget.isPoisoned());
    assert(poisonTarget.getPoisonTurns() == 5);

    // In next turn, poison DoT should tick 8 damage: 30 - 8 = 22 HP
    // Mage simply guards/defends so only poison deals damage
    poisonEngine.executeTurn(4);
    assert(poisonTarget.getHp() == 22);
    assert(poisonTarget.getPoisonTurns() == 4);

    // =========================================================
    // 4. Status Effect: Mage Healing Potion (Regeneration HoT trên Hero)
    // =========================================================
    Hero regenMage("Gandalf", HeroClass::MAGE, 100, 10, 5);
    Enemy dummyEnemy("Target", EnemyType::MINION, 200, 0, 0, 20, 10);
    regenMage.takeDamage(50); // HP: 50 / 100
    CombatEngine regenEngine(regenMage, dummyEnemy);
    regenEngine.startBattle();

    // Mage casts Skill 2: Healing Potion (+25 instant, +8/turn for 3 turns)
    regenEngine.executeTurn(2, 2);
    assert(regenMage.getHp() == 75); // 50 + 25 = 75
    assert(regenMage.hasRegen());
    assert(regenMage.getRegenTurns() == 3);

    // Next turn: Regen ticks +8 HP -> 75 + 8 = 83 HP
    regenEngine.executeTurn(4); // Hero defends
    assert(regenMage.getHp() == 83);
    assert(regenMage.getRegenTurns() == 2);

    // =========================================================
    // 5. Status Effect: Quái hệ độc (Poisonous Enemy) tiêm độc lên Hero
    // =========================================================
    Hero victimHero("Arthur", HeroClass::WARRIOR, 100, 10, 5, 0, 0.0f, 0.0f);
    Enemy venomSnake("Venom Cobra", EnemyType::MINION, 100, 10, 0, 50, 20);
    venomSnake.setPoisonous(true, 3, 7); // Inflicts poison: 3 turns, 7 dmg/turn
    assert(venomSnake.getIsPoisonous());

    CombatEngine snakeEngine(victimHero, venomSnake);
    snakeEngine.startBattle();

    // Turn 1: Hero attacks, Snake attacks and inflicts poison!
    snakeEngine.executeTurn(1);
    assert(victimHero.isPoisoned());
    assert(victimHero.getPoisonTurns() == 3);

    int hpBeforePoison = victimHero.getHp();
    // Turn 2: Turn starts -> Status effect pipeline ticks 7 poison damage on Hero!
    snakeEngine.executeTurn(1);
    // Hero suffered poison DoT (7 dmg) + Snake normal attack (5 dmg)
    assert(victimHero.getPoisonTurns() == 2);
    assert(victimHero.getHp() < hpBeforePoison);

    // Test Hero dying from poison status effect
    Hero frailHero("FrailHero", HeroClass::RANGER, 10, 10, 0, 0, 0.0f, 0.0f);
    frailHero.applyPoison(2, 20); // 20 poison dmg will kill hero
    Enemy idleEnemy("Watcher", EnemyType::MINION, 100, 0, 0, 0, 0);
    CombatEngine deathByPoisonEngine(frailHero, idleEnemy);
    deathByPoisonEngine.startBattle();
    deathByPoisonEngine.executeTurn(1);
    assert(deathByPoisonEngine.getState() == CombatState::ENEMY_VICTORY);
    assert(!frailHero.isAlive());

    // =========================================================
    // 6. Defend Action & Shield Block & Flee
    // =========================================================
    Hero guardHero("ShieldBearer", HeroClass::WARRIOR, 100, 10, 10, 0, 0.0f, 0.0f);
    Enemy ogre("Ogre", EnemyType::MINION, 100, 20, 0, 50, 20, 0, 0.0f, 0.0f);
    CombatEngine guardEngine(guardHero, ogre);
    guardEngine.startBattle();

    // Defend reduces 10 damage down to 5
    guardEngine.executeTurn(4);
    assert(guardHero.getHp() == 95);

    // Shield Block nullifies 100% damage (0 damage)
    guardEngine.executeTurn(2, 3);
    assert(guardHero.getHp() == 95);

    // Flee test
    Hero fleeHero("Runner", HeroClass::RANGER);
    Enemy dragon("Dragon", EnemyType::BOSS, 500, 50, 20, 100, 50);
    CombatEngine fleeEngine(fleeHero, dragon);
    fleeEngine.startBattle();
    fleeEngine.executeTurn(5);
    assert(fleeEngine.getState() == CombatState::FLED);

    // =========================================================
    // 7. Boss Attack Pattern
    // =========================================================
    Enemy bossMonster("Demon Lord", EnemyType::BOSS, 300, 20, 10, 100, 50);
    assert(bossMonster.chooseAction(HeroClass::WARRIOR, 1) == 1); // Turn 1: Normal
    assert(bossMonster.chooseAction(HeroClass::WARRIOR, 2) == 2); // Turn 2: Heavy
    assert(bossMonster.chooseAction(HeroClass::WARRIOR, 3) == 3); // Turn 3: Special
    assert(bossMonster.chooseAction(HeroClass::WARRIOR, 4) == 1); // Loops back

    // =========================================================
    // 8. Interactive Simulation via std::stringstream
    // =========================================================
    Hero simHero("SimHero", HeroClass::WARRIOR, 100, 50, 10, 0, 0.0f, 0.0f);
    Enemy simEnemy("Goblin", EnemyType::MINION, 30, 5, 0, 20, 10);
    CombatEngine simEngine(simHero, simEnemy);
    std::stringstream inputSim("1\n");
    std::stringstream outputSim;
    simEngine.runInteractiveBattle(inputSim, outputSim);
    assert(simEngine.getState() == CombatState::HERO_VICTORY);

    std::cout << "[PASS] All Edge Cases, Status Effects, and Combat tests passed successfully!\n";
    return 0;
}
