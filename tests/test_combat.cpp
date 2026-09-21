#include "CombatEngine.h"
#include "Weapon.h"
#include "Armor.h"
#include "Potion.h"
#include <cassert>
#include <iostream>

void testBasicCombat() {
    Hero hero("Knight", HeroClass::WARRIOR, 100, 20, 25, 5);
    Enemy enemy("Slime", EnemyType::MINION, 30, 8, 2, 10, 5);

    CombatEngine engine(hero, enemy);
    engine.startBattle();
    
    assert(engine.getState() == CombatState::ONGOING);

    // Hero attacks slime (25 - 2 = 23 dmg -> Slime HP 7)
    engine.executeTurn(1);
    assert(enemy.getHp() == 7);

    // Hero attacks again (Slime dies)
    engine.executeTurn(1);
    assert(enemy.getHp() == 0);
    assert(engine.getState() == CombatState::HERO_VICTORY);
}

void testUsePotionInCombat() {
    Hero hero("Paladin", HeroClass::WARRIOR, 100, 20, 20, 5);
    Enemy enemy("Goblin", EnemyType::MINION, 50, 15, 2, 20, 10);

    // Cho hero 1 bình Health Potion (hồi 35 HP)
    Potion hpPotion("pot_hp", "Health Potion", "Restores 35 HP", 35, false, 1);
    hero.getInventory().addItem(hpPotion);
    assert(hero.getInventory().getItemCount() == 1);

    // Hero bị mất máu
    hero.takeDamage(50); // HP giảm: 100 - (50 - 5) = 55 HP
    assert(hero.getHp() == 55);

    CombatEngine engine(hero, enemy);
    engine.startBattle();

    // Dùng Potion trong combat (actionChoice = 3, tự động chọn potion)
    // Sau khi dùng potion: HP = 55 + 35 = 90.
    // Tiếp đó quái vật phản công: atk 15 - def 5 = 10 damage -> Hero HP = 80.
    // Số lượng potion trong túi giảm về 0 (bị xoá khỏi inventory).
    engine.executeTurn(3);

    assert(hero.getInventory().getItemCount() == 0);
    assert(hero.getHp() == 80);
    assert(engine.getState() == CombatState::ONGOING);
}

void testItemDropOnDefeat() {
    Hero hero("Knight", HeroClass::WARRIOR, 100, 20, 30, 5);
    Enemy boss("Boss Monster", EnemyType::BOSS, 20, 10, 0, 100, 50);

    // Thiết lập drop table cho quái vật (100% tỷ lệ rớt)
    auto sword = std::make_shared<Weapon>("wpn_drop", "Drop Sword", "Strong blade", 15);
    auto armor = std::make_shared<Armor>("arm_drop", "Drop Shield", "Sturdy shield", 10);
    auto potion = std::make_shared<Potion>("pot_drop", "Drop Potion", "Heals 30 HP", 30, false, 1);

    boss.addDropItem(sword, 1.0);
    boss.addDropItem(armor, 1.0);
    boss.addDropItem(potion, 1.0);

    CombatEngine engine(hero, boss);
    engine.startBattle();

    // Hero tấn công và hạ gục boss (30 atk - 0 def = 30 dmg > 20 hp)
    engine.executeTurn(1);
    assert(engine.getState() == CombatState::HERO_VICTORY);

    // Kiểm tra danh sách vật phẩm rơi ra
    const auto& drops = engine.getLastDroppedItems();
    assert(drops.size() == 3);

    // Kiểm tra các vật phẩm đã được tự động thêm vào túi đồ của hero
    assert(hero.getInventory().hasItem("wpn_drop"));
    assert(hero.getInventory().hasItem("arm_drop"));
    assert(hero.getInventory().hasItem("pot_drop"));
    assert(hero.getInventory().getItemCount() == 3);
}

void testItemDropInventoryFull() {
    Hero hero("Knight", HeroClass::WARRIOR, 100, 20, 30, 5);
    // Làm đầy túi đồ của hero (capacity mặc định = 20, thêm 20 items)
    for (int i = 0; i < 20; ++i) {
        std::string id = "item_" + std::to_string(i);
        Item dummy(id, "Dummy Item", "Desc", ItemType::WEAPON, 1);
        hero.getInventory().addItem(dummy);
    }
    assert(hero.getInventory().isFull());

    Enemy minion("Goblin", EnemyType::MINION, 10, 5, 0, 10, 5);
    auto sword = std::make_shared<Weapon>("wpn_rare", "Rare Blade", "Rare", 25);
    minion.addDropItem(sword, 1.0);

    CombatEngine engine(hero, minion);
    engine.startBattle();

    // Đánh bại minion
    engine.executeTurn(1);
    assert(engine.getState() == CombatState::HERO_VICTORY);

    // Vật phẩm có rớt nhưng hero không nhặt được do túi đầy
    assert(engine.getLastDroppedItems().size() == 1);
    assert(hero.getInventory().hasItem("wpn_rare") == false);
}

int main() {
    testBasicCombat();
    testUsePotionInCombat();
    testItemDropOnDefeat();
    testItemDropInventoryFull();

    std::cout << "[PASS] CombatEngine unit tests (potion & item drops) successful!\n";
    return 0;
}

