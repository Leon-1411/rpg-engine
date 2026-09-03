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

    Hero hero("Tester", HeroClass::WARRIOR, 100, 20, 10, 5);
    hero.takeDamage(40); // HP = 65
    assert(hero.getHp() == 65);

    // Use potion at index 0
    assert(inv.useItem(0, hero) == true);
    assert(hero.getHp() == 95);
    assert(inv.getItemCount() == 1);

    std::cout << "[PASS] Inventory unit tests successful!\n";
    return 0;
}
