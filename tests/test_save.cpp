#include "SaveManager.h"
#include <cassert>
#include <iostream>

int main() {
    SaveManager manager;
    Hero hero("SavedHero", HeroClass::WARRIOR, 100, 20, 10, 5);
    StoryGraph story;

    assert(manager.saveGame(1, hero, story) == true);
    assert(manager.loadGame(1, hero, story) == true);

    std::cout << "[PASS] SaveManager unit tests successful!\n";
    return 0;
}
