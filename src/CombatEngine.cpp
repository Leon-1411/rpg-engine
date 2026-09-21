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
    } else if (actionChoice == 3) { // Use Potion / Item
        bool used = false;
        if (skillOrItemIndex >= 0) {
            // Dùng item theo chỉ mục cụ thể
            used = hero.getInventory().useItem(skillOrItemIndex, hero);
        } else {
            // Tự động tìm lọ Potion đầu tiên trong túi đồ
            const auto& items = hero.getInventory().getItems();
            for (int i = 0; i < static_cast<int>(items.size()); ++i) {
                if (items[i] && items[i]->getType() == ItemType::POTION) {
                    used = hero.getInventory().useItem(i, hero);
                    break;
                }
            }
        }

        if (used) {
            std::cout << hero.getName() << " used a potion! HP: " 
                      << hero.getHp() << "/" << hero.getMaxHp() 
                      << " | MP: " << hero.getMp() << "/" << hero.getMaxMp() << "\n";
        } else {
            std::cout << "Item usage failed (No potion available or invalid item)!\n";
            return currentState; // Không tốn lượt nếu dùng thất bại
        }
    } else if (actionChoice == 4) { // Defend
        isDefending = true;
        std::cout << hero.getName() << " raises defense stance! Incoming damage will be halved this turn.\n";
    } else if (actionChoice == 5) { // Flee
        std::cout << hero.getName() << " fled from battle!\n";
        currentState = CombatState::FLED;
        return currentState;
    }

    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " was defeated!\n";
        hero.addExp(enemy.getExpReward());

        // Xử lý rớt vật phẩm (Weapon, Armor, Health Potion, ...)
        lastDroppedItems = enemy.rollDrops();
        for (const auto& item : lastDroppedItems) {
            if (item) {
                std::cout << "[Item Drop] " << enemy.getName() << " dropped: " << item->getName() << "!\n";
                if (hero.getInventory().addItem(item)) {
                    std::cout << "Added '" << item->getName() << "' to " << hero.getName() << "'s inventory.\n";
                } else {
                    std::cout << "[Inventory Full] Could not take '" << item->getName() << "'!\n";
                }
            }
        }

        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    // Enemy Turn
    processEnemyTurn();

    // Reset defense stance after enemy turn
    isDefending = false;

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
    int heroDef = hero.getEffectiveDefense();
    if (isDefending) {
        // Tăng gấp đôi defense hiệu quả khi đang trong trạng thái phòng thủ (Defend)
        heroDef *= 2;
    }

    if (enemyAction == 2) {
        std::string skillName = enemy.getSpecialSkillName();
        if (skillName.empty()) skillName = "Special Skill";
        int skillDmg = static_cast<int>(enemy.getAttack() * 1.4);
        int effectiveDmg = calculateDamage(skillDmg, heroDef);
        if (isDefending) {
            effectiveDmg = std::max(1, effectiveDmg / 2);
        }
        hero.setHp(hero.getHp() - effectiveDmg);
        std::cout << enemy.getName() << " unleashes [" << skillName << "] on "
                  << hero.getName() << " for " << effectiveDmg << " damage!"
                  << (isDefending ? " (Guarded!)" : "") << "\n";
    } else {
        int rawAtk = enemy.getAttack();
        int effectiveDmg = calculateDamage(rawAtk, heroDef);
        if (isDefending) {
            effectiveDmg = std::max(1, effectiveDmg / 2);
        }
        hero.setHp(hero.getHp() - effectiveDmg);
        std::cout << enemy.getName() << " attacks " << hero.getName()
                  << " for " << effectiveDmg << " damage!"
                  << (isDefending ? " (Guarded!)" : "") << "\n";
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

const std::vector<std::shared_ptr<Item>>& CombatEngine::getLastDroppedItems() const {
    return lastDroppedItems;
}

