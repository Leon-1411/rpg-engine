/**
 * @file main.cpp
 * @brief Entry point for RPG Engine, combining all system modules.
 * @author Quang & Antigravity
 */

#include "Hero.h"
#include "Enemy.h"
#include "Item.h"
#include "Inventory.h"
#include "CombatEngine.h"
#include "StoryGraph.h"
#include "SaveManager.h"
#include <iostream>

int main() {
    std::cout << "========================================\n";
    std::cout << "         RPG ENGINE CONSOLE GAME        \n";
    std::cout << "========================================\n\n";

    // 1. Initialize Hero (Quý)
    Hero player("Arthur", HeroClass::WARRIOR);
    player.displayStats();

    // 2. Initialize Inventory & Items (Sang)
    Inventory inv;
    Item sword("wpn_01", "Iron Sword", "Sharp blade", ItemType::WEAPON, 10);
    Item armor("arm_01", "Iron Armor", "Sturdy armor", ItemType::ARMOR, 8);
    inv.addItem(sword);
    inv.addItem(armor);
    inv.listItems();

    // 3. Initialize Story Graph (Nghĩa)
    StoryGraph story;
    StoryNode current = story.getCurrentNode();
    std::cout << "\n[Story Node: " << current.id << "]\n" << current.text << "\n";

    // 4. Initialize Enemy & Combat Engine (Nhật & Lợi)
    Enemy minion("Goblin Scout", EnemyType::MINION, 40, 12, 3, 50, 15);
    minion.displayStats();

    CombatEngine combat(player, minion, &inv);
    combat.startBattle();
    
    // Display visual battle status
    combat.displayBattleStatus();

    // Turn 1: Hero takes Defend stance (halves incoming enemy damage)
    std::cout << "\n>>> Simulating Turn 1: Warrior Defends (Halves incoming damage) <<<\n";
    combat.executeTurn(4);

    // Turn 2: Warrior executes devastating Skill 1 (Sword Slash)
    if (!combat.isBattleOver()) {
        std::cout << "\n>>> Simulating Turn 2: Warrior uses Skill 1 (" << player.getSkillName(1) << ") <<<\n";
        combat.executeTurn(2, 1);
    }

    // Turn 3: Warrior finishes with Normal Attack if needed
    if (!combat.isBattleOver()) {
        std::cout << "\n>>> Simulating Turn 3: Warrior finishes with Normal Attack <<<\n";
        combat.executeTurn(1);
    }

    // Demonstrate Mage's exclusive Potion skillset (Skill 2: Healing Potion & Skill 3: Poison Flask)
    std::cout << "\n=======================================================\n";
    std::cout << "   DEMO: MAGE'S EXCLUSIVE POTION SKILLSET             \n";
    std::cout << "=======================================================\n";
    Hero mage("Merlin", HeroClass::MAGE);
    mage.displayStats();

    Enemy orc("Orc Brute", EnemyType::MINION, 60, 14, 4, 60, 20);
    CombatEngine mageCombat(mage, orc);
    mageCombat.startBattle();

    // Mage uses Skill 3: Poison Flask (Toxic Brew)
    std::cout << "\n>>> Mage uses Skill 3: " << mage.getSkillName(3) << " (Inflicts Poison DoT) <<<\n";
    mageCombat.executeTurn(2, 3);

    // Mage uses Skill 2: Healing Potion (Alchemical Brew)
    std::cout << "\n>>> Mage uses Skill 2: " << mage.getSkillName(2) << " (Restores HP + Gains Regen) <<<\n";
    mageCombat.executeTurn(2, 2);

    // Demonstrate Poisonous Enemy (Quái hệ độc) inflicting poison on Hero
    std::cout << "\n=======================================================\n";
    std::cout << "   DEMO: POISONOUS ENEMY (QUÁI HỆ ĐỘC) ENCOUNTER       \n";
    std::cout << "=======================================================\n";
    Hero ranger("Robin", HeroClass::RANGER);
    Enemy venomSnake("Venom Cobra", EnemyType::MINION, 50, 12, 2, 40, 15);
    venomSnake.setPoisonous(true, 3, 6); // Inflicts 6 poison dmg for 3 turns
    CombatEngine venomCombat(ranger, venomSnake);
    venomCombat.startBattle();

    std::cout << "\n>>> Turn 1: Ranger attacks, Venom Cobra strikes and infects Ranger with poison <<<\n";
    venomCombat.executeTurn(1);

    std::cout << "\n>>> Turn 2: Ranger suffers Poison DoT at turn start <<<\n";
    venomCombat.executeTurn(1);

    // 5. Save Manager Test (Phong)
    SaveManager saveMgr;
    saveMgr.saveGame(1, player, story);

    std::cout << "\n========================================\n";
    std::cout << "   Engine initialized & tested cleanly  \n";
    std::cout << "========================================\n";

    return 0;
}
