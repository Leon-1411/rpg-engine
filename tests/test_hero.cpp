#include "Hero.h"
#include <cassert>
#include <iostream>

int main() {
    Hero hero("TestHero", HeroClass::WARRIOR, 100, 50, 15, 5);
    
    assert(hero.getName() == "TestHero");
    assert(hero.getHp() == 100);
    assert(hero.isAlive() == true);

    hero.takeDamage(10); // 10 - 5 = 5 effective dmg -> 95 hp
    assert(hero.getHp() == 95);

    hero.heal(10);
    assert(hero.getHp() == 100);

    hero.takeDamage(500);
    assert(hero.getHp() == 0);
    assert(hero.isAlive() == false);

    std::cout << "[PASS] Hero unit tests successful!\n";
    return 0;
}
