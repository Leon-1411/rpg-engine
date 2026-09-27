#include "Minion.h"
#include "Hero.h"
#include "CombatEngine.h"
#include <cassert>
#include <iostream>
#include <vector>

void testJsonLoading() {
    std::cout << "Testing 3 Canon Minions JSON loading from data/enemies.json...\n";
    auto minions = MinionFactory::loadAllFromJson("data/enemies.json");
    
    // Check that at least 3 types of minions were loaded
    assert(minions.size() >= 3);
    std::cout << "Successfully loaded " << minions.size() << " minions from JSON.\n";

    bool foundWildMercenary = false;
    bool foundDemonScout = false;
    bool foundDemonBerserker = false;

    for (const auto& m : minions) {
        assert(m != nullptr);
        assert(m->getType() == EnemyType::MINION);
        assert(m->getHp() > 0);
        assert(m->getAttack() > 0);

        if (m->getId() == "Wild_Mercenary") {
            foundWildMercenary = true;
            assert(m->getMinionType() == MinionType::WILD_MERCENARY);
            assert(m->getName() == "Wild Mercenary");
            assert(m->getHp() == 90);
            assert(m->getAttack() == 20);
            assert(m->getDefense() == 8);
            assert(m->getExpReward() == 75);
            assert(m->getGoldReward() == 25);
            assert(m->getSpecialSkillName() == "Wild Ambush");
        } else if (m->getId() == "Demon_Scout") {
            foundDemonScout = true;
            assert(m->getMinionType() == MinionType::DEMON_SCOUT);
            assert(m->getName() == "Demon Scout");
            assert(m->getHp() == 85);
            assert(m->getAttack() == 22);
            assert(m->getDefense() == 6);
            assert(m->getExpReward() == 80);
            assert(m->getGoldReward() == 30);
            assert(m->getSpecialSkillName() == "Shadow Dart");
        } else if (m->getId() == "Demon_Berserker") {
            foundDemonBerserker = true;
            assert(m->getMinionType() == MinionType::DEMON_BERSERKER);
            assert(m->getName() == "Demon Berserker");
            assert(m->getHp() == 140);
            assert(m->getAttack() == 30);
            assert(m->getDefense() == 14);
            assert(m->getExpReward() == 130);
            assert(m->getGoldReward() == 50);
            assert(m->getSpecialSkillName() == "Demonic Frenzy");
        }
    }

    assert(foundWildMercenary);
    assert(foundDemonScout);
    assert(foundDemonBerserker);
    std::cout << "[PASS] 3 Canon Minions JSON loading and stat verification passed!\n";
}

void testSingleMinionLoading() {
    std::cout << "Testing loading individual 3 Canon Minions by ID...\n";
    auto merc = MinionFactory::createFromJson("Wild_Mercenary", "data/enemies.json");
    assert(merc != nullptr);
    assert(merc->getName() == "Wild Mercenary");
    assert(merc->getAttack() == 20);
    assert(merc->getMinionType() == MinionType::WILD_MERCENARY);

    auto scout = MinionFactory::createFromJson("Demon_Scout", "data/enemies.json");
    assert(scout != nullptr);
    assert(scout->getName() == "Demon Scout");
    assert(scout->getDefense() == 6);
    assert(scout->getMinionType() == MinionType::DEMON_SCOUT);

    auto berserker = MinionFactory::createFromJson("Demon_Berserker", "data/enemies.json");
    assert(berserker != nullptr);
    assert(berserker->getName() == "Demon Berserker");
    assert(berserker->getHp() == 140);
    assert(berserker->getMinionType() == MinionType::DEMON_BERSERKER);

    std::cout << "[PASS] Single minion loading passed!\n";
}

void testPolymorphismAndActions() {
    std::cout << "Testing 3 Canon Minions polymorphism and action AI...\n";
    
    // 1. Test WildMercenary action logic (ambush on even turns)
    WildMercenary merc;
    assert(merc.getType() == EnemyType::MINION);
    assert(merc.getMinionType() == MinionType::WILD_MERCENARY);
    int mAction1 = merc.chooseAction(); // turn 1 -> 1
    int mAction2 = merc.chooseAction(); // turn 2 -> 2 (Wild Ambush)
    assert(mAction1 == 1);
    assert(mAction2 == 2);

    // 2. Test DemonScout action logic (skill every 3rd turn)
    DemonScout scout;
    assert(scout.chooseAction() == 1); // turn 1
    assert(scout.chooseAction() == 1); // turn 2
    assert(scout.chooseAction() == 2); // turn 3 -> Shadow Dart

    // 3. Test DemonBerserker action logic
    DemonBerserker berserker;
    assert(berserker.chooseAction() == 1); // turn 1
    assert(berserker.chooseAction() == 1); // turn 2
    assert(berserker.chooseAction() == 1); // turn 3
    assert(berserker.chooseAction() == 2); // turn 4 (normal mode uses skill every 4 turns)

    // Lower DemonBerserker HP to <= 50% (Max HP: 140, Def: 14. 85 dmg - 14 def = 71 effective dmg -> HP 69 <= 70)
    berserker.takeDamage(85);
    assert(berserker.getHp() <= berserker.getMaxHp() / 2);
    // Enraged mode uses skill on even turns
    int bEnraged1 = berserker.chooseAction(); // turn 5 -> odd -> 1
    int bEnraged2 = berserker.chooseAction(); // turn 6 -> even -> 2 (Demonic Frenzy!)
    assert(bEnraged1 == 1);
    assert(bEnraged2 == 2);

    // Test pointer polymorphism: Enemy* pointing to WildMercenary
    Enemy* enemyPtr = &merc;
    assert(enemyPtr->getName() == "Wild Mercenary");
    assert(enemyPtr->isAlive() == true);
    enemyPtr->takeDamage(150);
    assert(enemyPtr->isAlive() == false);

    std::cout << "[PASS] Polymorphism and action AI tests passed!\n";
}

void testMinionCombat() {
    std::cout << "Testing Minion in CombatEngine...\n";
    Hero hero("Warrior Arthur", HeroClass::WARRIOR, 150, 40, 28, 10, 0, 0.0f, 0.0f);
    auto merc = MinionFactory::createFromJson("Wild_Mercenary", "data/enemies.json");
    assert(merc != nullptr);

    CombatEngine engine(hero, *merc);
    engine.startBattle();
    assert(engine.getState() == CombatState::ONGOING);

    // Hero attacks Wild Mercenary (28 - 8 = 20 dmg -> Wild Mercenary HP 70/90)
    engine.executeTurn(1);
    assert(merc->getHp() == 70);

    std::cout << "[PASS] Minion combat integration passed!\n";
}

void testSaveConfig() {
    std::cout << "Testing MinionFactory::saveMinionConfig...\n";
    auto minions = MinionFactory::loadAllFromJson("data/enemies.json");
    std::string testPath = "data/enemies_test_save.json";
    bool saved = MinionFactory::saveMinionConfig(testPath, minions);
    assert(saved == true);

    auto reloaded = MinionFactory::loadAllFromJson(testPath);
    assert(reloaded.size() == minions.size());
    std::cout << "[PASS] Minion config saving passed!\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   RUNNING 3 CANON MINION SYSTEM TESTS  \n";
    std::cout << "========================================\n";

    testJsonLoading();
    testSingleMinionLoading();
    testPolymorphismAndActions();
    testMinionCombat();
    testSaveConfig();

    std::cout << "\n========================================\n";
    std::cout << "  [ALL PASS] All Minion tests successful!\n";
    std::cout << "========================================\n";
    return 0;
}
