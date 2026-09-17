/**
 * @file CombatEngine.cpp
 * @brief Implement CombatEngine class methods with 5 actions and clean logging.
 */

#include "CombatEngine.h"
#include "BossMonster.h"
#include <algorithm>
#include <iostream>

CombatEngine::CombatEngine(Hero& hero, Enemy& enemy, Inventory* inv)
    : hero(hero),
      enemy(enemy),
      inventory(inv),
      turnCount(0),
      currentState(CombatState::ONGOING),
      isHeroDefending(false),
      isEnemyDefending(false) {}

void CombatEngine::startBattle() {
    turnCount = 1;
    currentState = CombatState::ONGOING;
    isHeroDefending = false;
    isEnemyDefending = false;
    turnLogs.clear();

    std::string startMsg = "=== BATTLE STARTED: " + hero.getName() + " VS " + enemy.getName() + " ===";
    turnLogs.push_back(startMsg);
    std::cout << "\n" << startMsg << "\n";
}

CombatState CombatEngine::executeTurn(int actionChoice, int skillOrItemIndex) {
    if (currentState != CombatState::ONGOING) return currentState;

    turnLogs.clear();
    std::string turnHeader = "--- Turn " + std::to_string(turnCount) + " ---";
    turnLogs.push_back(turnHeader);
    std::cout << "\n" << turnHeader << "\n";

    // Reset hero defending status at the start of player turn
    isHeroDefending = false;

    // 1. Player Action
    if (actionChoice == 1) { // Normal Attack
        int rawDmg = hero.getAttack();
        int damage = DamageCalculator::calculateEffectiveDamage(rawDmg, enemy.getDefense(), isEnemyDefending);
        enemy.takeDamage(rawDmg);
        std::string log = hero.getName() + " attacks " + enemy.getName() + " for " + std::to_string(damage) + " damage!";
        turnLogs.push_back(log);
        std::cout << log << "\n";
    } else if (actionChoice == 2) { // Special Skill
        int skillDmg = 0;
        if (hero.useSkill(skillOrItemIndex, skillDmg)) {
            int damage = DamageCalculator::calculateEffectiveDamage(skillDmg, enemy.getDefense(), isEnemyDefending);
            enemy.takeDamage(skillDmg);
            std::string skillName = hero.getSkillName(skillOrItemIndex);
            if (skillName.empty() || skillName == "Unknown") skillName = "Special Skill";
            std::string log = hero.getName() + " uses [" + skillName + "] on " + enemy.getName() + " for " + std::to_string(damage) + " damage!";
            turnLogs.push_back(log);
            std::cout << log << "\n";
        } else {
            std::string log = "Skill execution failed (Not enough MP or invalid skill)! Used basic attack instead.";
            turnLogs.push_back(log);
            std::cout << log << "\n";
            int rawDmg = hero.getAttack();
            int damage = DamageCalculator::calculateEffectiveDamage(rawDmg, enemy.getDefense(), isEnemyDefending);
            enemy.takeDamage(rawDmg);
        }
    } else if (actionChoice == 3) { // Use Item (Potion)
        bool itemUsed = false;
        if (inventory && skillOrItemIndex >= 0 && skillOrItemIndex < inventory->getItemCount()) {
            Item item = inventory->getItem(skillOrItemIndex);
            if (inventory->useItem(skillOrItemIndex, hero)) {
                std::string log = hero.getName() + " uses " + item.getName() + " (" + item.getDescription() + ")!";
                turnLogs.push_back(log);
                std::cout << log << "\n";
                itemUsed = true;
            }
        }
        if (!itemUsed) {
            std::string log = hero.getName() + " tried to use an item, but the pouch was empty or invalid!";
            turnLogs.push_back(log);
            std::cout << log << "\n";
        }
    } else if (actionChoice == 4) { // Defend
        isHeroDefending = true;
        std::string log = hero.getName() + " raises guard into a defensive stance! Defense is doubled this turn!";
        turnLogs.push_back(log);
        std::cout << log << "\n";
    } else if (actionChoice == 5) { // Flee / Run
        // Minions can be fled from; Bosses prevent fleeing
        if (enemy.getType() == EnemyType::BOSS) {
            std::string log = "You cannot flee from a Boss battle! The terrifying presence blocks your path!";
            turnLogs.push_back(log);
            std::cout << log << "\n";
        } else {
            std::string log = hero.getName() + " fled from battle!";
            turnLogs.push_back(log);
            std::cout << log << "\n";
            currentState = CombatState::FLED;
            return currentState;
        }
    }

    // Check if Enemy is defeated
    if (!enemy.isAlive()) {
        std::string log = enemy.getName() + " was defeated!";
        turnLogs.push_back(log);
        std::cout << log << "\n";
        hero.addExp(enemy.getExpReward());
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    // 2. Enemy Turn
    processEnemyTurn();

    // Check if Hero is defeated
    if (!hero.isAlive()) {
        std::string log = hero.getName() + " was defeated in battle...";
        turnLogs.push_back(log);
        std::cout << log << "\n";
        currentState = CombatState::ENEMY_VICTORY;
        return currentState;
    }

    turnCount++;
    return currentState;
}

int CombatEngine::calculateDamage(int attackerAttack, int defenderDefense) const {
    return DamageCalculator::calculateEffectiveDamage(attackerAttack, defenderDefense, isHeroDefending);
}

void CombatEngine::processEnemyTurn() {
    BossMonster* bossPtr = dynamic_cast<BossMonster*>(&enemy);
    int enemyAction = enemy.chooseAction();

    if (enemyAction == 2) { // Special Skill
        std::string skillName = enemy.getSpecialSkillName();
        if (skillName.empty()) skillName = "Special Skill";

        double multiplier = 1.4;
        if (bossPtr) {
            multiplier = bossPtr->getActiveSkillMultiplier();
            bossPtr->updateCooldowns(1);
        }

        int rawSkillDmg = DamageCalculator::calculateRawSkillDamage(enemy.getAttack(), multiplier);
        if (isHeroDefending) {
            rawSkillDmg = std::max(1, rawSkillDmg / 2);
        }

        int damage = DamageCalculator::calculateEffectiveDamage(rawSkillDmg, hero.getDefense(), isHeroDefending);
        hero.takeDamage(rawSkillDmg);

        std::string log = enemy.getName() + " unleashes [" + skillName + "] on " + hero.getName() + " for " + std::to_string(damage) + " damage!";
        turnLogs.push_back(log);
        std::cout << log << "\n";
    } else { // Normal Attack
        int rawAtk = enemy.getAttack();
        if (isHeroDefending) {
            rawAtk = std::max(1, rawAtk / 2);
        }

        if (bossPtr) {
            bossPtr->updateCooldowns(1);
        }

        int damage = DamageCalculator::calculateEffectiveDamage(rawAtk, hero.getDefense(), isHeroDefending);
        hero.takeDamage(rawAtk);

        std::string log = enemy.getName() + " attacks " + hero.getName() + " for " + std::to_string(damage) + " damage!";
        turnLogs.push_back(log);
        std::cout << log << "\n";
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

const std::vector<std::string>& CombatEngine::getTurnLogs() const {
    return turnLogs;
}

void CombatEngine::clearLogs() {
    turnLogs.clear();
}
