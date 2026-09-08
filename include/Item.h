#pragma once

#include <string>
#include <iostream>
#include <memory>

enum class ItemType {
    WEAPON,
    ARMOR,
    POTION
};

class Hero; // forward declaration

class Item {
protected:
    std::string id;
    std::string name;
    std::string description;
    ItemType type;
    int statValue; // Attack bonus for Weapon, Defense for Armor, HP/MP restore for Potion

public:
    Item(const std::string& id, const std::string& name, const std::string& description, ItemType type, int statValue);
    virtual ~Item() = default;

    std::string getId() const;
    std::string getName() const;
    std::string getDescription() const;
    ItemType getType() const;
    int getStatValue() const;

    virtual void displayInfo() const;
    virtual std::shared_ptr<Item> clone() const;
};

class Weapon : public Item {
private:
    int attackBonus;

public:
    Weapon(const std::string& id, const std::string& name, const std::string& description, int attackBonus);
    virtual ~Weapon() = default;

    int getAttackBonus() const;
    void displayInfo() const override;
    std::shared_ptr<Item> clone() const override;
};

class Armor : public Item {
private:
    int defenseBonus;

public:
    Armor(const std::string& id, const std::string& name, const std::string& description, int defenseBonus);
    virtual ~Armor() = default;

    int getDefenseBonus() const;
    void displayInfo() const override;
    std::shared_ptr<Item> clone() const override;
};

class Potion : public Item {
private:
    int healAmount;
    bool isManaPotion;
    int quantity;

public:
    Potion(const std::string& id, const std::string& name, const std::string& description, int healAmount, bool isManaPotion = false, int quantity = 1);
    virtual ~Potion() = default;

    int getHealAmount() const;
    bool isMana() const;
    int getQuantity() const;
    void setQuantity(int qty);
    void addQuantity(int qty);
    bool consumeOne(); // Giảm 1 đơn vị, trả về true nếu vẫn còn > 0, false nếu đã hết
    void apply(Hero& hero) const;
    void displayInfo() const override;
    std::shared_ptr<Item> clone() const override;
};

