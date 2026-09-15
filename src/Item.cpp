/**
 * @file Item.cpp
 * @brief Implement Item, Weapon, Armor, and Potion class methods.
 * @author Sang
 */

#include "Item.h"
#include "Hero.h"

// ==========================================
// Base Item Implementation
// ==========================================

Item::Item(const std::string& id, const std::string& name, const std::string& description, ItemType type, int statValue)
    : id(id), name(name), description(description), type(type), statValue(statValue) {}

std::string Item::getId() const { return id; }
std::string Item::getName() const { return name; }
std::string Item::getDescription() const { return description; }
ItemType Item::getType() const { return type; }
int Item::getStatValue() const { return statValue; }

void Item::displayInfo() const {
    std::cout << "[" << id << "] " << name << ": " << description << " (Value: " << statValue << ")\n";
}

std::shared_ptr<Item> Item::clone() const {
    return std::make_shared<Item>(*this);
}

// ==========================================
// Weapon Implementation
// ==========================================

Weapon::Weapon(const std::string& id, const std::string& name, const std::string& description, int attackBonus)
    : Item(id, name, description, ItemType::WEAPON, attackBonus), attackBonus(attackBonus) {}

int Weapon::getAttackBonus() const {
    return attackBonus;
}

void Weapon::displayInfo() const {
    std::cout << "[" << id << "] " << name << " (Weapon): " << description << " [ATK +" << attackBonus << "]\n";
}

std::shared_ptr<Item> Weapon::clone() const {
    return std::make_shared<Weapon>(*this);
}

// ==========================================
// Armor Implementation
// ==========================================

Armor::Armor(const std::string& id, const std::string& name, const std::string& description, int defenseBonus)
    : Item(id, name, description, ItemType::ARMOR, defenseBonus), defenseBonus(defenseBonus) {}

int Armor::getDefenseBonus() const {
    return defenseBonus;
}

void Armor::displayInfo() const {
    std::cout << "[" << id << "] " << name << " (Armor): " << description << " [DEF +" << defenseBonus << "]\n";
}

std::shared_ptr<Item> Armor::clone() const {
    return std::make_shared<Armor>(*this);
}

// ==========================================
// Potion Implementation
// ==========================================

Potion::Potion(const std::string& id, const std::string& name, const std::string& description, int healAmount, bool isManaPotion, int quantity)
    : Item(id, name, description, ItemType::POTION, healAmount),
      healAmount(healAmount), isManaPotion(isManaPotion), quantity(quantity > 0 ? quantity : 1) {}

int Potion::getHealAmount() const {
    return healAmount;
}

bool Potion::isMana() const {
    return isManaPotion;
}

int Potion::getQuantity() const {
    return quantity;
}

void Potion::setQuantity(int qty) {
    quantity = (qty >= 0) ? qty : 0;
}

void Potion::addQuantity(int qty) {
    if (qty > 0) {
        quantity += qty;
    }
}

bool Potion::consumeOne() {
    if (quantity > 0) {
        quantity--;
    }
    return quantity > 0;
}

void Potion::apply(Hero& hero) const {
    if (isManaPotion) {
        hero.restoreMp(healAmount);
    } else {
        hero.heal(healAmount);
    }
}

void Potion::displayInfo() const {
    std::cout << "[" << id << "] " << name << " (x" << quantity << "): "
              << description << " [" << (isManaPotion ? "Restore MP +" : "Heal HP +")
              << healAmount << "]\n";
}

std::shared_ptr<Item> Potion::clone() const {
    return std::make_shared<Potion>(*this);
}
