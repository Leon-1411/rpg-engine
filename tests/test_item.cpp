#include "Item.h"
#include "Weapon.h"
#include "Armor.h"
#include "Potion.h"
#include "Hero.h"
#include <cassert>
#include <iostream>

void testLegacyItem() {
    Item item("p01", "Small Potion", "Restores 20 HP", ItemType::POTION, 20);
    assert(item.getId() == "p01");
    assert(item.getName() == "Small Potion");
    assert(item.getType() == ItemType::POTION);
    assert(item.getStatValue() == 20);
}

void testWeaponSubclass() {
    Weapon sword("w01", "Iron Sword", "Sharp blade", 15);
    assert(sword.getId() == "w01");
    assert(sword.getName() == "Iron Sword");
    assert(sword.getType() == ItemType::WEAPON);
    assert(sword.getAttackBonus() == 15);
    assert(sword.getStatValue() == 15);

    auto cloned = sword.clone();
    assert(cloned != nullptr);
    assert(cloned->getId() == "w01");
    assert(cloned->getType() == ItemType::WEAPON);
}

void testArmorSubclass() {
    Armor shield("a01", "Iron Shield", "Heavy protection", 10);
    assert(shield.getId() == "a01");
    assert(shield.getName() == "Iron Shield");
    assert(shield.getType() == ItemType::ARMOR);
    assert(shield.getDefenseBonus() == 10);
    assert(shield.getStatValue() == 10);
}

void testPotionSubclass() {
    Potion healthPot("p_hp", "Health Potion", "Heals 30 HP", 30, false, 2);
    assert(healthPot.getId() == "p_hp");
    assert(healthPot.getType() == ItemType::POTION);
    assert(healthPot.getHealAmount() == 30);
    assert(healthPot.isMana() == false);
    assert(healthPot.getQuantity() == 2);

    // Consume one
    assert(healthPot.consumeOne() == true); // quantity becomes 1
    assert(healthPot.getQuantity() == 1);
    assert(healthPot.consumeOne() == false); // quantity becomes 0
    assert(healthPot.getQuantity() == 0);

    // Mana potion
    Potion manaPot("p_mp", "Mana Potion", "Restores 20 MP", 20, true, 1);
    assert(manaPot.isMana() == true);
    Hero hero("Tester", HeroClass::MAGE, 50, 10, 10, 5);
    manaPot.apply(hero);
    assert(hero.getMp() == 30);
}

void testPolymorphism() {
    std::shared_ptr<Item> item1 = std::make_shared<Weapon>("w_poly", "Magic Staff", "Staff", 25);
    std::shared_ptr<Item> item2 = std::make_shared<Armor>("a_poly", "Robe", "Robe", 12);

    assert(item1->getType() == ItemType::WEAPON);
    assert(item1->getStatValue() == 25);

    assert(item2->getType() == ItemType::ARMOR);
    assert(item2->getStatValue() == 12);
}

int main() {
    testLegacyItem();
    testWeaponSubclass();
    testArmorSubclass();
    testPotionSubclass();
    testPolymorphism();

    std::cout << "[PASS] All Item unit tests successful!\n";
    return 0;
}
