/**
 * @file main.cpp
 * @brief Entry point for RPG Engine, combining all system modules.
 * @author Quang
 */

#include "Hero.h"
#include "Enemy.h"
#include "Minion.h"
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

    // 4. Initialize Minion & Combat Engine from JSON (Nhật & Lợi)
    std::cout << "\n[Loading Minions from data/enemies.json]\n";
    auto loadedMinions = MinionFactory::loadAllFromJson("data/enemies.json");
    for (const auto& m : loadedMinions) {
        std::cout << " - Loaded: " << m->getName() << " [" << minionTypeToString(m->getMinionType())
                  << "] (HP: " << m->getHp() << ", ATK: " << m->getAttack()
                  << ", DEF: " << m->getDefense() << ")\n";
    }

    auto minion = MinionFactory::createFromJson("goblin", "data/enemies.json");
    if (!minion) {
        minion = std::make_shared<Goblin>();
    }
    std::cout << "\nEncountered an enemy:\n";
    minion->displayStats();

    CombatEngine combat(player, *minion);
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
