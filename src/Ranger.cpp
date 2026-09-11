/**
 * @file Ranger.cpp
 * @brief Hiện thực lớp Ranger kế thừa Hero.
 */

#include "Ranger.h"
#include <iostream>

Ranger::Ranger(const std::string& name, int hp, int mp, int attack, int defense)
    : Hero(name, HeroClass::RANGER, hp, mp, attack, defense) {}

bool Ranger::useSkill(int skillIndex, int& outDamage) {
    switch (skillIndex) {
        case 1: { // Double Shot
            const int cost = 12;
            if (mp >= cost) {
                mp -= cost;
                outDamage = static_cast<int>(attack * 1.8);
                std::cout << "[Ranger] " << name << " giương cung bắn liên hoàn [Double Shot]!\n";
                return true;
            }
            break;
        }
        case 2: { // Poison Arrow
            const int cost = 18;
            if (mp >= cost) {
                mp -= cost;
                outDamage = attack * 2 + 10;
                std::cout << "[Ranger] " << name << " bắn mũi tên tẩm kịch độc [Poison Arrow]!\n";
                return true;
            }
            break;
        }
        case 3: { // Rain of Arrows
            const int cost = 25;
            if (mp >= cost) {
                mp -= cost;
                outDamage = attack * 3;
                std::cout << "[Ranger] " << name << " bắn bão tên diện rộng [Rain of Arrows]!\n";
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

void Ranger::levelUp() {
    Hero::levelUp();
    std::cout << "[Ranger] " << name << " đã thăng lên Cấp " << level << "! Tốc độ và Sát thương chuẩn xác gia tăng!\n";
}

std::string Ranger::getSkillName(int skillIndex) const {
    switch (skillIndex) {
        case 1: return "Double Shot";
        case 2: return "Poison Arrow";
        case 3: return "Rain of Arrows";
        default: return "Unknown";
    }
}

void Ranger::displaySkills() const {
    std::cout << "[Ranger Skills - " << name << "]\n"
              << "  1. Double Shot    (MP: 12) - Bắn 2 mũi tên gây 180% ATK damage\n"
              << "  2. Poison Arrow   (MP: 18) - Tên độc gây (ATK*2 + 10) damage\n"
              << "  3. Rain of Arrows (MP: 25) - Mưa tên áp đảo gây 300% ATK damage\n";
}
