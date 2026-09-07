/**
 * @file CombatEngine.cpp
 * @brief Implement CombatEngine class methods with new damage formula, Parry/Block/Evade, and DoT.
 * @author Lợi & Antigravity
 */

#include "CombatEngine.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>

CombatEngine::CombatEngine(Hero& hero, Enemy& enemy)
    : hero(hero), enemy(enemy), turnCount(0), currentState(CombatState::ONGOING) {}

void CombatEngine::startBattle() {
    turnCount = 1;
    currentState = CombatState::ONGOING;
    hero.resetCombatStances();
    std::cout << "\n=======================================================\n";
    std::cout << "  BATTLE STARTED: " << hero.getName() << " [" << hero.getHeroClassName()
              << "] VS " << enemy.getName() << "\n";
    std::cout << "=======================================================\n";
}

DamageResult CombatEngine::calculateDamage(int attackerAttack, float critChance, float critDmg,
                                           int armorPen, int defenderDefense, bool ignoreArmor) const {
    bool isCrit = false;
    float baseDmg = static_cast<float>(attackerAttack);

    if (critChance > 0.0f) {
        float roll = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        if (roll <= critChance) {
            isCrit = true;
            baseDmg = attackerAttack * (1.0f + critDmg);
        }
    }

    int finalDmg = static_cast<int>(baseDmg);

    // If target has defense and attacker does not ignore armor
    if (!ignoreArmor && defenderDefense > 0) {
        int effectiveArmor = std::max(0, defenderDefense - armorPen);
        finalDmg = finalDmg - effectiveArmor;
    }

    // Damage cannot be less than 1
    finalDmg = std::max(1, finalDmg);

    return { finalDmg, isCrit };
}

int CombatEngine::calculateDamage(int attackerAttack, int defenderDefense) const {
    return std::max(1, attackerAttack - defenderDefense);
}

void CombatEngine::processPoisonTick() {
    if (enemy.isPoisoned()) {
        int poisonDmg = enemy.takePoisonDamage();
        std::cout << "[POISON DoT] " << enemy.getName() << " suffers " << poisonDmg
                  << " poison damage! (HP: " << enemy.getHp() << "/" << enemy.getMaxHp()
                  << " | " << enemy.getPoisonTurns() << " turn(s) left)\n";
    }
}

CombatState CombatEngine::executeTurn(int actionChoice, int skillOrItemIndex) {
    if (currentState != CombatState::ONGOING) return currentState;

    std::cout << "\n--- Turn " << turnCount << " ---\n";

    // Trigger Poison DoT on enemy at turn beginning
    processPoisonTick();
    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " succumbed to deadly poison!\n";
        hero.addExp(enemy.getExpReward());
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    // 1. Player action
    if (actionChoice == 1) { // Normal Attack
        DamageResult res = calculateDamage(hero.getAttack(), hero.getCritChance(), hero.getCritDamage(),
                                           hero.getArmorPenetration(), enemy.getDefense(), hero.isIgnoreArmor());
        enemy.takeDamage(res.damage);
        if (res.isCrit) {
            std::cout << "[CRITICAL HIT!] ";
        }
        std::cout << hero.getName() << " attacks " << enemy.getName() << " for " << res.damage << " damage!\n";

    } else if (actionChoice == 2) { // Skill
        int skillDmg = 0;
        std::string skillMsg;
        if (hero.useSkill(skillOrItemIndex, skillDmg, skillMsg)) {
            std::cout << skillMsg << "\n";

            if (skillDmg == -3) { // Mage Skill 3: Poison Flask
                enemy.applyPoison(5, 8);
                std::cout << "[POISON APPLIED] " << enemy.getName() << " is poisoned for 5 turns (8 dmg/turn)!\n";
            } else if (skillDmg > 0) { // Damaging Skill
                DamageResult res = calculateDamage(skillDmg, hero.getCritChance(), hero.getCritDamage(),
                                                   hero.getArmorPenetration(), enemy.getDefense(), hero.isIgnoreArmor());
                enemy.takeDamage(res.damage);
                if (res.isCrit) {
                    std::cout << "[CRITICAL HIT!] ";
                }
                std::cout << "Skill deals " << res.damage << " damage to " << enemy.getName() << "!\n";
            }
        } else {
            std::cout << "[SKILL FAILED] " << skillMsg << "\n";
            return currentState; // Allow re-action if skill was unavailable
        }

    } else if (actionChoice == 5) { // Flee
        std::cout << hero.getName() << " fled from battle!\n";
        currentState = CombatState::FLED;
        return currentState;
    }

    // Check if enemy defeated
    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " was defeated!\n";
        hero.addExp(enemy.getExpReward());
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    // 2. Enemy Turn
    processEnemyTurn();

    // Check if hero or enemy defeated
    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " was defeated!\n";
        hero.addExp(enemy.getExpReward());
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    if (!hero.isAlive()) {
        std::cout << hero.getName() << " was defeated in battle...\n";
        currentState = CombatState::ENEMY_VICTORY;
        return currentState;
    }

    // End of full round: reduce skill cooldowns and increment turn
    hero.reduceCooldowns();
    turnCount++;

    return currentState;
}

void CombatEngine::processEnemyTurn() {
    std::cout << "\n[Enemy Turn - " << enemy.getName() << "]\n";

    // Poison damage can trigger on enemy turn as well
    processPoisonTick();
    if (!enemy.isAlive()) return;

    int enemyAction = enemy.chooseAction(hero.getHeroClass());

    if (enemyAction == 0) { // Enemy Misses or Hesitates
        std::cout << enemy.getName() << " hesitates and misses the attack!\n";

        // Warrior Parry penalty: mistimed parry locks skill for 1 turn
        if (hero.getIsParrying()) {
            hero.lockSkills(1);
            std::cout << "[MISTIMED PARRY] " << hero.getName()
                      << " anticipated an attack that never came! Skills are locked for 1 turn.\n";
        }
        hero.resetCombatStances();

    } else if (enemyAction == 1) { // Enemy Attacks
        if (hero.getIsParrying()) {
            // Warrior Parry Success: counterattack
            DamageResult counterRes = calculateDamage(static_cast<int>(hero.getAttack() * 1.5f),
                                                     hero.getCritChance(), hero.getCritDamage(),
                                                     hero.getArmorPenetration(), enemy.getDefense(), hero.isIgnoreArmor());
            enemy.takeDamage(counterRes.damage);
            std::cout << "[PARRY SUCCESS!] " << hero.getName() << " parried the incoming attack and countered for "
                      << counterRes.damage << " damage!\n";

        } else if (hero.getIsBlocking()) {
            // Warrior Block Success: 0 damage
            std::cout << "[BLOCK SUCCESS!] " << hero.getName()
                      << " raised shield into Block stance! 100% damage nullified (0 damage taken).\n";

        } else if (hero.getIsEvading()) {
            // Ranger Evade Success: dodge 100% and reload 5 ready arrows
            hero.addReadyArrows(5);
            std::cout << "[EVADE SUCCESS!] " << hero.getName()
                      << " swiftly dodged " << enemy.getName()
                      << "'s strike (0 damage taken) and reloaded +5 Ready Arrows! (Current: "
                      << hero.getReadyArrows() << ")\n";

        } else {
            // Normal Enemy Attack hit
            DamageResult res = calculateDamage(enemy.getAttack(), enemy.getCritChance(), enemy.getCritDamage(),
                                               enemy.getArmorPenetration(), hero.getDefense(), false);
            hero.takeDamage(res.damage);
            if (res.isCrit) {
                std::cout << "[CRITICAL HIT!] ";
            }
            std::cout << enemy.getName() << " attacks " << hero.getName() << " for " << res.damage << " damage!\n";
        }

        hero.resetCombatStances();
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
