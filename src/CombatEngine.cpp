/**
 * @file CombatEngine.cpp
 * @brief Implement CombatEngine class methods with new damage formula, Parry/Block/Evade/Defend, Items, and Boss patterns.
 * @author Lợi & Antigravity
 */

#include "CombatEngine.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>

CombatEngine::CombatEngine(Hero& hero, Enemy& enemy, Inventory* inventory)
    : hero(hero), enemy(enemy), inventory(inventory), turnCount(0), currentState(CombatState::ONGOING) {}

void CombatEngine::setInventory(Inventory* inv) {
    inventory = inv;
}

void CombatEngine::startBattle() {
    turnCount = 1;
    currentState = CombatState::ONGOING;
    hero.resetCombatStances();
    std::cout << "\n=======================================================\n";
    std::cout << "  BATTLE STARTED: " << hero.getName() << " [" << hero.getHeroClassName()
              << "] VS " << enemy.getName()
              << (enemy.getType() == EnemyType::BOSS ? " [BOSS]" : " [MINION]") << "\n";
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

void CombatEngine::processHeroStatusEffects() {
    if (hero.hasRegen()) {
        int healed = hero.processRegen();
        std::cout << "[STATUS: REGEN] " << hero.getName() << " heals for " << healed
                  << " HP! (HP: " << hero.getHp() << "/" << hero.getMaxHp()
                  << " | " << hero.getRegenTurns() << " turn(s) left)\n";
    }

    if (hero.isPoisoned()) {
        int poisonDmg = hero.takePoisonDamage();
        std::cout << "[STATUS: POISON DoT] " << hero.getName() << " suffers " << poisonDmg
                  << " poison damage! (HP: " << hero.getHp() << "/" << hero.getMaxHp()
                  << " | " << hero.getPoisonTurns() << " turn(s) left)\n";
    }
}

void CombatEngine::processEnemyStatusEffects() {
    if (enemy.hasRegen()) {
        int healed = enemy.processRegen();
        std::cout << "[STATUS: REGEN] " << enemy.getName() << " heals for " << healed
                  << " HP! (HP: " << enemy.getHp() << "/" << enemy.getMaxHp()
                  << " | " << enemy.getRegenTurns() << " turn(s) left)\n";
    }

    if (enemy.isPoisoned()) {
        int poisonDmg = enemy.takePoisonDamage();
        std::cout << "[STATUS: POISON DoT] " << enemy.getName() << " suffers " << poisonDmg
                  << " poison damage! (HP: " << enemy.getHp() << "/" << enemy.getMaxHp()
                  << " | " << enemy.getPoisonTurns() << " turn(s) left)\n";
    }
}

void CombatEngine::processStatusEffects() {
    processHeroStatusEffects();
    processEnemyStatusEffects();
}

CombatState CombatEngine::executeTurn(int actionChoice, int skillOrItemIndex) {
    if (currentState != CombatState::ONGOING) return currentState;

    std::cout << "\n--- Turn " << turnCount << " ---\n";

    // Process Status Effects (Poison DoT & Regeneration HoT) for both Hero and Enemy
    processStatusEffects();

    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " succumbed to deadly poison!\n";
        hero.addExp(enemy.getExpReward());
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    if (!hero.isAlive()) {
        std::cout << hero.getName() << " succumbed to deadly poison in battle...\n";
        currentState = CombatState::ENEMY_VICTORY;
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
        std::cout << hero.getName() << " attacks " << enemy.getName() << " for " << res.damage << " damage! ("
                  << enemy.getName() << " HP: " << enemy.getHp() << "/" << enemy.getMaxHp() << ")\n";

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
                std::cout << "Skill deals " << res.damage << " damage to " << enemy.getName() << "! ("
                          << enemy.getName() << " HP: " << enemy.getHp() << "/" << enemy.getMaxHp() << ")\n";
            }
        } else {
            std::cout << "[SKILL FAILED] " << skillMsg << "\n";
            return currentState; // Allow re-action if skill was unavailable
        }

    } else if (actionChoice == 3) { // Item (Potion - Mage Exclusive)
        if (hero.getHeroClass() != HeroClass::MAGE) {
            std::cout << "[RESTRICTION] Potions are part of the Mage's exclusive craft and skill set! "
                      << hero.getName() << " [" << hero.getHeroClassName()
                      << "] cannot use potions. Use Skills or Defend instead!\n";
            return currentState;
        }
        if (!inventory || inventory->getItemCount() == 0) {
            std::cout << "[ITEM FAILED] No inventory or no items available to use!\n";
            return currentState;
        }
        if (skillOrItemIndex < 0 || skillOrItemIndex >= inventory->getItemCount()) {
            std::cout << "[ITEM FAILED] Invalid item index (" << skillOrItemIndex + 1 << ")!\n";
            return currentState;
        }
        Item item = inventory->getItem(skillOrItemIndex);
        if (item.getType() != ItemType::POTION) {
            std::cout << "[ITEM FAILED] " << item.getName() << " is not a usable Potion in battle!\n";
            return currentState;
        }
        int beforeHp = hero.getHp();
        if (inventory->useItem(skillOrItemIndex, hero)) {
            int healed = hero.getHp() - beforeHp;
            std::cout << hero.getName() << " consumed " << item.getName() << " and restored "
                      << healed << " HP! (HP: " << hero.getHp() << "/" << hero.getMaxHp() << ")\n";
        } else {
            std::cout << "[ITEM FAILED] Could not use item!\n";
            return currentState;
        }

    } else if (actionChoice == 4) { // Defend
        hero.setIsDefending(true);
        std::cout << hero.getName() << " assumes a defensive stance, bracing to reduce next incoming damage by 50%!\n";

    } else if (actionChoice == 5) { // Flee
        std::cout << hero.getName() << " fled from battle!\n";
        currentState = CombatState::FLED;
        return currentState;
    } else {
        std::cout << "[INVALID ACTION] Choice must be between 1 and 5.\n";
        return currentState;
    }

    // Check if enemy defeated after player action
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
    if (!enemy.isAlive()) return;

    int enemyAction = enemy.chooseAction(hero.getHeroClass(), turnCount);

    if (enemyAction == 0) { // Enemy Misses or Hesitates
        std::cout << enemy.getName() << " hesitates and misses the attack!\n";

        // Warrior Parry penalty: mistimed parry locks skill for 1 turn
        if (hero.getIsParrying()) {
            hero.lockSkills(1);
            std::cout << "[MISTIMED PARRY] " << hero.getName()
                      << " anticipated an attack that never came! Skills are locked for 1 turn.\n";
        }
        hero.resetCombatStances();

    } else { // Enemy Attacks (Action 1 = Normal, Action 2 = Heavy, Action 3 = Special)
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
            // Calculate base enemy attack power based on action type
            int baseAtk = enemy.getAttack();
            int pen = enemy.getArmorPenetration();

            if (enemyAction == 2) {
                baseAtk = static_cast<int>(baseAtk * 1.5f);
                std::cout << "[BOSS HEAVY ATTACK!] " << enemy.getName() << " unleashes a devastating smash!\n";
            } else if (enemyAction == 3) {
                baseAtk = static_cast<int>(baseAtk * 1.2f);
                pen += 10; // Dark surge has increased armor penetration
                std::cout << "[BOSS SPECIAL SKILL!] " << enemy.getName() << " channels Dark Surge with high penetration!\n";
            }

            DamageResult res = calculateDamage(baseAtk, enemy.getCritChance(), enemy.getCritDamage(),
                                               pen, hero.getDefense(), false);

            // Defend stance reduces damage by 50%
            if (hero.getIsDefending()) {
                int originalDamage = res.damage;
                res.damage = std::max(1, res.damage / 2);
                std::cout << "[DEFEND GUARD] " << hero.getName() << " guarded against the strike! Reduced damage from "
                          << originalDamage << " to " << res.damage << ".\n";
            }

            hero.takeDamage(res.damage);
            if (res.isCrit) {
                std::cout << "[CRITICAL HIT!] ";
            }
            std::cout << enemy.getName() << " hits " << hero.getName() << " for " << res.damage << " damage! ("
                      << hero.getName() << " HP: " << hero.getHp() << "/" << hero.getMaxHp() << ")\n";

            // Poisonous Enemy trait: applies poison to Hero on successful hit
            if (enemy.getIsPoisonous() && hero.isAlive()) {
                hero.applyPoison(enemy.getPoisonInflictTurns(), enemy.getPoisonInflictDmg());
                std::cout << "[VENOM INFLICTED] " << enemy.getName() << " injects venom! "
                          << hero.getName() << " is poisoned for " << enemy.getPoisonInflictTurns()
                          << " turns (" << enemy.getPoisonInflictDmg() << " dmg/turn)!\n";
            }
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

std::string CombatEngine::renderBar(int current, int max, int length) {
    if (max <= 0) max = 1;
    current = std::clamp(current, 0, max);
    int filled = (current * length) / max;
    int empty = length - filled;
    std::string bar = "[";
    bar.append(filled, '=');
    bar.append(empty, ' ');
    bar.append("] " + std::to_string(current) + "/" + std::to_string(max));
    return bar;
}

void CombatEngine::displayBattleStatus(std::ostream& out) const {
    out << "\n=======================================================\n";
    out << " TURN " << turnCount << " | BATTLE STATUS\n";
    out << "-------------------------------------------------------\n";
    out << " " << hero.getName() << " [" << hero.getHeroClassName() << " Lv." << hero.getLevel() << "]\n";
    out << "   HP:  " << renderBar(hero.getHp(), hero.getMaxHp(), 20) << "\n";
    out << "   ATK: " << hero.getAttack() << " | DEF: " << hero.getDefense();
    if (hero.getHeroClass() == HeroClass::RANGER) {
        out << " | Arrows: " << hero.getReadyArrows();
    }
    if (hero.isPoisoned()) {
        out << " | [POISON: " << hero.getPoisonTurns() << "t, " << hero.getPoisonDamagePerTurn() << " dmg/t]";
    }
    if (hero.hasRegen()) {
        out << " | [REGEN: " << hero.getRegenTurns() << "t, +" << hero.getRegenPerTurn() << " HP/t]";
    }
    out << "\n";
    out << "   CD:  [1] " << hero.getSkillCooldown(1) << "t | [2] "
        << hero.getSkillCooldown(2) << "t | [3] " << hero.getSkillCooldown(3) << "t\n";
    out << " VS\n";
    out << " " << enemy.getName() << (enemy.getType() == EnemyType::BOSS ? " [BOSS]" : " [MINION]") << "\n";
    out << "   HP:  " << renderBar(enemy.getHp(), enemy.getMaxHp(), 20) << "\n";
    out << "   ATK: " << enemy.getAttack() << " | DEF: " << enemy.getDefense();
    if (enemy.isPoisoned()) {
        out << " | [POISON: " << enemy.getPoisonTurns() << "t]";
    }
    if (enemy.hasRegen()) {
        out << " | [REGEN: " << enemy.getRegenTurns() << "t]";
    }
    out << "\n=======================================================\n";
}

void CombatEngine::runInteractiveBattle(std::istream& in, std::ostream& out) {
    startBattle();

    while (!isBattleOver()) {
        displayBattleStatus(out);

        out << "Actions:\n";
        out << " 1. Normal Attack\n";
        out << " 2. Skill\n";
        out << " 3. Item (Potions - Mage exclusive)\n";
        out << " 4. Defend (Reduce damage by 50%)\n";
        out << " 5. Flee\n";
        out << "Choose action (1-5): ";

        int choice = 0;
        if (!(in >> choice)) {
            break;
        }

        int subIndex = -1;
        if (choice == 2) {
            out << "Choose Skill:\n";
            if (hero.getHeroClass() == HeroClass::WARRIOR) {
                out << " 1. Sword Slash (Damage x1.8, CD 1) [CD: " << hero.getSkillCooldown(1) << "]\n";
                out << " 2. Parry (Counter enemy attack, CD 1) [CD: " << hero.getSkillCooldown(2) << "]\n";
                out << " 3. Shield Block (100% block, CD 5) [CD: " << hero.getSkillCooldown(3) << "]\n";
            } else if (hero.getHeroClass() == HeroClass::RANGER) {
                out << " 1. Evade & Reload (Dodge +5 arrows, CD 2) [CD: " << hero.getSkillCooldown(1) << "]\n";
                out << " 2. Aimed Shot (Cost 1 arrow, CD 1) [CD: " << hero.getSkillCooldown(2) << "]\n";
                out << " 3. Arrow Barrage (Unleash all arrows, CD 5) [CD: " << hero.getSkillCooldown(3) << "]\n";
            } else if (hero.getHeroClass() == HeroClass::MAGE) {
                out << " 1. Energy Ray (Damage x1.2, CD 1) [CD: " << hero.getSkillCooldown(1) << "]\n";
                out << " 2. Healing Potion (Heal 30 HP, CD 3) [CD: " << hero.getSkillCooldown(2) << "]\n";
                out << " 3. Poison Flask (Poison 5 turns, CD 5) [CD: " << hero.getSkillCooldown(3) << "]\n";
            }
            out << "Select skill (1-3): ";
            in >> subIndex;
        } else if (choice == 3) {
            if (hero.getHeroClass() != HeroClass::MAGE) {
                out << "[RESTRICTION] Potions are part of the Mage's exclusive skillset! "
                    << hero.getName() << " [" << hero.getHeroClassName()
                    << "] cannot use potions. Use Skills or Defend instead!\n";
            } else if (!inventory || inventory->getItemCount() == 0) {
                out << "Your inventory is empty!\n";
            } else {
                out << "Potions in Inventory:\n";
                std::vector<int> potionIndices;
                for (int i = 0; i < inventory->getItemCount(); ++i) {
                    Item item = inventory->getItem(i);
                    if (item.getType() == ItemType::POTION) {
                        potionIndices.push_back(i);
                        out << " " << potionIndices.size() << ". " << item.getName()
                            << " (Heal +" << item.getStatValue() << " HP)\n";
                    }
                }
                if (potionIndices.empty()) {
                    out << "No potions found in inventory!\n";
                } else {
                    out << "Select potion (1-" << potionIndices.size() << "): ";
                    int pSelect = 0;
                    in >> pSelect;
                    if (pSelect >= 1 && pSelect <= static_cast<int>(potionIndices.size())) {
                        subIndex = potionIndices[pSelect - 1];
                    }
                }
            }
        }

        executeTurn(choice, subIndex);
    }

    out << "\n=======================================================\n";
    if (currentState == CombatState::HERO_VICTORY) {
        out << "  VICTORY! " << hero.getName() << " defeated " << enemy.getName() << "!\n";
        out << "  Reward: +" << enemy.getExpReward() << " EXP, +" << enemy.getGoldReward() << " Gold!\n";
    } else if (currentState == CombatState::ENEMY_VICTORY) {
        out << "  DEFEAT! " << hero.getName() << " has fallen in battle...\n";
    } else if (currentState == CombatState::FLED) {
        out << "  " << hero.getName() << " successfully escaped from battle.\n";
    }
    out << "=======================================================\n";
}
