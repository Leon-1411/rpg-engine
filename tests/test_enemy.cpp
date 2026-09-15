#include "Enemy.h"
#include <cassert>
#include <iostream>

int main() {
    Enemy enemy("Goblin", EnemyType::MINION, 50, 10, 2, 20, 5);

    assert(enemy.getName() == "Goblin");
    assert(enemy.getHp() == 50);
    assert(enemy.isAlive() == true);

    enemy.takeDamage(10); // 50 - 10 = 40 HP (direct damage, no double armor reduction)
    assert(enemy.getHp() == 40);

    // Test poison
    enemy.applyPoison(2, 5);
    assert(enemy.isPoisoned() == true);
    assert(enemy.takePoisonDamage() == 5);
    assert(enemy.getHp() == 35);

    enemy.takeDamage(100);
    assert(enemy.isAlive() == false);

    std::cout << "[PASS] Enemy unit tests successful!\n";
    return 0;
}
