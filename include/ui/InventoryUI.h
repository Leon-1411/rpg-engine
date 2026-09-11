#pragma once

#include "Inventory.h"
#include <string>

class InventoryUI {
public:
    InventoryUI() = default;
    ~InventoryUI() = default;

    // Renders the inventory items in a formatted table with ANSI styling
    void renderInventory(const Inventory& inventory);

    // Prompts player to select an item index (-1 if back)
    int selectItemIndex(const Inventory& inventory);

    // Prompts action for selected item (1: Use, 2: Equip, 3: Discard, 0: Back)
    int selectItemAction(const Item& item);
};
