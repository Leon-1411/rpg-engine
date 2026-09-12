/**
 * @file Mage.cpp
 * @brief Hiện thực lớp Mage kế thừa Hero.
 */

#include "Mage.h"
#include <iostream>

Mage::Mage(const std::string& name, int hp, int mp, int attack, int defense)
    : Hero(name, HeroClass::MAGE, hp, mp, attack, defense) {}

bool Mage::useSkill(int skillIndex, int& outDamage) {
    switch (skillIndex) {
        case 1: { // Fireball
            const int cost = 15;
            if (mp >= cost) {
                mp -= cost;
                outDamage = attack * 2 + 10;
                std::cout << "[Mage] " << name << " niệm chú phóng [Fireball] rực lửa!\n";
                return true;
            }
            break;
        }
        case 2: { // Ice Blast
            const int cost = 20;
            if (mp >= cost) {
                mp -= cost;
                outDamage = attack * 2 + 20;
                std::cout << "[Mage] " << name << " triệu hồi băng giá [Ice Blast] đóng băng kẻ thù!\n";
                return true;
            }
            break;
        }
        case 3: { // Meteor
            const int cost = 35;
            if (mp >= cost) {
                mp -= cost;
                outDamage = attack * 3 + 40;
                std::cout << "[Mage] " << name << " gọi mưa thiên thạch hủy diệt [Meteor]!\n";
                return true;
            }
            break;
        }
        default:
            break;
    }
    outDamage = 0;
    return false;
}

void Mage::levelUp() {
    Hero::levelUp();
    std::cout << "[Mage] " << name << " đã thăng lên Cấp " << level << "! MP và Sức mạnh phép thuật bùng nổ!\n";
}

std::string Mage::getSkillName(int skillIndex) const {
    switch (skillIndex) {
        case 1: return "Fireball";
        case 2: return "Ice Blast";
        case 3: return "Meteor";
        default: return "Unknown";
    }
}

void Mage::displaySkills() const {
    std::cout << "[Mage Skills - " << name << "]\n"
              << "  1. Fireball   (MP: 15) - Cầu lửa gây (ATK*2 + 10) damage\n"
              << "  2. Ice Blast  (MP: 20) - Băng tiễn gây (ATK*2 + 20) damage\n"
              << "  3. Meteor     (MP: 35) - Thiên thạch hủy diệt gây (ATK*3 + 40) damage\n";
}
