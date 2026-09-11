#include "Inventory.h"
#include "Hero.h"
#include "Weapon.h"
#include "Armor.h"
#include "Potion.h"
#include <cassert>
#include <iostream>

void testLegacyUsage() {
    Inventory inv(5);
    Item pot("p01", "Health Potion", "Heals 30 HP", ItemType::POTION, 30);
    Item wpn("w01", "Sword", "Weapon", ItemType::WEAPON, 15);

    assert(inv.addItem(pot) == true);
    assert(inv.addItem(wpn) == true);
    assert(inv.getItemCount() == 2);

    Hero hero("Tester", HeroClass::WARRIOR, 100, 20, 10, 5);
    hero.takeDamage(40); // 100 - (40 - 5) = 65
    assert(hero.getHp() == 65);

    // Use potion at index 0
    assert(inv.useItem(0, hero) == true);
    assert(hero.getHp() == 95);
    assert(inv.getItemCount() == 1);
}

void testPotionStacking() {
    Inventory inv(3);
    Item pot1("p01", "Health Potion", "Heals 20 HP", ItemType::POTION, 20);
    Item pot2("p01", "Health Potion", "Heals 20 HP", ItemType::POTION, 20);

    assert(inv.addItem(pot1) == true);
    assert(inv.getItemCount() == 1);

    // Thêm lọ thứ 2 cùng ID -> Gộp (stack) số lượng, không tăng số slot
    assert(inv.addItem(pot2) == true);
    assert(inv.getItemCount() == 1);

    auto itemPtr = inv.getItemPtr(0);
    auto potion = std::dynamic_pointer_cast<Potion>(itemPtr);
    assert(potion != nullptr);
    assert(potion->getQuantity() == 2);

    Hero hero("StackTester", HeroClass::WARRIOR, 100, 20, 10, 5);
    hero.takeDamage(50); // HP = 55

    // Dùng lần 1: Còn 1 lọ trong túi
    assert(inv.useItem(0, hero) == true);
    assert(hero.getHp() == 75);
    assert(inv.getItemCount() == 1);
    assert(potion->getQuantity() == 1);

    // Dùng lần 2: Hết lọ, tự động xóa khỏi mảng
    assert(inv.useItem(0, hero) == true);
    assert(hero.getHp() == 95);
    assert(inv.getItemCount() == 0);
}

void testEquipmentAndBonuses() {
    Inventory inv(5);
    auto sword = std::make_shared<Weapon>("w_sw", "Broadsword", "Blade", 20);
    auto armor = std::make_shared<Armor>("a_ar", "Steel Plate", "Plate", 15);

    inv.addItem(sword);
    inv.addItem(armor);
    assert(inv.getItemCount() == 2);

    // Mặc trang bị
    assert(inv.equipWeapon(0) == true);
    assert(inv.equipArmor(1) == true);
    assert(inv.getEquippedWeaponBonus() == 20);
    assert(inv.getEquippedArmorBonus() == 15);

    // Tháo trang bị
    inv.unequipWeapon();
    assert(inv.getEquippedWeaponBonus() == 0);
    inv.unequipArmor();
    assert(inv.getEquippedArmorBonus() == 0);
}

void testCapacityLimit() {
    Inventory inv(2);
    Item w1("w1", "W1", "Desc", ItemType::WEAPON, 5);
    Item w2("w2", "W2", "Desc", ItemType::WEAPON, 5);
    Item w3("w3", "W3", "Desc", ItemType::WEAPON, 5);

    assert(inv.addItem(w1) == true);
    assert(inv.addItem(w2) == true);
    assert(inv.addItem(w3) == false); // Đầy túi đồ
    assert(inv.getItemCount() == 2);
}

// ==========================================
// Test giới hạn tối đa 10 items (VD: 10 items)
// ==========================================
void testMaxCapacity10Items() {
    Inventory inv(10); // Khởi tạo túi đồ giới hạn tối đa 10 món
    assert(inv.getCapacity() == 10);
    assert(inv.getItemCount() == 0);
    assert(inv.isFull() == false);

    // Thêm tuần tự 10 món đồ -> tất cả đều thành công
    for (int i = 1; i <= 10; ++i) {
        std::string id = "item_" + std::to_string(i);
        Item wpn(id, "Item " + std::to_string(i), "Equipment", ItemType::WEAPON, 10 + i);
        assert(inv.addItem(wpn) == true);
    }

    assert(inv.getItemCount() == 10);
    assert(inv.isFull() == true);

    // Bắt lỗi: Thêm vật phẩm thứ 11 -> Phải trả về false và không tăng kích thước túi
    Item overflowItem("item_11", "Overflow Blade", "Extra", ItemType::WEAPON, 99);
    assert(inv.addItem(overflowItem) == false);
    assert(inv.getItemCount() == 10);
    assert(inv.hasItem("item_11") == false);

    // Thử thêm 1 loại potion mới khi túi đã có 10 items -> phải bị từ chối
    Item newPotion("pot_new", "Elixir", "Restores HP", ItemType::POTION, 50);
    assert(inv.addItem(newPotion) == false);
    assert(inv.getItemCount() == 10);

    // Xóa 1 vật phẩm khỏi túi
    assert(inv.removeItem("item_1") == true);
    assert(inv.getItemCount() == 9);
    assert(inv.isFull() == false);

    // Lúc này đã có chỗ trống, có thể thêm lại thành công
    assert(inv.addItem(overflowItem) == true);
    assert(inv.getItemCount() == 10);
    assert(inv.isFull() == true);
    assert(inv.hasItem("item_11") == true);
}

void testHeroInventoryIntegration() {
    Hero hero("EquippedKnight", HeroClass::WARRIOR, 100, 30, 25, 10);
    assert(hero.getEffectiveAttack() == 25);
    assert(hero.getEffectiveDefense() == 10);

    auto sword = std::make_shared<Weapon>("w_ex", "Excalibur", "Holy blade", 30);
    auto armor = std::make_shared<Armor>("a_dr", "Dragon Armor", "Dragon scales", 20);

    hero.getInventory().addItem(sword);
    hero.getInventory().addItem(armor);

    hero.getInventory().equipWeapon(0);
    hero.getInventory().equipArmor(1);

    // Kiểm tra chỉ số cộng dồn
    assert(hero.getEffectiveAttack() == 55); // 25 + 30
    assert(hero.getEffectiveDefense() == 30); // 10 + 20
}

// ==========================================
// Test removeItem(id): Weapon/Armor xóa trực tiếp
// ==========================================
void testRemoveItemById() {
    Inventory inv(5);
    auto sword = std::make_shared<Weapon>("w_sw", "Sword", "Blade", 15);
    auto shield = std::make_shared<Armor>("a_sh", "Shield", "Guard", 10);

    inv.addItem(sword);
    inv.addItem(shield);
    assert(inv.getItemCount() == 2);

    // Xóa Weapon theo ID
    assert(inv.removeItem("w_sw") == true);
    assert(inv.getItemCount() == 1);
    assert(inv.hasItem("w_sw") == false);
    assert(inv.hasItem("a_sh") == true);

    // Xóa ID không tồn tại
    assert(inv.removeItem("nonexistent") == false);
}

// ==========================================
// Test removeItem(id) với Potion: giảm qty trước, xóa slot khi qty == 1
// ==========================================
void testRemovePotionById() {
    Inventory inv(5);
    // Thêm 3 lọ potion cùng ID (stack vào 1 slot, qty = 3)
    Potion pot("p01", "Health Potion", "Heals 30 HP", 30, false, 1);
    inv.addItem(pot);
    inv.addItem(pot);
    inv.addItem(pot);
    assert(inv.getItemCount() == 1); // Chỉ 1 slot
    assert(inv.getItemCountById("p01") == 3);

    // Lần 1: qty giảm xuống 2, slot vẫn còn
    assert(inv.removeItem("p01") == true);
    assert(inv.getItemCount() == 1);
    assert(inv.getItemCountById("p01") == 2);

    // Lần 2: qty giảm xuống 1, slot vẫn còn
    assert(inv.removeItem("p01") == true);
    assert(inv.getItemCount() == 1);
    assert(inv.getItemCountById("p01") == 1);

    // Lần 3: qty = 1 → xóa slot
    assert(inv.removeItem("p01") == true);
    assert(inv.getItemCount() == 0);
    assert(inv.hasItem("p01") == false);
}

// ==========================================
// Test findItemIndex, hasItem, getItemById
// ==========================================
void testQueryHelpers() {
    Inventory inv(5);
    auto sword = std::make_shared<Weapon>("w01", "Sword", "Sharp", 12);
    inv.addItem(sword);

    assert(inv.hasItem("w01") == true);
    assert(inv.hasItem("w99") == false);
    assert(inv.findItemIndex("w01") == 0);
    assert(inv.findItemIndex("w99") == -1);

    auto ptr = inv.getItemById("w01");
    assert(ptr != nullptr);
    assert(ptr->getId() == "w01");

    assert(inv.getItemById("w99") == nullptr);
}

// ==========================================
// Test useItemById
// ==========================================
void testUseItemById() {
    Inventory inv(5);
    Potion pot("p_hp", "Health Potion", "Heals 40 HP", 40, false, 2);
    inv.addItem(pot);

    Hero hero("Tester", HeroClass::WARRIOR, 100, 20, 10, 5);
    hero.takeDamage(60); // HP = 45

    // Dùng bằng ID: qty 2→1
    assert(inv.useItemById("p_hp", hero) == true);
    assert(hero.getHp() == 85); // 45 + 40
    assert(inv.getItemCountById("p_hp") == 1);

    // Dùng lần 2: qty 1→0, slot bị xóa
    assert(inv.useItemById("p_hp", hero) == true);
    assert(inv.hasItem("p_hp") == false);

    // Dùng lần 3: không còn item
    assert(inv.useItemById("p_hp", hero) == false);
}

int main() {
    testLegacyUsage();
    testPotionStacking();
    testEquipmentAndBonuses();
    testCapacityLimit();
    testMaxCapacity10Items();
    testHeroInventoryIntegration();
    testRemoveItemById();
    testRemovePotionById();
    testQueryHelpers();
    testUseItemById();

    std::cout << "[PASS] All Inventory unit tests successful!\n";
    return 0;
}
