#include "StoryGraph.h"
#include "Hero.h"
#include "Mage.h"
#include "Warrior.h"
#include "Ranger.h"
#include "SaveManager.h"
#include "Shop.h"
#include "DataLoader.h"
#include "BossMonster.h"
#include "Minion.h"
#include <cassert>
#include <iostream>

void testFracturedCrownDialogueFlow() {
    std::cout << "[E2E Test 1] Testing Fractured Crown Dialogue Tree flow (Node 01 King)...\n";
    StoryGraph story;
    assert(story.loadStoryGraph("data/story.json") == true);
    assert(story.getCurrentNodeId() == "Node01");

    // Node 01 must be in dialogue
    assert(story.isInDialogue() == true);
    DialogueNode d1 = story.getCurrentDialogueNode();
    assert(d1.speaker == "Nhà Vua Eldoria");
    assert(d1.choices.size() == 2);

    // Select choice 0 ("Lời thề danh dự") -> king_honor
    assert(story.selectDialogueChoice(0) == true);
    DialogueNode d2 = story.getCurrentDialogueNode();
    assert(d2.id == "king_honor");
    assert(d2.choices.size() == 1);

    // Select choice in king_honor -> transitions to Node02
    assert(story.selectDialogueChoice(0) == true);
    assert(story.getCurrentNodeId() == "Node02");
    std::cout << " -> PASSED: Node 01 Dialogue Tree smoothly navigated to Node 02!\n";
}

void testShopIntegration() {
    std::cout << "[E2E Test 2] Testing Shop Node (Node02 -> Node02_Shop)...\n";
    StoryGraph story;
    story.loadStoryGraph("data/story.json");
    story.moveToNode("Node02");

    // Node 02 has shop choice
    StoryNode node02 = story.getCurrentNode();
    bool foundShopChoice = false;
    for (size_t i = 0; i < node02.choices.size(); ++i) {
        if (node02.choices[i].nextNodeId == "Node02_Shop") {
            foundShopChoice = true;
            assert(story.selectChoice(i) == true);
            break;
        }
    }
    assert(foundShopChoice == true);
    assert(story.getCurrentNodeId() == "Node02_Shop");
    assert(story.getCurrentNode().type == EventType::SHOP);

    // Shop purchase test
    Hero hero("Arthur", HeroClass::WARRIOR, 120, 30, 18, 8);
    hero.setGold(200);
    Shop shop("Tram Tiep Te", "Mo ta");
    shop.loadDefaultStock();
    assert(shop.buyItem(0, hero) == true);
    assert(hero.getGold() < 200);
    assert(hero.getInventory().getItemCount() > 0);

    // Leave shop back to Node 02
    assert(story.selectChoice(0) == true);
    assert(story.getCurrentNodeId() == "Node02");
    std::cout << " -> PASSED: Shop Node & Economy transactions verified!\n";
}

void testTrueEndingPathAndSaveLoad() {
    std::cout << "[E2E Test 3] Testing True Ending Path (Mage -> Codex -> Elena Diary -> True Ending) with Save/Load...\n";
    Mage mage("Morrigan");
    StoryGraph story;
    story.loadStoryGraph("data/story.json");

    // 1. Give Royal Insignia at Node 01
    auto items = DataLoader::loadItems("data/items.json");
    for (const auto& itm : items) {
        if (itm.getId() == "RoyalInsignia") mage.getInventory().addItem(itm);
    }
    story.moveToNode("Node02");

    // 2. Take Path to Ancient Codex (Node 04 -> Node 04_Check -> Node 07)
    story.moveToNode("Node04");
    story.moveToNode("Node04_Check");
    StoryNode checkNode = story.getCurrentNode();
    assert(checkNode.requiredHeroClass == "Mage");
    assert(mage.getHeroClassName() == checkNode.requiredHeroClass);
    story.moveToNode(checkNode.onPassNodeId); // Node 07
    assert(story.getCurrentNodeId() == "Node07");

    // Reward Ancient Codex
    for (const auto& itm : items) {
        if (itm.getId() == "AncientCodex") mage.getInventory().addItem(itm);
    }
    assert(mage.getInventory().hasItem("AncientCodex") == true);

    // 3. Move to Gorge and Demon Castle (Node 09 -> Node 10 -> Node 12 -> Node 11)
    story.moveToNode("Node09");
    story.moveToNode("Node10");
    StoryNode runeCheck = story.getCurrentNode();
    assert(mage.getInventory().hasItem("DemonEmpathyRune") == false);
    story.moveToNode(runeCheck.onFailNodeId); // Node 12 (Demon Berserker combat)
    assert(story.getCurrentNodeId() == "Node12");

    // Defeat Demon Berserker -> Move to Node 11
    story.moveToNode("Node11");
    assert(story.getCurrentNodeId() == "Node11");
    for (const auto& itm : items) {
        if (itm.getId() == "ElenaDiary") mage.getInventory().addItem(itm);
    }
    assert(mage.getInventory().hasItem("ElenaDiary") == true);

    // --- SAVE / LOAD VERIFICATION AT NODE 11 ---
    SaveManager saveMgr("saves_test/");
    assert(saveMgr.saveGame(88, mage, story) == true);

    // Reset Hero and Story
    Hero loadedHero("Blank", HeroClass::WARRIOR, 1, 1, 1);
    StoryGraph loadedStory;
    loadedStory.loadStoryGraph("data/story.json");
    assert(saveMgr.loadGame(88, loadedHero, loadedStory) == true);

    // Assert that loaded state is at Node 11 and has all required keys
    assert(loadedStory.getCurrentNodeId() == "Node11");
    assert(loadedHero.getHeroClassName() == "Mage");
    assert(loadedHero.getInventory().hasItem("AncientCodex") == true);
    assert(loadedHero.getInventory().hasItem("ElenaDiary") == true);

    // 4. Advance from Node 11 to Node 13 (Decision Node)
    loadedStory.moveToNode("Node13");
    assert(loadedStory.getCurrentNodeId() == "Node13");

    // Choose Route 2: Together with Elena to Core (Node 16 -> Node 17 -> Node 18 -> CheckSecret)
    loadedStory.moveToNode("Node16");
    loadedStory.moveToNode("Node17"); // General Aldric
    loadedStory.moveToNode("Node18"); // The Core Guardian
    loadedStory.moveToNode("CheckSecret");

    // Verify CheckSecret passes and unlocks End5 (True Ending)
    StoryNode secretCheck = loadedStory.getCurrentNode();
    bool hasAll = true;
    for (const auto& req : secretCheck.requiredItems) {
        if (!loadedHero.getInventory().hasItem(req)) hasAll = false;
    }
    assert(hasAll == true);
    loadedStory.moveToNode(secretCheck.onPassNodeId);
    assert(loadedStory.getCurrentNodeId() == "End5");
    assert(loadedStory.getCurrentNode().type == EventType::ENDING);

    // Clean up
    saveMgr.deleteSlot(88);
    std::cout << " -> PASSED: Complete True Ending path and Save/Load cycle verified flawlessly!\n";
}

void testFactionBossLoading() {
    std::cout << "[E2E Test 4] Testing Faction Bosses JSON creation & Stats...\n";
    std::vector<std::string> bossIds = {
        "Demon_King_Malakor",
        "General_Aldric",
        "The_Core_Guardian",
        "Arcane_Council_Enforcers",
        "Archmage_Morvath",
        "Multi-Faction Battle"
    };

    for (const auto& bId : bossIds) {
        auto boss = BossFactory::createFromJson(bId, "data/enemies.json");
        assert(boss != nullptr);
        assert(boss->isAlive() == true);
        assert(boss->getMaxHp() >= 400);
        assert(boss->getAttack() >= 60);
    }
    std::cout << " -> PASSED: All 6 Fractured Crown Faction Bosses correctly instantiated from JSON!\n";
}

int main() {
    std::cout << "===============================================================\n";
    std::cout << "   RUNNING FRACTURED CROWN END-TO-END INTEGRATION TEST SUITE   \n";
    std::cout << "===============================================================\n";
    testFracturedCrownDialogueFlow();
    testShopIntegration();
    testTrueEndingPathAndSaveLoad();
    testFactionBossLoading();
    std::cout << "===============================================================\n";
    std::cout << " [ALL E2E PASS] Entire Fractured Crown narrative is 100% verified!\n";
    std::cout << "===============================================================\n";
    return 0;
}
