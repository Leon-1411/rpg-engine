#include "Inventory.h"
#include "Hero.h"
#include <cassert>
#include <iostream>

int main() {
    Inventory inv(5);
    Item pot("p01", "Health Potion", "Heals 30 HP", ItemType::POTION, 30);
    Item wpn("w01", "Sword", "Weapon", ItemType::WEAPON, 15);

    assert(inv.addItem(pot) == true);
    assert(inv.addItem(wpn) == true);
    assert(inv.getItemCount() == 2);

    // Rule: Potions are exclusive to Mage! Warrior cannot use potions.
    Hero warrior("WarriorTester", HeroClass::WARRIOR, 100, 10, 5);
    warrior.takeDamage(35); // 100 - 35 = 65
    assert(warrior.getHp() == 65);
    assert(inv.useItem(0, warrior) == false); // Warrior fails to use potion
    assert(warrior.getHp() == 65);            // HP unchanged
    assert(inv.getItemCount() == 2);          // Potion not consumed

    // Mage can use potions
    Hero mage("MageTester", HeroClass::MAGE, 100, 10, 5);
    mage.takeDamage(35); // 100 - 35 = 65
    assert(mage.getHp() == 65);
    assert(inv.useItem(0, mage) == true);    // Mage successfully uses potion
    assert(mage.getHp() == 95);
    assert(inv.getItemCount() == 1);          // Potion consumed

    std::cout << "[PASS] Inventory unit tests successful!\n";
    return 0;
}
