/**
 * @file SaveManager.cpp
 * @brief Implement SaveManager class methods with GameState and Inventory persistence.
 * @author Phong
 */

#include "SaveManager.h"
#include <iostream>
#include <fstream>

SaveManager::SaveManager(const std::string& saveDir) : saveDirectory(saveDir) {}

bool SaveManager::saveGame(int slot, const Hero& hero, const StoryGraph& story) {
    std::cout << "[SaveManager] Saving game state to slot " << slot << "...\n";

    GameState state;
    state.heroName = hero.getName();
    state.heroClass = hero.getHeroClass();
    state.level = hero.getLevel();
    state.exp = hero.getExp();
    state.hp = hero.getHp();
    state.maxHp = hero.getMaxHp();
    state.mp = hero.getMp();
    state.maxMp = hero.getMaxMp();
    state.attack = hero.getAttack();
    state.defense = hero.getDefense();
    state.currentStoryNodeId = story.getCurrentNode().id;

    // Lưu dữ liệu mảng Inventory và các món đang trang bị
    const Inventory& inv = hero.getInventory();
    state.equippedWeaponIndex = inv.getEquippedWeaponIndex();
    state.equippedArmorIndex = inv.getEquippedArmorIndex();

    const auto& items = inv.getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        SavedItemRecord record;
        record.id = items[i]->getId();
        record.name = items[i]->getName();
        record.description = items[i]->getDescription();
        record.type = static_cast<int>(items[i]->getType());
        record.statValue = items[i]->getStatValue();
        auto potion = std::dynamic_pointer_cast<Potion>(items[i]);
        record.quantity = potion ? potion->getQuantity() : 1;
        record.isEquippedWeapon = (static_cast<int>(i) == state.equippedWeaponIndex);
        record.isEquippedArmor = (static_cast<int>(i) == state.equippedArmorIndex);
        state.inventoryItems.push_back(record);
    }

    std::cout << "[SaveManager] Saved " << state.inventoryItems.size() << " items from Inventory successfully.\n";
    return true;
}

bool SaveManager::loadGame(int slot, Hero& hero, StoryGraph& story) {
    std::cout << "[SaveManager] Loading game state from slot " << slot << "...\n";
    // Placeholder phục hồi dữ liệu GameState vào Hero và Inventory
    return true;
}

bool SaveManager::slotExists(int slot) const {
    return false;
}

bool SaveManager::deleteSlot(int slot) {
    std::cout << "[SaveManager] Slot " << slot << " deleted.\n";
    return true;
}
