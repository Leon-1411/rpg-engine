/**
 * @file CombatEngine.cpp
 * @brief Implement CombatEngine class methods.
 * @author Lợi
 */

#include "CombatEngine.h"
#include <algorithm>

CombatEngine::CombatEngine(Hero& hero, Enemy& enemy)
    : hero(hero), enemy(enemy), turnCount(0), currentState(CombatState::ONGOING) {}

void CombatEngine::startBattle() {
    turnCount = 1;
    currentState = CombatState::ONGOING;
    std::cout << "\n=== BATTLE STARTED: " << hero.getName() << " VS " << enemy.getName() << " ===\n";
}

CombatState CombatEngine::executeTurn(int actionChoice, int skillOrItemIndex) {
    if (currentState != CombatState::ONGOING) return currentState;

    std::cout << "\n--- Turn " << turnCount << " ---\n";

    // Player action
    if (actionChoice == 1) { // Normal Attack
        // Truyền raw attack; Enemy::takeDamage tự áp defense của nó
        int rawAtk     = hero.getEffectiveAttack();
        int displayDmg = calculateDamage(rawAtk, enemy.getDefense()); // chỉ để log
        enemy.takeDamage(rawAtk);
        std::cout << hero.getName() << " attacks " << enemy.getName()
                  << " for " << displayDmg << " damage!\n";
    } else if (actionChoice == 2) { // Skill
        int skillDmg = 0;
        if (hero.useSkill(skillOrItemIndex, skillDmg)) {
            // skillDmg là damage thô từ skill; truyền thẳng, Enemy tự giảm defense
            int displayDmg = calculateDamage(skillDmg, enemy.getDefense());
            enemy.takeDamage(skillDmg);
            std::cout << hero.getName() << " uses skill on " << enemy.getName()
                      << " for " << displayDmg << " damage!\n";
        } else {
            std::cout << "Skill execution failed (Not enough MP or invalid skill)!\n";
        }
    } else if (actionChoice == 5) { // Flee
        std::cout << hero.getName() << " fled from battle!\n";
        currentState = CombatState::FLED;
        return currentState;
    }

    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " was defeated!\n";
        hero.addExp(enemy.getExpReward());
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    // Enemy Turn
    processEnemyTurn();

    if (!hero.isAlive()) {
        std::cout << hero.getName() << " was defeated in battle...\n";
        currentState = CombatState::ENEMY_VICTORY;
        return currentState;
    }

    turnCount++;
    return currentState;
}

int CombatEngine::calculateDamage(int attackerAttack, int defenderDefense) const {
    return std::max(1, attackerAttack - defenderDefense);
}

void CombatEngine::processEnemyTurn() {
    int enemyAction = enemy.chooseAction();
    if (enemyAction == 1) {
        // Truyền raw attack; Hero::takeDamage tự áp getEffectiveDefense() bên trong
        int rawAtk     = enemy.getAttack();
        int displayDmg = calculateDamage(rawAtk, hero.getEffectiveDefense()); // chỉ để log
        hero.takeDamage(rawAtk);
        std::cout << enemy.getName() << " attacks " << hero.getName()
                  << " for " << displayDmg << " damage!\n";
    }
}

bool CombatEngine::isBattleOver() const {
    return currentState != CombatState::ONGOING;
}

CombatState CombatEngine::getState() const {
    return currentState;
}

int CombatEngine::getTurnCount() const {
    return turnCount;
}
