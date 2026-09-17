#include "Hero.h"
#include <cassert>
#include <iostream>

int main() {
    // 1. Test Warrior
    Hero warrior("Arthur", HeroClass::WARRIOR);
    assert(warrior.getName() == "Arthur");
    assert(warrior.getHp() == 150);
    assert(warrior.getAttack() == 7);
    assert(warrior.getDefense() == 25);
    assert(warrior.getArmorPenetration() == 10);

    // Direct damage (no double defense subtraction)
    warrior.takeDamage(15);
    assert(warrior.getHp() == 135);
    warrior.heal(15);
    assert(warrior.getHp() == 150);

    // Test Warrior Skill 1 (Sword Slash)
    int dmg = 0;
    std::string msg;
    assert(warrior.isSkillReady(1) == true);
    assert(warrior.useSkill(1, dmg, msg) == true);
    assert(dmg > 0);
    assert(warrior.isSkillReady(1) == false); // Now on cooldown
    warrior.reduceCooldowns();
    assert(warrior.isSkillReady(1) == true); // Cooldown reduced

    // 2. Test Ranger & Ready Arrows
    Hero ranger("Robin", HeroClass::RANGER);
    assert(ranger.getHp() == 100);
    assert(ranger.getAttack() == 15);
    assert(ranger.getDefense() == 10);
    assert(ranger.getReadyArrows() == 2);

    // Test Ranger Skill 2 (Aimed Shot, consumes 1 arrow)
    assert(ranger.useSkill(2, dmg, msg) == true);
    assert(ranger.getReadyArrows() == 1);

    // 3. Test Mage & Ignore Armor
    Hero mage("Merlin", HeroClass::MAGE);
    assert(mage.getHp() == 100);
    assert(mage.getAttack() == 25);
    assert(mage.isIgnoreArmor() == true);
    assert(mage.getCritChance() == 0.0f);

    std::cout << "[PASS] Hero unit tests successful!\n";
    return 0;
}
