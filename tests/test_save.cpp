#include "SaveManager.h"
#include <cassert>
#include <iostream>

int main() {
    SaveManager manager("saves_test/");
    Hero hero("SavedHero", HeroClass::WARRIOR, 100, 20, 10, 5);
    hero.setHp(85);
    hero.setMp(15);
    hero.setLevel(3);
    hero.setExp(120);

    StoryGraph story;

    // Test saving
    assert(manager.saveGame(99, hero, story) == true);
    assert(manager.slotExists(99) == true);

    // Test loading
    Hero loadedHero("Blank", HeroClass::MAGE, 50, 50, 5, 2);
    StoryGraph loadedStory;
    assert(manager.loadGame(99, loadedHero, loadedStory) == true);

    // Validate attributes restored correctly from JSON
    assert(loadedHero.getHp() == 85);
    assert(loadedHero.getMp() == 15);
    assert(loadedHero.getLevel() == 3);
    assert(loadedHero.getExp() == 120);

    // Test deleting slot
    assert(manager.deleteSlot(99) == true);
    assert(manager.slotExists(99) == false);

    std::cout << "[PASS] SaveManager & JSON serialization unit tests successful!\n";
    return 0;
}
