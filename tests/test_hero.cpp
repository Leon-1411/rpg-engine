#include "Hero.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "Item.h"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "=== Running Comprehensive Hero Unit Tests ===\n";

    // -------------------------------------------------------------
    // 1. Test Warrior (Arthur)
    // -------------------------------------------------------------
    std::shared_ptr<Hero> warrior = Hero::createHero(HeroClass::WARRIOR, "Arthur");
    assert(warrior->getName() == "Arthur");
    assert(warrior->getHeroClass() == HeroClass::WARRIOR);
    assert(warrior->getHeroClassName() == "Warrior");
    assert(warrior->getHp() == 120 && warrior->getMaxHp() == 120);
    assert(warrior->getMp() == 30 && warrior->getMaxMp() == 30);
    assert(warrior->getAttack() == 18);
    assert(warrior->getDefense() == 8);
    assert(warrior->getArmorPenetration() == 10);
    assert(warrior->isAlive() == true);

    // Damage and Healing
    warrior->takeDirectDamage(20);
    assert(warrior->getHp() == 100);
    warrior->heal(15);
    assert(warrior->getHp() == 115);
    warrior->heal(50); // Clamped to maxHp
    assert(warrior->getHp() == 120);

    // Test Warrior Skills
    int dmg = 0;
    std::string msg;

    // Skill 1: Power Slash (10 MP, 200% ATK = 36 dmg)
    assert(warrior->useSkill(1, dmg, msg) == true);
    assert(dmg == 36);
    assert(warrior->getMp() == 20); // 30 - 10

    // Skill 2: Shield Block (12 MP, ATK + DEF*2 = 18 + 16 = 34 dmg)
    assert(warrior->useSkill(2, dmg, msg) == true);
    assert(dmg == 34);
    assert(warrior->getIsBlocking() == true);
    assert(warrior->getMp() == 8); // 20 - 12

    // Skill 3: Berserk (Requires 15 MP, currently 8 -> fail)
    assert(warrior->useSkill(3, dmg, msg) == false);
    assert(dmg == 0);

    // Restore MP and use Berserk (15 MP, 300% ATK = 54 dmg)
    warrior->restoreMp(20);
    assert(warrior->getMp() == 28);
    assert(warrior->useSkill(3, dmg, msg) == true);
    assert(dmg == 54);
    assert(warrior->getMp() == 13); // 28 - 15

    std::cout << "  [PASS] Warrior base stats and skills verified.\n";

    // -------------------------------------------------------------
    // 2. Test Mage (Morrigan)
    // -------------------------------------------------------------
    std::shared_ptr<Hero> mage = Hero::createHero(HeroClass::MAGE, "Morrigan");
    assert(mage->getName() == "Morrigan");
    assert(mage->getHeroClass() == HeroClass::MAGE);
    assert(mage->getHeroClassName() == "Mage");
    assert(mage->getHp() == 80 && mage->getMaxHp() == 80);
    assert(mage->getMp() == 100 && mage->getMaxMp() == 100);
    assert(mage->getAttack() == 24);
    assert(mage->getDefense() == 3);
    assert(mage->isIgnoreArmor() == true);

    // Skill 1: Fireball (15 MP, ATK*2 + 10 = 58 dmg)
    assert(mage->useSkill(1, dmg, msg) == true);
    assert(dmg == 58);
    assert(mage->getMp() == 85);

    // Skill 2: Ice Blast (20 MP, ATK*2 + 20 = 68 dmg, +10 MP restore)
    assert(mage->useSkill(2, dmg, msg) == true);
    assert(dmg == 68);
    assert(mage->getMp() == 75); // 85 - 20 + 10 = 75

    // Skill 3: Meteor (35 MP, ATK*3 + 40 = 112 dmg)
    assert(mage->useSkill(3, dmg, msg) == true);
    assert(dmg == 112);
    assert(mage->getMp() == 40); // 75 - 35 = 40

    std::cout << "  [PASS] Mage base stats and magic skills verified.\n";

    // -------------------------------------------------------------
    // 3. Test Ranger (Lyra)
    // -------------------------------------------------------------
    std::shared_ptr<Hero> ranger = Hero::createHero(HeroClass::RANGER, "Lyra");
    assert(ranger->getName() == "Lyra");
    assert(ranger->getHeroClass() == HeroClass::RANGER);
    assert(ranger->getHeroClassName() == "Ranger");
    assert(ranger->getHp() == 95 && ranger->getMaxHp() == 95);
    assert(ranger->getMp() == 50 && ranger->getMaxMp() == 50);
    assert(ranger->getAttack() == 20);
    assert(ranger->getDefense() == 5);
    assert(ranger->getCritChance() == 0.25f);
    assert(ranger->getCritDamage() == 0.50f);

    // Skill 1: Double Shot (12 MP, 180% ATK = 36 dmg)
    assert(ranger->useSkill(1, dmg, msg) == true);
    assert(dmg == 36);
    assert(ranger->getMp() == 38);

    // Skill 2: Poison Arrow (18 MP, ATK*2 + 10 = 50 dmg)
    assert(ranger->useSkill(2, dmg, msg) == true);
    assert(dmg == 50);
    assert(ranger->getMp() == 20);

    // Skill 3: Rain of Arrows (25 MP, currently 20 -> fail)
    assert(ranger->useSkill(3, dmg, msg) == false);
    ranger->restoreMp(20);
    assert(ranger->useSkill(3, dmg, msg) == true);
    assert(dmg == 60); // 20 * 3

    std::cout << "  [PASS] Ranger base stats and archery skills verified.\n";

    // -------------------------------------------------------------
    // 4. Test Equipment & Effective Stats + Weapon Skill Scaling
    // -------------------------------------------------------------
    Item ironSword("wpn_01", "Iron Sword", "Sharp blade", ItemType::WEAPON, 12);
    Item ironArmor("arm_01", "Iron Armor", "Sturdy armor", ItemType::ARMOR, 8);

    warrior->getInventory().addItem(ironSword);
    warrior->getInventory().addItem(ironArmor);
    warrior->getInventory().equipWeapon(0); // Equip weapon (+12 ATK)
    warrior->getInventory().equipArmor(1); // Equip armor (+8 DEF)

    assert(warrior->getEffectiveAttack() == 18 + 12); // 30
    assert(warrior->getEffectiveDefense() == 8 + 8);   // 16

    // Test weapon scaling on Warrior Power Slash (200% of 30 ATK = 60 dmg)
    warrior->restoreMp(30);
    assert(warrior->useSkill(1, dmg, msg) == true);
    assert(dmg == 60);

    std::cout << "  [PASS] Equipment, effective stats, and weapon skill scaling verified.\n";

    // -------------------------------------------------------------
    // 5. Test Attribute Allocation System
    // -------------------------------------------------------------
    warrior->setStatPoints(3);
    assert(warrior->getStatPoints() == 3);
    
    int atkBefore = warrior->getAttack();
    assert(warrior->allocateStrength() == true);
    assert(warrior->getAttack() == atkBefore + 2);
    assert(warrior->getStatPoints() == 2);

    int mpBefore = warrior->getMaxMp();
    assert(warrior->allocateIntelligence() == true);
    assert(warrior->getMaxMp() == mpBefore + 10);
    assert(warrior->getStatPoints() == 1);

    int hpBefore = warrior->getMaxHp();
    int defBefore = warrior->getDefense();
    assert(warrior->allocateVitality() == true);
    assert(warrior->getMaxHp() == hpBefore + 15);
    assert(warrior->getDefense() == defBefore + 1);
    assert(warrior->getStatPoints() == 0);

    // No points left to allocate
    assert(warrior->allocateStrength() == false);

    std::cout << "  [PASS] Attribute allocation (Strength, Intelligence, Vitality) verified.\n";

    // -------------------------------------------------------------
    // 6. Test Gold Economy
    // -------------------------------------------------------------
    assert(warrior->getGold() == 0);
    warrior->addGold(150);
    assert(warrior->getGold() == 150);
    assert(warrior->spendGold(50) == true);
    assert(warrior->getGold() == 100);
    assert(warrior->spendGold(200) == false);
    assert(warrior->getGold() == 100);

    std::cout << "  [PASS] Hero gold operations verified.\n";

    // -------------------------------------------------------------
    // 7. Test Status Effects (Poison DoT & Regen HoT)
    // -------------------------------------------------------------
    warrior->applyPoison(3, 10);
    assert(warrior->isPoisoned() == true);
    assert(warrior->getPoisonTurns() == 3);
    int pDmg = warrior->takePoisonDamage();
    assert(pDmg == 10);
    assert(warrior->getPoisonTurns() == 2);

    warrior->applyRegen(2, 8);
    assert(warrior->hasRegen() == true);
    int rHeal = warrior->processRegen();
    assert(rHeal == 8);
    assert(warrior->getRegenTurns() == 1);

    warrior->clearStatusEffects();
    assert(warrior->isPoisoned() == false);
    assert(warrior->hasRegen() == false);

    std::cout << "  [PASS] Status effects (Poison & Regen) verified.\n";

    std::cout << "\n>>> ALL HERO UNIT TESTS PASSED 100%! <<<\n";
    return 0;
}
