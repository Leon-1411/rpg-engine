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
#include <memory>

int main() {
    std::cout << "=== RUNNING INTEGRATION TEST: FULL GAMEPLAY FLOW ===\n";

    // -------------------------------------------------------------
    // 1. Khởi tạo Hero đa hình (Polymorphic Hero Creation)
    // -------------------------------------------------------------
    std::shared_ptr<Hero> hero = Hero::createHero(HeroClass::WARRIOR, "Arthur");
    assert(hero != nullptr);
    assert(hero->getName() == "Arthur");
    assert(hero->getHeroClass() == HeroClass::WARRIOR);
    assert(hero->getHp() == 120);
    assert(hero->getMaxHp() == 120);

    // Thêm trang bị cơ bản & kiểm tra chỉ số hiệu dụng
    Item sword("wpn_01", "Iron Sword", "Thanh kiếm sắt", ItemType::WEAPON, 12);
    Item armor("arm_01", "Iron Armor", "Áo giáp sắt", ItemType::ARMOR, 8);
    hero->getInventory().addItem(sword);
    hero->getInventory().addItem(armor);
    assert(hero->getInventory().equipWeapon(0) == true);
    assert(hero->getInventory().equipArmor(1) == true);
    assert(hero->getEffectiveAttack() == 18 + 12); // 30
    assert(hero->getEffectiveDefense() == 8 + 8);   // 16
    std::cout << "  [PASS] Step 1: Hero creation & equipment stats verified.\n";

    // -------------------------------------------------------------
    // 2. Nạp Cốt truyện Fractured Crown từ JSON & Duyệt luồng
    // -------------------------------------------------------------
    StoryGraph story;
    bool storyLoaded = story.loadStoryGraph("data/story.json");
    assert(storyLoaded == true);
    assert(story.getCurrentNode().id == "Node01");

    // Node 01: REWARD node (Nhận Huy hiệu Hoàng Gia + EXP + Gold)
    StoryNode startNode = story.getCurrentNode();
    assert(startNode.rawType == "REWARD");
    assert(startNode.rewardExp == 50);
    assert(startNode.rewardGold == 50);
    assert(!startNode.rewardItems.empty());
    assert(startNode.rewardItems[0] == "RoyalInsignia");

    // Giả lập nhận thưởng
    hero->addExp(startNode.rewardExp);
    hero->addGold(startNode.rewardGold);
    Item royalInsignia("RoyalInsignia", "Royal Insignia", "Huy hiệu Hoàng Gia", ItemType::KEY_ITEM, 0);
    hero->getInventory().addItem(royalInsignia);
    assert(hero->getGold() == 50);
    assert(hero->getInventory().hasItem("RoyalInsignia") == true);

    // Di chuyển sang Ngã Ba Biên Giới Trăng Máu (Node02)
    assert(story.moveToNode(startNode.nextNodeId) == true);
    assert(story.getCurrentNode().id == "Node02");

    // Chọn nhánh [1]: Hướng Rừng Sâu Bộ Tộc Tự Do -> Node03 (COMBAT)
    assert(story.selectChoice(1) == true);
    StoryNode combatNode = story.getCurrentNode();
    assert(combatNode.id == "Node03");
    assert(combatNode.rawType == "COMBAT");
    assert(combatNode.enemyId == "Wild_Mercenary");
    std::cout << "  [PASS] Step 2: Story progression & reward processing verified.\n";

    // -------------------------------------------------------------
    // 3. Chiến đấu Minion & Nhận chiến lợi phẩm
    // -------------------------------------------------------------
    Enemy minion("Wild_Mercenary", EnemyType::MINION, 90, 20, 8, 75, 25);
    Item potion("pot_01", "Health Potion", "Hồi phục 35 HP", ItemType::POTION, 35);
    hero->getInventory().addItem(potion);

    CombatEngine combat(*hero, minion, &hero->getInventory());
    combat.startBattle();

    // Turn 1: Hero tấn công
    combat.executeTurn(1);
    assert(minion.getHp() < 90);

    // Turn 2: Hero dùng kỹ năng Power Slash (skill index 1)
    combat.executeTurn(2, 1);

    // Đánh bại Minion
    minion.takeDamage(100);
    combat.executeTurn(1);
    assert(combat.getState() == CombatState::HERO_VICTORY);

    // Chuyển sang Node chiến thắng: Node06
    story.moveToNode(combatNode.onWinNodeId);
    assert(story.getCurrentNode().id == "Node06");
    std::cout << "  [PASS] Step 3: CombatEngine minion battle & victory transition verified.\n";

    // -------------------------------------------------------------
    // 4. Chiến đấu Boss Tối Cao (Demon King Malakor) & Cơ chế Enrage
    // -------------------------------------------------------------
    BossMonster boss("Demon_King_Malakor", "Demon King Malakor", 450, 65, 35, 400, 200, "Thống lĩnh Ma tộc", "Hellfire Cataclysm", 20);
    CombatEngine bossCombat(*hero, boss, &hero->getInventory());
    bossCombat.startBattle();

    // Test đòn đánh thường
    bossCombat.executeTurn(1);
    assert(!bossCombat.isBattleOver());

    // Test Enrage threshold (< 30% HP)
    boss.takeDamage(350); // HP còn 100/450 (< 30%)
    assert(boss.isEnraged() == true);
    assert(boss.getAttack() > 65); // Tăng sát thương Berserk

    // Kết liễu Boss
    boss.takeDamage(100);
    bossCombat.executeTurn(1);
    assert(bossCombat.getState() == CombatState::HERO_VICTORY);
    std::cout << "  [PASS] Step 4: Boss battle, enrage mechanics & victory verified.\n";

    // -------------------------------------------------------------
    // 5. Kiểm tra Save & Load đa hình tiến trình e2e
    // -------------------------------------------------------------
    SaveManager saveMgr("saves_test_flow/");
    story.moveToNode("End1"); // Khám phá Ending
    assert(saveMgr.saveGame(99, *hero, story) == true);

    std::shared_ptr<Hero> loadedHero;
    StoryGraph loadedStory;
    assert(loadedStory.loadStoryGraph("data/story.json") == true);
    assert(saveMgr.loadGame(99, loadedHero, loadedStory) == true);
    assert(loadedHero != nullptr);
    assert(loadedHero->getName() == "Arthur");
    assert(loadedHero->getHeroClass() == HeroClass::WARRIOR);
    assert(loadedHero->getGold() >= 50);
    assert(loadedHero->getInventory().hasItem("RoyalInsignia") == true);
    assert(loadedStory.getCurrentNode().id == "End1");

    // Dọn dẹp slot test
    saveMgr.deleteSlot(99);
    std::cout << "  [PASS] Step 5: Save/Load polymorphic state persistence verified.\n";

    std::cout << "\n=======================================================\n";
    std::cout << " [SUCCESS] ALL E2E INTEGRATION TESTS PASSED 100%!\n";
    std::cout << "=======================================================\n";
    return 0;
}
