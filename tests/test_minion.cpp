#include "Minion.h"
#include "Hero.h"
#include "CombatEngine.h"
#include <cassert>
#include <iostream>
#include <vector>

void testJsonLoading() {
    std::cout << "Testing Minion JSON loading from data/enemies.json...\n";
    auto minions = MinionFactory::loadAllFromJson("data/enemies.json");
    
    // Check that at least 3 types of minions were loaded
    assert(minions.size() >= 3);
    std::cout << "Successfully loaded " << minions.size() << " minions from JSON.\n";

    bool foundGoblin = false;
    bool foundSkeleton = false;
    bool foundOrc = false;
    bool foundDarkKnight = false;

    for (const auto& m : minions) {
        assert(m != nullptr);
        assert(m->getType() == EnemyType::MINION);
        assert(m->getHp() > 0);
        assert(m->getAttack() > 0);

        if (m->getId() == "goblin") {
            foundGoblin = true;
            assert(m->getMinionType() == MinionType::GOBLIN);
            assert(m->getName() == "Goblin");
            assert(m->getHp() == 60);
            assert(m->getAttack() == 12);
            assert(m->getDefense() == 5);
            assert(m->getSpecialSkillName() == "Quick Strike");
        } else if (m->getId() == "skeleton") {
            foundSkeleton = true;
            assert(m->getMinionType() == MinionType::SKELETON);
            assert(m->getName() == "Skeleton Warrior");
            assert(m->getHp() == 80);
            assert(m->getAttack() == 18);
            assert(m->getDefense() == 8);
            assert(m->getSpecialSkillName() == "Bone Pierce");
        } else if (m->getId() == "orc") {
            foundOrc = true;
            assert(m->getMinionType() == MinionType::ORC);
            assert(m->getName() == "Orc Berserker");
            assert(m->getHp() == 120);
            assert(m->getAttack() == 25);
            assert(m->getDefense() == 12);
            assert(m->getSpecialSkillName() == "Heavy Slam");
        } else if (m->getId() == "dark_knight") {
            foundDarkKnight = true;
            assert(m->getMinionType() == MinionType::DARK_KNIGHT);
            assert(m->getName() == "Dark Knight");
            assert(m->getHp() == 160);
            assert(m->getAttack() == 32);
            assert(m->getDefense() == 18);
            assert(m->getSpecialSkillName() == "Shadow Slash");
        }
    }

    assert(foundGoblin);
    assert(foundSkeleton);
    assert(foundOrc);
    assert(foundDarkKnight);
    std::cout << "[PASS] JSON loading and stat verification passed!\n";
}

void testSingleMinionLoading() {
    std::cout << "Testing loading individual Minion by ID...\n";
    auto goblin = MinionFactory::createFromJson("goblin", "data/enemies.json");
    assert(goblin != nullptr);
    assert(goblin->getName() == "Goblin");
    assert(goblin->getAttack() == 12);

    auto orc = MinionFactory::createFromJson("orc", "data/enemies.json");
    assert(orc != nullptr);
    assert(orc->getName() == "Orc Berserker");
    assert(orc->getDefense() == 12);

    std::cout << "[PASS] Single minion loading passed!\n";
}

void testPolymorphismAndActions() {
    std::cout << "Testing Minion polymorphism and action AI...\n";
    
    // Test Goblin action logic (subclass polymorphism)
    Goblin goblin;
    assert(goblin.getType() == EnemyType::MINION);
    assert(goblin.getMinionType() == MinionType::GOBLIN);
    int gAction1 = goblin.chooseAction(); // turn 1 -> 1 (regular)
    int gAction2 = goblin.chooseAction(); // turn 2 -> 2 (quick strike)
    assert(gAction1 == 1);
    assert(gAction2 == 2);

    // Test Skeleton action logic
    Skeleton skeleton;
    assert(skeleton.chooseAction() == 1); // turn 1
    assert(skeleton.chooseAction() == 1); // turn 2
    assert(skeleton.chooseAction() == 2); // turn 3 -> Bone Pierce

    // Test Orc berserk action logic
    Orc orc;
    assert(orc.chooseAction() == 1); // turn 1
    assert(orc.chooseAction() == 1); // turn 2
    assert(orc.chooseAction() == 1); // turn 3
    assert(orc.chooseAction() == 2); // turn 4 (normal mode uses skill every 4 turns)

    // Lower Orc HP to <= 50% (Max HP: 120, Def: 12. 75 dmg - 12 def = 63 effective dmg -> HP 57 <= 60)
    orc.takeDamage(75);
    assert(orc.getHp() <= orc.getMaxHp() / 2);
    // Enraged mode uses skill on even turns
    int orcEnraged1 = orc.chooseAction(); // turn 5 -> odd -> 1
    int orcEnraged2 = orc.chooseAction(); // turn 6 -> even -> 2 (Heavy Slam!)
    assert(orcEnraged1 == 1);
    assert(orcEnraged2 == 2);

    // Test pointer polymorphism: Enemy* pointing to Minion
    Enemy* enemyPtr = &goblin;
    assert(enemyPtr->getName() == "Goblin");
    assert(enemyPtr->isAlive() == true);
    enemyPtr->takeDamage(100);
    assert(enemyPtr->isAlive() == false);

    std::cout << "[PASS] Polymorphism and action AI tests passed!\n";
}

void testMinionCombat() {
    std::cout << "Testing Minion in CombatEngine...\n";
    Hero hero("Warrior Hero", HeroClass::WARRIOR, 150, 40, 25, 10);
    auto skeleton = MinionFactory::createFromJson("skeleton", "data/enemies.json");
    assert(skeleton != nullptr);

    CombatEngine engine(hero, *skeleton);
    engine.startBattle();
    assert(engine.getState() == CombatState::ONGOING);

    // Hero attacks Skeleton (25 - 8 = 17 dmg -> Skeleton HP 63)
    engine.executeTurn(1);
    assert(skeleton->getHp() == 63);

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
    std::cout << "       RUNNING MINION SYSTEM TESTS      \n";
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
