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

    // Mana potion: Hero ph\u1ea3i c\u00f3 maxMp \u0111\u1ee7 l\u1edbn \u0111\u1ec3 kh\u00f4ng b\u1ecb cap
    Potion manaPot("p_mp", "Mana Potion", "Restores 20 MP", 20, true, 1);
    assert(manaPot.isMana() == true);
    Hero hero("Tester", HeroClass::MAGE, 50, 50, 10, 5);  // maxMp=50
    hero.setMp(10);    // Hi\u1ec7n t\u1ea1i ch\u1ec9 c\u00f2n 10 MP
    manaPot.apply(hero);
    assert(hero.getMp() == 30); // 10 + 20 = 30 (\u2264 maxMp=50)

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
