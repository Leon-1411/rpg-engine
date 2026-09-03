/**
 * @file main.cpp
 * @brief Entry point for RPG Engine, combining all system modules.
 * @author Quang
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
    Hero player("Arthur", HeroClass::WARRIOR, 100, 30, 20, 5);
    player.displayStats();

    // 2. Initialize Inventory & Items (Sang)
    Inventory inv;
    Item potion("pot_01", "Health Potion", "Restores 30 HP", ItemType::POTION, 30);
    Item sword("wpn_01", "Iron Sword", "Sharp blade", ItemType::WEAPON, 10);
    inv.addItem(potion);
    inv.addItem(sword);
    inv.listItems();

    // 3. Initialize Story Graph (Nghĩa)
    StoryGraph story;
    StoryNode current = story.getCurrentNode();
    std::cout << "\n[Story Node: " << current.id << "]\n" << current.text << "\n";

    // 4. Initialize Enemy & Combat Engine (Nhật & Lợi)
    Enemy minion("Goblin Scout", EnemyType::MINION, 40, 12, 3, 50, 15);
    minion.displayStats();

    CombatEngine combat(player, minion);
    combat.startBattle();
    
    // Simulate a battle turn
    combat.executeTurn(1); // Hero attacks
    if (!combat.isBattleOver()) {
        combat.executeTurn(1); // Hero attacks again
    }

    // 5. Save Manager Test (Phong)
    SaveManager saveMgr;
    saveMgr.saveGame(1, player, story);

    std::cout << "\n========================================\n";
    std::cout << "   Engine initialized & tested cleanly  \n";
    std::cout << "========================================\n";

    return 0;
}
