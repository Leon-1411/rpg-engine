#include "Item.h"
#include <cassert>
#include <iostream>

int main() {
    Item item("p01", "Small Potion", "Restores 20 HP", ItemType::POTION, 20);

    assert(item.getId() == "p01");
    assert(item.getName() == "Small Potion");
    assert(item.getType() == ItemType::POTION);
    assert(item.getStatValue() == 20);

    std::cout << "[PASS] Item unit tests successful!\n";
    return 0;
}
