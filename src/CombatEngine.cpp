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
        int damage = calculateDamage(hero.getAttack(), enemy.getDefense());
        enemy.takeDamage(hero.getAttack());
        std::cout << hero.getName() << " attacks " << enemy.getName() << " for " << damage << " damage!\n";
    } else if (actionChoice == 2) { // Skill
        int skillDmg = 0;
        if (hero.useSkill(skillOrItemIndex, skillDmg)) {
            int damage = calculateDamage(skillDmg, enemy.getDefense());
            enemy.takeDamage(skillDmg);
            std::cout << hero.getName() << " uses skill on " << enemy.getName() << " for " << damage << " damage!\n";
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
    if (enemyAction == 2) {
        std::string skillName = enemy.getSpecialSkillName();
        if (skillName.empty()) skillName = "Special Skill";
        int skillDmg = static_cast<int>(enemy.getAttack() * 1.4);
        int damage = calculateDamage(skillDmg, hero.getDefense());
        hero.takeDamage(skillDmg);
        std::cout << enemy.getName() << " unleashes [" << skillName << "] on "
                  << hero.getName() << " for " << damage << " damage!\n";
    } else {
        int damage = calculateDamage(enemy.getAttack(), hero.getDefense());
        hero.takeDamage(enemy.getAttack());
        std::cout << enemy.getName() << " attacks " << hero.getName() << " for " << damage << " damage!\n";
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
