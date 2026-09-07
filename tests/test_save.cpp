#include "SaveManager.h"
#include <cassert>
#include <iostream>

int main() {
    SaveManager manager;
    Hero hero("SavedHero", HeroClass::WARRIOR);
    StoryGraph story;

    assert(manager.saveGame(1, hero, story) == true);
    assert(manager.loadGame(1, hero, story) == true);

    std::cout << "[PASS] SaveManager unit tests successful!\n";
    return 0;
}
