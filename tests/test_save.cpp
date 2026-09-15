#include "SaveManager.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>

void testBasicSaveAndLoad() {
    std::cout << "[Test 1] Basic Save, Load, and Slot Deletion...\n";
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
    assert(loadedHero.getName() == "SavedHero");
    assert(loadedHero.getHeroClass() == HeroClass::WARRIOR);
    assert(loadedHero.getHp() == 85);
    assert(loadedHero.getMp() == 15);
    assert(loadedHero.getLevel() == 3);
    assert(loadedHero.getExp() == 120);

    // Test deleting slot
    assert(manager.deleteSlot(99) == true);
    assert(manager.slotExists(99) == false);
    std::cout << " -> PASSED\n";
}

void testFullHeroAttributesAndStatus() {
    std::cout << "[Test 2] Full Hero Stats, Gold, ReadyArrows, and Status Effects...\n";
    SaveManager manager("saves_test/");
    Hero hero("Robin", HeroClass::RANGER, 120, 60, 25, 12, 8, 0.35f, 0.60f, false);
    hero.setLevel(5);
    hero.setExp(450);
    hero.setHp(95);
    hero.setMp(40);
    hero.setReadyArrows(7);
    hero.setGold(350);
    hero.applyPoison(3, 8);
    hero.applyRegen(4, 10);
    hero.setSkillCooldowns({1, 0, 3});

    StoryGraph story;
    assert(manager.saveGame(5, hero, story) == true);
    assert(manager.slotExists(5) == true);

    Hero loadedHero("Blank", HeroClass::WARRIOR, 50, 10, 5);
    StoryGraph loadedStory;
    assert(manager.loadGame(5, loadedHero, loadedStory) == true);

    assert(loadedHero.getName() == "Robin");
    assert(loadedHero.getHeroClass() == HeroClass::RANGER);
    assert(loadedHero.getLevel() == 5);
    assert(loadedHero.getExp() == 450);
    assert(loadedHero.getHp() == 95);
    assert(loadedHero.getMaxHp() == 120);
    assert(loadedHero.getMp() == 40);
    assert(loadedHero.getMaxMp() == 60);
    assert(loadedHero.getAttack() == 25);
    assert(loadedHero.getDefense() == 12);
    assert(loadedHero.getArmorPenetration() == 8);
    assert(loadedHero.getCritChance() >= 0.34f && loadedHero.getCritChance() <= 0.36f);
    assert(loadedHero.getCritDamage() >= 0.59f && loadedHero.getCritDamage() <= 0.61f);
    assert(loadedHero.getReadyArrows() == 7);
    assert(loadedHero.getGold() == 350);

    // Status effects
    assert(loadedHero.isPoisoned() == true);
    assert(loadedHero.getPoisonTurns() == 3);
    assert(loadedHero.getPoisonDamagePerTurn() == 8);
    assert(loadedHero.hasRegen() == true);
    assert(loadedHero.getRegenTurns() == 4);
    assert(loadedHero.getRegenPerTurn() == 10);

    // Skill cooldowns
    assert(loadedHero.getSkillCooldowns().size() == 3);
    assert(loadedHero.getSkillCooldowns()[0] == 1);
    assert(loadedHero.getSkillCooldowns()[1] == 0);
    assert(loadedHero.getSkillCooldowns()[2] == 3);

    assert(manager.deleteSlot(5) == true);
    std::cout << " -> PASSED\n";
}

void testInventoryPersistence() {
    std::cout << "[Test 3] Inventory Items, Potions (Health & Mana), and Equipment...\n";
    SaveManager manager("saves_test/");
    Hero hero("Arthur", HeroClass::WARRIOR, 150, 50, 20, 15);
    
    Inventory& inv = hero.getInventory();
    inv.addItem(std::make_shared<Weapon>("excalibur", "Excalibur", "Holy blade", 35));
    inv.addItem(std::make_shared<Armor>("dragon_mail", "Dragon Mail", "Dragon scale armor", 20));
    inv.addItem(std::make_shared<Potion>("hp_potion", "Health Potion", "Restores 50 HP", 50, false, 5));
    inv.addItem(std::make_shared<Potion>("mp_potion", "Mana Potion", "Restores 40 MP", 40, true, 3));

    // Equip weapon (index 0) and armor (index 1)
    assert(inv.equipWeapon(0) == true);
    assert(inv.equipArmor(1) == true);
    assert(hero.getEffectiveAttack() == 20 + 35);
    assert(hero.getEffectiveDefense() == 15 + 20);

    StoryGraph story;
    assert(manager.saveGame(1, hero, story) == true);

    Hero loadedHero("Dummy", HeroClass::WARRIOR, 100, 10, 5);
    StoryGraph loadedStory;
    assert(manager.loadGame(1, loadedHero, loadedStory) == true);

    Inventory& loadedInv = loadedHero.getInventory();
    assert(loadedInv.getItemCount() == 4);

    // Check Weapon
    auto weapon = std::dynamic_pointer_cast<Weapon>(loadedInv.getItemPtr(0));
    assert(weapon != nullptr);
    assert(weapon->getId() == "excalibur");
    assert(weapon->getAttackBonus() == 35);

    // Check Armor
    auto armor = std::dynamic_pointer_cast<Armor>(loadedInv.getItemPtr(1));
    assert(armor != nullptr);
    assert(armor->getId() == "dragon_mail");
    assert(armor->getDefenseBonus() == 20);

    // Check Health Potion
    auto hpPotion = std::dynamic_pointer_cast<Potion>(loadedInv.getItemPtr(2));
    assert(hpPotion != nullptr);
    assert(hpPotion->getId() == "hp_potion");
    assert(hpPotion->isMana() == false);
    assert(hpPotion->getQuantity() == 5);
    assert(hpPotion->getHealAmount() == 50);

    // Check Mana Potion
    auto mpPotion = std::dynamic_pointer_cast<Potion>(loadedInv.getItemPtr(3));
    assert(mpPotion != nullptr);
    assert(mpPotion->getId() == "mp_potion");
    assert(mpPotion->isMana() == true);
    assert(mpPotion->getQuantity() == 3);
    assert(mpPotion->getHealAmount() == 40);

    // Check Equipment Indices and Effective stats
    assert(loadedInv.getEquippedWeaponIndex() == 0);
    assert(loadedInv.getEquippedArmorIndex() == 1);
    assert(loadedHero.getEffectiveAttack() == 20 + 35);
    assert(loadedHero.getEffectiveDefense() == 15 + 20);

    assert(manager.deleteSlot(1) == true);
    std::cout << " -> PASSED\n";
}

void testStoryGraphProgressAndFlags() {
    std::cout << "[Test 4] StoryGraph Node ID and Story Flags Persistence...\n";
    SaveManager manager("saves_test/");
    Hero hero("Hero", HeroClass::WARRIOR);
    StoryGraph story;

    story.setCurrentNodeId("temple_altar_02");
    story.setFlag("has_temple_key", true);
    story.setFlag("talked_to_guardian", true);
    story.setFlag("boss_defeated", false);

    assert(manager.saveGame(2, hero, story) == true);

    Hero loadedHero("Dummy", HeroClass::WARRIOR);
    StoryGraph loadedStory;
    assert(manager.loadGame(2, loadedHero, loadedStory) == true);

    assert(loadedStory.getCurrentNodeId() == "temple_altar_02");
    assert(loadedStory.getFlag("has_temple_key") == true);
    assert(loadedStory.getFlag("talked_to_guardian") == true);
    assert(loadedStory.getFlag("boss_defeated") == false);
    assert(loadedStory.getFlag("non_existent_flag") == false);

    assert(manager.deleteSlot(2) == true);
    std::cout << " -> PASSED\n";
}

void testMultiSlotManagement() {
    std::cout << "[Test 5] Multi-Slot Enumeration and Previews...\n";
    SaveManager manager("saves_test/");
    Hero h1("Warrior1", HeroClass::WARRIOR, 100, 20, 10);
    Hero h2("Mage2", HeroClass::MAGE, 80, 30, 5);
    Hero h3("Ranger3", HeroClass::RANGER, 90, 25, 8);
    Hero h4("Hero99", HeroClass::WARRIOR, 150, 40, 20);
    StoryGraph story;

    assert(manager.saveGame(1, h1, story) == true);
    assert(manager.saveGame(2, h2, story) == true);
    assert(manager.saveGame(10, h3, story) == true);
    assert(manager.saveGame(99, h4, story) == true);

    std::vector<int> existing = manager.getExistingSlots();
    assert(std::find(existing.begin(), existing.end(), 1) != existing.end());
    assert(std::find(existing.begin(), existing.end(), 2) != existing.end());
    assert(std::find(existing.begin(), existing.end(), 10) != existing.end());
    assert(std::find(existing.begin(), existing.end(), 99) != existing.end());
    assert(std::find(existing.begin(), existing.end(), 42) == existing.end());

    // Slot preview
    SavedGameState preview2;
    assert(manager.getSlotPreview(2, preview2) == true);
    assert(preview2.heroName == "Mage2");
    assert(preview2.heroClass == HeroClass::MAGE);

    SavedGameState preview10;
    assert(manager.getSlotPreview(10, preview10) == true);
    assert(preview10.heroName == "Ranger3");
    assert(preview10.heroClass == HeroClass::RANGER);

    // Clean up
    assert(manager.deleteSlot(1) == true);
    assert(manager.deleteSlot(2) == true);
    assert(manager.deleteSlot(10) == true);
    assert(manager.deleteSlot(99) == true);
    assert(manager.slotExists(1) == false);
    assert(manager.slotExists(2) == false);
    assert(manager.slotExists(10) == false);
    assert(manager.slotExists(99) == false);
    std::cout << " -> PASSED\n";
}

void testCorruptedFileAndErrorHandling() {
    std::cout << "[Test 6] Corrupted File and Error Handling...\n";
    SaveManager manager("saves_test/");

    // Test non-existent slot
    Hero hero("Hero", HeroClass::WARRIOR);
    StoryGraph story;
    assert(manager.loadGame(777, hero, story) == false);
    assert(manager.deleteSlot(777) == false);

    // Create a corrupted JSON file
    std::string badFilePath = manager.getSlotFilePath(13);
    std::ofstream badFile(badFilePath);
    badFile << "{ this is not a valid json syntax !!!";
    badFile.close();

    assert(manager.slotExists(13) == true);
    assert(manager.isSlotCorrupted(13) == true);

    // Loading corrupted slot must fail gracefully without crash
    assert(manager.loadGame(13, hero, story) == false);

    // Delete corrupted slot
    assert(manager.deleteSlot(13) == true);
    assert(manager.slotExists(13) == false);
    std::cout << " -> PASSED\n";
}

void testBackwardCompatibilityWithLegacySave() {
    std::cout << "[Test 7] Backward Compatibility with Legacy Save JSON...\n";
    SaveManager manager("saves_test/");
    std::string legacyJson = R"({
        "heroName": "LegacyHero",
        "heroClass": 1,
        "level": 4,
        "exp": 280,
        "hp": 75,
        "maxHp": 90,
        "mp": 80,
        "maxMp": 110,
        "attack": 22,
        "defense": 6,
        "currentStoryNodeId": "ancient_library",
        "equippedWeaponIndex": 0,
        "equippedArmorIndex": -1,
        "inventory": [
            {
                "id": "apprentice_staff",
                "name": "Apprentice Staff",
                "description": "A wooden staff",
                "type": 0,
                "statValue": 8,
                "quantity": 1,
                "isEquippedWeapon": true,
                "isEquippedArmor": false
            }
        ]
    })";

    std::string filePath = manager.getSlotFilePath(7);
    std::ofstream f(filePath);
    f << legacyJson;
    f.close();

    Hero loadedHero("Dummy", HeroClass::WARRIOR);
    StoryGraph loadedStory;
    assert(manager.loadGame(7, loadedHero, loadedStory) == true);

    assert(loadedHero.getName() == "LegacyHero");
    assert(loadedHero.getHeroClass() == HeroClass::MAGE);
    assert(loadedHero.getLevel() == 4);
    assert(loadedHero.getExp() == 280);
    assert(loadedHero.getHp() == 75);
    assert(loadedHero.getMaxHp() == 90);
    assert(loadedHero.getMp() == 80);
    assert(loadedHero.getMaxMp() == 110);
    assert(loadedHero.getAttack() == 22);
    assert(loadedHero.getDefense() == 6);
    assert(loadedStory.getCurrentNodeId() == "ancient_library");
    assert(loadedHero.getInventory().getItemCount() == 1);
    assert(loadedHero.getInventory().getEquippedWeaponIndex() == 0);
    assert(loadedHero.getEffectiveAttack() == 22 + 8);

    assert(manager.deleteSlot(7) == true);
    std::cout << " -> PASSED\n";
}

void testSnapshotDirectAPI() {
    std::cout << "[Test 8] Direct Snapshot Create and Apply API...\n";
    SaveManager manager("saves_test/");
    Hero hero("Gandalf", HeroClass::MAGE, 100, 150, 30, 8);
    hero.setLevel(10);
    hero.setGold(999);
    hero.getInventory().addItem(std::make_shared<Weapon>("staff_power", "Staff of Power", "Legendary", 50));
    hero.getInventory().equipWeapon(0);

    StoryGraph story;
    story.setCurrentNodeId("tower_peak");
    story.setFlag("ring_destroyed", true);

    // Create snapshot
    SavedGameState snapshot = manager.createSnapshot(hero, story);
    assert(snapshot.heroName == "Gandalf");
    assert(snapshot.heroClass == HeroClass::MAGE);
    assert(snapshot.level == 10);
    assert(snapshot.gold == 999);
    assert(snapshot.currentStoryNodeId == "tower_peak");
    assert(snapshot.storyFlags["ring_destroyed"] == true);
    assert(snapshot.inventoryItems.size() == 1);
    assert(snapshot.equippedWeaponIndex == 0);

    // Save and load GameState directly
    assert(manager.saveGameState(8, snapshot) == true);
    SavedGameState loadedSnapshot;
    assert(manager.loadGameState(8, loadedSnapshot) == true);
    assert(loadedSnapshot.heroName == "Gandalf");
    assert(loadedSnapshot.gold == 999);

    // Apply to blank hero
    Hero blankHero("Blank", HeroClass::WARRIOR);
    StoryGraph blankStory;
    assert(manager.applySnapshot(loadedSnapshot, blankHero, blankStory) == true);
    assert(blankHero.getName() == "Gandalf");
    assert(blankHero.getHeroClass() == HeroClass::MAGE);
    assert(blankHero.getGold() == 999);
    assert(blankHero.getEffectiveAttack() == 30 + 50);
    assert(blankStory.getCurrentNodeId() == "tower_peak");

    assert(manager.deleteSlot(8) == true);
    std::cout << " -> PASSED\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << " RUNNING SAVEMANAGER COMPREHENSIVE TESTS\n";
    std::cout << "========================================\n";

    testBasicSaveAndLoad();
    testFullHeroAttributesAndStatus();
    testInventoryPersistence();
    testStoryGraphProgressAndFlags();
    testMultiSlotManagement();
    testCorruptedFileAndErrorHandling();
    testBackwardCompatibilityWithLegacySave();
    testSnapshotDirectAPI();

    std::cout << "========================================\n";
    std::cout << " [ALL PASS] SaveManager unit tests successful!\n";
    std::cout << "========================================\n";
    return 0;
}
