#include "Shop.h"
#include "Warrior.h"
#include "CombatEngine.h"
#include "StoryGraph.h"
#include "Enemy.h"
#include <cassert>
#include <iostream>
#include <memory>

int main() {
    std::cout << "=======================================\n";
    std::cout << "  RUNNING SHOP & GOLD ECONOMY TESTS\n";
    std::cout << "=======================================\n";

    // 1. Test Shop creation and adding stock
    Shop merchant("Eldoria Wandering Merchant");
    auto hpPot = std::make_shared<Item>("item_hp_pot_s", "Binh Mau Nho", "Hoi phuc mau", ItemType::POTION, 50);
    auto ironSword = std::make_shared<Item>("item_iron_sword", "Kiem Sat Eldoria", "Tang sat thuong", ItemType::WEAPON, 15);
    auto ironShield = std::make_shared<Item>("item_iron_shield", "Khien Sat Eldoria", "Tang phong thu", ItemType::ARMOR, 8);

    merchant.addItem(hpPot, 50, 25, 5);
    merchant.addItem(ironSword, 150, 75, 1);
    merchant.addItem(ironShield, 120, 60, 1);

    assert(merchant.getGoods().size() == 3);
    assert(merchant.getGoods()[0].item->getId() == "item_hp_pot_s");
    assert(merchant.getGoods()[0].stock == 5);
    std::cout << "[Test 1] Shop creation & Stock management: PASSED\n";

    // 2. Test Purchasing item with sufficient gold vs insufficient gold
    Warrior hero("Arthur");
    hero.setGold(100);

    // Fail purchase: item index 1 (ironSword) costs 150, hero has 100
    bool buyFail = merchant.buyItem(1, hero);
    assert(!buyFail);
    assert(hero.getGold() == 100);
    assert(!hero.getInventory().hasItem("item_iron_sword"));
    assert(merchant.getGoods()[1].stock == 1);

    // Success purchase: item index 0 (hpPot) costs 50, hero has 100 -> gold becomes 50
    bool buySuccess = merchant.buyItem(0, hero);
    assert(buySuccess);
    assert(hero.getGold() == 50);
    assert(hero.getInventory().hasItem("item_hp_pot_s"));
    assert(merchant.getGoods()[0].stock == 4);
    std::cout << "[Test 2] Buying items (Sufficient & Insufficient Gold): PASSED\n";

    // 3. Test Selling item back for 50% value
    // Sell item at inventory index 0 (hpPot buyPrice 50 -> sellPrice 25)
    int heroGoldBeforeSell = hero.getGold(); // 50
    bool sellSuccess = merchant.sellItem(0, hero);
    assert(sellSuccess);
    assert(hero.getGold() == heroGoldBeforeSell + 25);
    assert(!hero.getInventory().hasItem("item_hp_pot_s"));
    std::cout << "[Test 3]Selling items back for 50% Gold: PASSED\n";

    // 4. Test Combat Victory Gold Reward
    Warrior fighter("Arthur");
    fighter.setGold(0);
    Enemy goblin("Goblin Scout", EnemyType::MINION, 30, 5, 1, 30, 25); // exp=30, gold=25

    CombatEngine combat(fighter, goblin, &fighter.getInventory());
    combat.startBattle();
    while (combat.getState() == CombatState::ONGOING)
        combat.executeTurn(1); // Attack
    assert(combat.getState() == CombatState::HERO_VICTORY);
    assert(fighter.getGold() == 25);
    std::cout << "[Test 4] CombatEngine enemy defeat gold reward: PASSED\n";

    // 5. Test StoryGraph branching with Gold conditions
    StoryGraph sg;
    StoryNode gateNode;
    gateNode.id = "Node_Gate";
    gateNode.text = "Cong Thanh Eldoria - Linh gac yeu cau 50 vang de vao";
    gateNode.type = EventType::NORMAL;

    Choice bribeChoice("Hoi lo 50 vang cho linh gac", "Node_InsideCity", "", "", 50, 50);
    Choice sneakChoice("Tim duong vong quanh tuong thanh", "Node_SecretPath", "", "", 0, 0);

    gateNode.choices.push_back(bribeChoice);
    gateNode.choices.push_back(sneakChoice);
    sg.addNode(gateNode);

    StoryNode insideNode;
    insideNode.id = "Node_InsideCity";
    insideNode.text = "Ban da vao duoc ben trong thanh";
    insideNode.type = EventType::NORMAL;
    sg.addNode(insideNode);

    sg.moveToNode("Node_Gate");
    assert(sg.getCurrentNodeId() == "Node_Gate");

    StoryNode curr = sg.getCurrentNode();
    assert(curr.choices.size() == 2);
    assert(curr.choices[0].requiredGold == 50);
    assert(curr.choices[0].goldCost == 50);

    // Hero with 40 gold cannot bribe
    Warrior poorHero("PoorArthur");
    poorHero.setGold(40);
    bool canBribe = (poorHero.getGold() >= curr.choices[0].requiredGold);
    assert(!canBribe);

    // Hero with 100 gold can bribe and gold is deducted
    Warrior richHero("RichArthur");
    richHero.setGold(100);
    assert(richHero.getGold() >= curr.choices[0].requiredGold);
    richHero.spendGold(curr.choices[0].goldCost);
    assert(richHero.getGold() == 50);
    sg.selectChoice(0);
    assert(sg.getCurrentNodeId() == "Node_InsideCity");
    std::cout << "[Test 5] StoryGraph Gold condition & deduction: PASSED\n";

    std::cout << "=======================================\n";
    std::cout << " [ALL PASS] All Shop and Gold Economy tests passed!\n";
    std::cout << "=======================================\n";
    return 0;
}
