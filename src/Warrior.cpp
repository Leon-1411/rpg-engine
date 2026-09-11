/**
 * @file Warrior.cpp
 * @brief Hiện thực lớp Warrior kế thừa Hero.
 */

#include "Warrior.h"
#include <iostream>

Warrior::Warrior(const std::string& name, int hp, int mp, int attack, int defense)
    : Hero(name, HeroClass::WARRIOR, hp, mp, attack, defense) {}

bool Warrior::useSkill(int skillIndex, int& outDamage) {
    switch (skillIndex) {
        case 1: { // Power Slash
            const int cost = 10;
            if (mp >= cost) {
                mp -= cost;
                outDamage = attack * 2;
                std::cout << "[Warrior] " << name << " tung chiêu [Power Slash] chém mạnh mẽ!\n";
                return true;
            }
            break;
        }
        case 2: { // Shield Block
            const int cost = 12;
            if (mp >= cost) {
                mp -= cost;
                outDamage = attack + defense * 2;
                std::cout << "[Warrior] " << name << " dựng khiên chắn và phản đòn [Shield Block]!\n";
                return true;
            }
            break;
        }
        case 3: { // Berserk
            const int cost = 15;
            if (mp >= cost) {
                mp -= cost;
                outDamage = attack * 3;
                std::cout << "[Warrior] " << name << " bước vào trạng thái cuồng bạo [Berserk]!\n";
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

void Warrior::levelUp() {
    Hero::levelUp();
    std::cout << "[Warrior] " << name << " đã thăng lên Cấp " << level << "! HP và DEF tăng cường vượt bậc!\n";
}

std::string Warrior::getSkillName(int skillIndex) const {
    switch (skillIndex) {
        case 1: return "Power Slash";
        case 2: return "Shield Block";
        case 3: return "Berserk";
        default: return "Unknown";
    }
}

void Warrior::displaySkills() const {
    std::cout << "[Warrior Skills - " << name << "]\n"
              << "  1. Power Slash   (MP: 10) - Chém uy lực gây 200% ATK damage\n"
              << "  2. Shield Block  (MP: 12) - Dựng khiên phản kích gây (ATK + DEF*2) damage\n"
              << "  3. Berserk       (MP: 15) - Trọng kích cuồng nộ gây 300% ATK damage\n";
}
