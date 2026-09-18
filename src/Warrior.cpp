/**
 * @file Warrior.cpp
 * @brief Hiện thực lớp Warrior (Arthur) kế thừa Hero với sát thương cộng dồn từ vũ khí & khống chế Choáng.
 */

#include "Warrior.h"
#include <iostream>

Warrior::Warrior(const std::string& name, int hp, int mp, int attack, int defense)
    : Hero(name, HeroClass::WARRIOR, hp, mp, attack, defense, 10, 0.10f, 0.25f, false) {}

bool Warrior::useSkill(int skillIndex, int& outDamage, std::string& outMessage) {
    int effAtk = getEffectiveAttack();
    int effDef = getEffectiveDefense();

    switch (skillIndex) {
        case 1: { // Power Slash (200% Effective ATK)
            const int cost = 10;
            if (mp >= cost) {
                mp -= cost;
                outDamage = effAtk * 2;
                outMessage = "[Warrior] " + name + " tung chiêu [Power Slash] chém uy lực gây " +
                             std::to_string(outDamage) + " sát thương!";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Power Slash! (Cần 10 MP, hiện có: " + std::to_string(mp) + ")";
                return false;
            }
        }
        case 2: { // Shield Block & Slam (Effective ATK + Effective DEF*2, Thế thủ & Gây Choáng)
            const int cost = 12;
            if (mp >= cost) {
                mp -= cost;
                outDamage = effAtk + effDef * 2;
                isBlocking = true;
                outMessage = "[Warrior] " + name + " dựng khiên phản đòn [Shield Block] gây " +
                             std::to_string(outDamage) + " sát thương, kích hoạt thế thủ chặn đòn và làm choáng kẻ địch!";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Shield Block! (Cần 12 MP, hiện có: " + std::to_string(mp) + ")";
                return false;
            }
        }
        case 3: { // Berserk (300% Effective ATK)
            const int cost = 15;
            if (mp >= cost) {
                mp -= cost;
                outDamage = effAtk * 3;
                outMessage = "[Warrior] " + name + " bước vào trạng thái cuồng nộ [Berserk] giáng đòn sấm sét gây " +
                             std::to_string(outDamage) + " sát thương!";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Berserk! (Cần 15 MP, hiện có: " + std::to_string(mp) + ")";
                return false;
            }
        }
        default:
            outMessage = "Kỹ năng không hợp lệ (chỉ chọn từ 1 đến 3)!";
            break;
    }
    outDamage = 0;
    return false;
}

bool Warrior::useSkill(int skillIndex, int& outDamage) {
    std::string msg;
    bool result = useSkill(skillIndex, outDamage, msg);
    if (result) {
        std::cout << msg << "\n";
    }
    return result;
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
              << "  1. Power Slash   (MP: 10) - Chém uy lực gây 200% ATK damage (" << getEffectiveAttack() * 2 << " dmg)\n"
              << "  2. Shield Block  (MP: 12) - Dựng khiên phản kích (" << getEffectiveAttack() + getEffectiveDefense() * 2
              << " dmg), phòng thủ 100% & Gây Choáng kẻ địch\n"
              << "  3. Berserk       (MP: 15) - Trọng kích cuồng nộ gây 300% ATK damage (" << getEffectiveAttack() * 3 << " dmg)\n";
}
