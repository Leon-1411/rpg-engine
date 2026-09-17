#include "BossMonster.h"
#include "Minion.h"
#include "Hero.h"
#include "CombatEngine.h"
#include <cassert>
#include <iostream>
#include <memory>

void testBossStats5xMinion() {
    std::cout << "Testing Boss stats 5x scaling from Minions...\n";

    // 1. Test dynamic scaling from a Goblin
    Goblin goblin;
    auto goblinBoss = BossMonster::createScaledFromMinion(goblin, "goblin_king", "Goblin King");
    assert(goblinBoss != nullptr);
    assert(goblinBoss->getType() == EnemyType::BOSS);
    assert(goblinBoss->getMaxHp() == goblin.getMaxHp() * 5); // 60 * 5 = 300
    assert(goblinBoss->getBaseAttack() == goblin.getAttack() * 5); // 12 * 5 = 60
    assert(goblinBoss->getBaseDefense() == goblin.getDefense() * 5); // 5 * 5 = 25
    assert(goblinBoss->getExpReward() == goblin.getExpReward() * 5); // 40 * 5 = 200
    assert(goblinBoss->getGoldReward() == goblin.getGoldReward() * 5); // 10 * 5 = 50

    // 2. Test dynamic scaling from an Orc Berserker
    Orc orc;
    auto orcWarchief = BossMonster::createScaledFromMinion(orc, "orc_warchief", "Orc Warchief");
    assert(orcWarchief->getMaxHp() == orc.getMaxHp() * 5); // 120 * 5 = 600
    assert(orcWarchief->getBaseAttack() == orc.getAttack() * 5); // 25 * 5 = 125
    assert(orcWarchief->getBaseDefense() == orc.getDefense() * 5); // 12 * 5 = 60

    // 3. Test default BossMonster stats (configured as 5x average regular minion)
    BossMonster defaultBoss;
    assert(defaultBoss.getType() == EnemyType::BOSS);
    assert(defaultBoss.getMaxHp() == 525); // ~ 105 average minion HP * 5
    assert(defaultBoss.getBaseAttack() == 105); // ~ 21 average minion ATK * 5
    assert(defaultBoss.getBaseDefense() == 50); // ~ 10 average minion DEF * 5
    assert(defaultBoss.getExpReward() == 450); // ~ 90 average minion EXP * 5
    assert(defaultBoss.getGoldReward() == 150); // ~ 30 average minion Gold * 5

    std::cout << "[PASS] Boss 5x scaling test passed!\n";
}

void testEnrageMechanic() {
    std::cout << "Testing Boss Enrage mechanic (HP < 30% -> stats +50%)...\n";

    // Max HP: 500, ATK: 100, DEF: 50
    // 30% HP threshold = 150 HP
    BossMonster boss("test_boss", "Test Boss", 500, 100, 50, 400, 100, "Test Description", "Flame Strike", 20);

    assert(!boss.isEnraged());
    assert(boss.getAttack() == 100);
    assert(boss.getDefense() == 50);

    // Inflict damage to bring HP to 200 (200 / 500 = 40% > 30% -> not enraged)
    // 500 - (350 - 50) = 200 HP
    boss.takeDamage(350);
    assert(boss.getHp() == 200);
    assert(!boss.isEnraged());
    assert(boss.getAttack() == 100);
    assert(boss.getDefense() == 50);

    // Inflict further damage to bring HP to 140 (140 / 500 = 28% < 30% -> enrage triggered!)
    // 200 - (110 - 50) = 140 HP
    boss.takeDamage(110);
    assert(boss.getHp() == 140);
    assert(boss.isEnraged());

    // Verify stats increased by 50%
    // Attack: 100 * 1.5 = 150
    // Defense: 50 * 1.5 = 75
    assert(boss.getAttack() == 150);
    assert(boss.getDefense() == 75);

    // Taking further damage while already enraged should NOT multiply stats again
    boss.takeDamage(85); // 140 - (85 - 75) = 130 HP
    assert(boss.getHp() == 130);
    assert(boss.isEnraged());
    assert(boss.getAttack() == 150);
    assert(boss.getDefense() == 75);

    std::cout << "[PASS] Enrage mechanic test passed!\n";
}

void testHealingAbility() {
    std::cout << "Testing Boss slight healing ability (Enraged regeneration)...\n";

    BossMonster boss("test_boss", "Test Boss", 500, 100, 50, 400, 100, "Test", "Skill", 25);

    // Direct heal test
    boss.setHp(100);
    boss.heal(25);
    assert(boss.getHp() == 125);

    // Heal should not exceed maxHp
    boss.setHp(490);
    boss.heal(50);
    assert(boss.getHp() == 500);

    // Regenerate test when Enraged during turn action
    boss.setHp(120); // < 30%
    boss.checkEnrage();
    assert(boss.isEnraged());

    int hpBeforeTurn = boss.getHp();
    boss.chooseAction(); // In enraged state, chooseAction() automatically calls regenerate()
    assert(boss.getHp() == hpBeforeTurn + boss.getHealAmount()); // 120 + 25 = 145

    std::cout << "[PASS] Healing ability test passed!\n";
}

void testCombatWithBoss() {
    std::cout << "Testing Boss in CombatEngine...\n";

    Hero hero("Legendary Knight", HeroClass::WARRIOR, 500, 120, 40, 20);
    BossMonster boss("boss_overlord", "Demon Overlord", 300, 60, 20, 300, 100, "Fierce Demon", "Dark Nova", 15);

    CombatEngine combat(hero, boss);
    combat.startBattle();
    assert(combat.getState() == CombatState::ONGOING);

    // Hero attacks: hero ATK 40, boss DEF 20 -> damage = 20
    // Boss HP: 300 - 20 = 280 (not enraged yet)
    combat.executeTurn(1);
    assert(!boss.isEnraged());

    // Reduce boss HP directly to test in-combat transition
    // 30% of 300 = 90. Set HP to 80 to trigger enrage
    boss.setHp(80);
    assert(!boss.isEnraged()); // checkEnrage not called yet

    // Hero attacks again -> boss takes damage -> checkEnrage triggers enrage!
    combat.executeTurn(1);
    assert(boss.isEnraged());
    assert(boss.getAttack() == 90); // 60 * 1.5 = 90
    assert(boss.getDefense() == 30); // 20 * 1.5 = 30

    // During enemy turn, boss regenerated 15 HP
    std::cout << "[PASS] Boss CombatEngine integration test passed!\n";
}

void testBossJsonLoading() {
    std::cout << "Testing Boss JSON loading from data/enemies.json...\n";

    auto boss = BossFactory::createFromJson("dragon_lord", "data/enemies.json");
    assert(boss != nullptr);
    assert(boss->getType() == EnemyType::BOSS);
    assert(boss->getName() == "Dragon Lord");
    assert(boss->getMaxHp() == 525);
    assert(boss->getAttack() == 105);
    assert(boss->getDefense() == 50);
    assert(boss->getExpReward() == 450);
    assert(boss->getGoldReward() == 150);
    assert(boss->getSpecialSkillName() == "Infernal Cataclysm");
    assert(boss->getHealAmount() == 25);

    auto allBosses = BossFactory::loadAllFromJson("data/enemies.json");
    assert(!allBosses.empty());
    bool foundDragonLord = false;
    for (const auto& b : allBosses) {
        if (b->getId() == "dragon_lord") {
            foundDragonLord = true;
            assert(b->getType() == EnemyType::BOSS);
        }
    }
    assert(foundDragonLord);

    // Serialization test
    auto jsonOutput = BossFactory::bossToJson(*boss);
    assert(jsonOutput["id"] == "dragon_lord");
    assert(jsonOutput["type"] == "BOSS");
    assert(jsonOutput["hp"] == 525);
    assert(jsonOutput["attack"] == 105);

    std::cout << "[PASS] Boss JSON loading test passed!\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "        RUNNING BOSS SYSTEM TESTS       \n";
    std::cout << "========================================\n";

    testBossStats5xMinion();
    testEnrageMechanic();
    testHealingAbility();
    testCombatWithBoss();
    testBossJsonLoading();

    std::cout << "\n========================================\n";
    std::cout << "   [ALL PASS] All Boss tests passed!    \n";
    std::cout << "========================================\n";
    return 0;
}
