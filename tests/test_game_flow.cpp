/**
 * @file test_game_flow.cpp
 * @brief Full end-to-end integration test for RPG Engine gameplay flow:
 *        Hero creation -> Story progression -> Reward collection ->
 *        Requirement check -> Boss combat & drops -> Victory ending -> Save/Load.
 * @author Phong & Team
 */

#include "Hero.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "Enemy.h"
#include "Minion.h"
#include "BossMonster.h"
#include "Item.h"
#include "Inventory.h"
#include "CombatEngine.h"
#include "StoryGraph.h"
#include "SaveManager.h"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "=== RUNNING INTEGRATION TEST: FULL GAMEPLAY FLOW ===\n";

    // 1. Khởi tạo Hero
    Warrior hero("Arthur", 200, 80, 45, 20);
    assert(hero.getName() == "Arthur");
    assert(hero.getHp() == 200);

    // Thêm trang bị cơ bản
    Item sword("wpn_01", "Iron Sword", "Thanh kiếm sắt", ItemType::WEAPON, 12);
    Item armor("arm_01", "Iron Armor", "Áo giáp sắt", ItemType::ARMOR, 8);
    hero.getInventory().addItem(sword);
    hero.getInventory().addItem(armor);
    assert(hero.getInventory().equipWeapon(0) == true);
    assert(hero.getInventory().equipArmor(1) == true);
    assert(hero.getEffectiveAttack() == 45 + 12);
    assert(hero.getEffectiveDefense() == 20 + 8);

    // 2. Nạp Cốt truyện từ JSON
    StoryGraph story;
    bool storyLoaded = story.loadStoryGraph("data/story.json");
    assert(storyLoaded == true);
    assert(story.getCurrentNode().id == "node_01");

    // Chọn nhánh [1]: Khám phá Hang Cổ Băng Giá -> node_03
    assert(story.selectChoice(1) == true);
    assert(story.getCurrentNode().id == "node_03");

    // Chọn mở rương báu [0] -> node_05 (REWARD)
    assert(story.selectChoice(0) == true);
    StoryNode rewardNode = story.getCurrentNode();
    assert(rewardNode.id == "node_05");
    assert(rewardNode.rawType == "REWARD");
    assert(rewardNode.rewardExp == 100);
    assert(rewardNode.rewardItems.size() == 2);
    assert(rewardNode.rewardItems[0] == "IceKey");

    // Giả lập nhận thưởng tại REWARD node
    hero.addExp(rewardNode.rewardExp);
    Item iceKey("IceKey", "Chìa Khóa Băng Bất Hoại", "Cổ vật mở phong ấn", ItemType::ARMOR, 5);
    Item bigPotion("GreaterHealthPotion", "Bình Thuốc Máu Lớn", "Hồi phục 75 HP", ItemType::POTION, 75);
    hero.getInventory().addItem(iceKey);
    hero.getInventory().addItem(bigPotion);
    assert(hero.getInventory().hasItem("IceKey") == true);

    // Di chuyển tiếp sang Sảnh Ngoài Đền Băng (node_04)
    story.moveToNode(rewardNode.nextNodeId);
    assert(story.getCurrentNode().id == "node_04");

    // Chọn lối đi bí mật [1] -> node_09 (REQUIREMENT_CHECK)
    assert(story.selectChoice(1) == true);
    StoryNode checkNode = story.getCurrentNode();
    assert(checkNode.id == "node_09");
    assert(checkNode.rawType == "REQUIREMENT_CHECK");
    assert(checkNode.requiredItem == "IceKey");

    // Kiểm tra điều kiện có chìa khóa: hasItem == true -> qua node_10
    bool passCheck = hero.getInventory().hasItem(checkNode.requiredItem);
    assert(passCheck == true);
    story.moveToNode(checkNode.onPassNodeId);
    assert(story.getCurrentNode().id == "node_10");

    // 3. Chiến đấu Boss Thần Thú Băng Cổ Đại (node_10)
    StoryNode combatNode = story.getCurrentNode();
    assert(combatNode.rawType == "COMBAT");
    assert(combatNode.enemyId == "boss_ice_dragon");

    auto boss = BossFactory::createFromJson(combatNode.enemyId, "data/enemies.json");
    assert(boss != nullptr);
    assert(boss->getHp() == 550);
    assert(boss->getAttack() == 70);

    // Đặt drop item cho boss
    auto dropKey = std::make_shared<Potion>("GreaterHealthPotion", "Bình Thuốc Máu Lớn", "Hồi 75 HP", 75);
    boss->addDropItem(dropKey, 1.0);

    CombatEngine combat(hero, *boss);
    combat.startBattle();

    // Turn 1: Defend test
    combat.executeTurn(4); // Hero defends
    assert(combat.isBattleOver() == false);

    // Turn 2: Dùng kỹ năng Power Slash (skill index 1)
    combat.executeTurn(2, 1); // Skill index: 1 = Power Slash
    assert(combat.isBattleOver() == false);

    // Giảm máu boss xuống dưới 30% để test Enrage
    boss->setHp(100);
    assert(boss->checkEnrage() == true);
    combat.executeTurn(1); // Normal attack
    assert(boss->isEnraged() == true);

    // Đánh đòn quyết định hạ gục boss
    boss->setHp(10);
    combat.executeTurn(1);
    assert(boss->isAlive() == false);
    assert(combat.getState() == CombatState::HERO_VICTORY);

    // Kiểm tra rớt đồ
    const auto& drops = combat.getLastDroppedItems();
    assert(!drops.empty());
    assert(drops[0]->getName() == "Bình Thuốc Máu Lớn");

    // Chuyển sang True Ending (node_11)
    story.moveToNode(combatNode.onWinNodeId);
    assert(story.getCurrentNode().id == "node_11");
    assert(story.getCurrentNode().rawType == "VICTORY");

    // 4. Kiểm tra Save / Load tiến trình kết thúc
    SaveManager saveMgr("saves_test_flow/");
    assert(saveMgr.saveGame(88, hero, story) == true);

    Warrior loadedHero("Temp", 1, 1, 1, 1);
    StoryGraph loadedStory;
    assert(loadedStory.loadStoryGraph("data/story.json") == true); // Phải load JSON trước
    assert(saveMgr.loadGame(88, loadedHero, loadedStory) == true);
    assert(loadedStory.getCurrentNode().id == "node_11");
    assert(loadedHero.getInventory().hasItem("IceKey") == true);

    // Dọn dẹp test slot
    saveMgr.deleteSlot(88);

    std::cout << "[SUCCESS] ALL INTEGRATION TESTS PASSED! Game loop, Story, Combat, Drops, Save/Load verified.\n";
    return 0;
}
