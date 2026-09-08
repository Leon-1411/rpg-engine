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

int main() {
    testLegacyUsage();
    testPotionStacking();
    testEquipmentAndBonuses();
    testCapacityLimit();
    testHeroInventoryIntegration();

    std::cout << "[PASS] All Inventory unit tests successful!\n";
    return 0;
}
