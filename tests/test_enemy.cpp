#include "Enemy.h"
#include <cassert>
#include <iostream>

int main() {
    Enemy enemy("Goblin", EnemyType::MINION, 50, 10, 2, 20, 5);

    assert(enemy.getName() == "Goblin");
    assert(enemy.getHp() == 50);
    assert(enemy.isAlive() == true);

    enemy.takeDamage(12); // 12 - 2 = 10 effective damage -> 40 HP
    assert(enemy.getHp() == 40);

    enemy.takeDamage(100);
    assert(enemy.isAlive() == false);

    std::cout << "[PASS] Enemy unit tests successful!\n";
    return 0;
}
