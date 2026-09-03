/**
 * @file SaveManager.cpp
 * @brief Implement SaveManager class methods.
 * @author Phong
 */

#include "SaveManager.h"
#include <iostream>
#include <fstream>

SaveManager::SaveManager(const std::string& saveDir) : saveDirectory(saveDir) {}

bool SaveManager::saveGame(int slot, const Hero& hero, const StoryGraph& story) {
    std::cout << "[SaveManager] Saving game state to slot " << slot << "...\n";
    // Serialization placeholder
    return true;
}

bool SaveManager::loadGame(int slot, Hero& hero, StoryGraph& story) {
    std::cout << "[SaveManager] Loading game state from slot " << slot << "...\n";
    // Deserialization placeholder
    return true;
}

bool SaveManager::slotExists(int slot) const {
    return false;
}

bool SaveManager::deleteSlot(int slot) {
    std::cout << "[SaveManager] Slot " << slot << " deleted.\n";
    return true;
}
