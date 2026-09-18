/**
 * @file Ranger.cpp
 * @brief Hiện thực lớp Ranger (Lyra) kế thừa Hero với sát thương cộng từ vũ khí & DoT (Độc tố/Chảy máu).
 */

#include "Ranger.h"
#include <iostream>

Ranger::Ranger(const std::string& name, int hp, int mp, int attack, int defense)
    : Hero(name, HeroClass::RANGER, hp, mp, attack, defense, 5, 0.25f, 0.50f, false) {}

bool Ranger::useSkill(int skillIndex, int& outDamage, std::string& outMessage) {
    int effAtk = getEffectiveAttack();

    switch (skillIndex) {
        case 1: { // Double Shot (180% Effective ATK)
            const int cost = 12;
            if (mp >= cost) {
                mp -= cost;
                outDamage = static_cast<int>(effAtk * 1.8f);
                outMessage = "[Ranger] " + name + " giương cung bắn liên hoàn [Double Shot] gây " +
                             std::to_string(outDamage) + " sát thương!";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Double Shot! (Cần 12 MP, hiện có: " + std::to_string(mp) + ")";
                return false;
            }
        }
        case 2: { // Poison Arrow (Effective ATK * 2 + 10 & gài Độc DoT)
            const int cost = 18;
            if (mp >= cost) {
                mp -= cost;
                outDamage = effAtk * 2 + 10;
                outMessage = "[Ranger] " + name + " bắn mũi tên tẩm kịch độc [Poison Arrow] gây " +
                             std::to_string(outDamage) + " sát thương và gài độc DoT (12 dmg/lượt trong 3 lượt)!";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Poison Arrow! (Cần 18 MP, hiện có: " + std::to_string(mp) + ")";
                return false;
            }
        }
        case 3: { // Rain of Arrows (300% Effective ATK & gài Chảy máu DoT)
            const int cost = 25;
            if (mp >= cost) {
                mp -= cost;
                outDamage = effAtk * 3;
                outMessage = "[Ranger] " + name + " bắn bão tên diện rộng [Rain of Arrows] gây " +
                             std::to_string(outDamage) + " sát thương và gài vết thương Chảy Máu (8 dmg/lượt trong 2 lượt)!";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Rain of Arrows! (Cần 25 MP, hiện có: " + std::to_string(mp) + ")";
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

bool Ranger::useSkill(int skillIndex, int& outDamage) {
    std::string msg;
    bool result = useSkill(skillIndex, outDamage, msg);
    if (result) {
        std::cout << msg << "\n";
    }
    return result;
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
    int effAtk = getEffectiveAttack();
    std::cout << "[Ranger Skills - " << name << "]\n"
              << "  1. Double Shot    (MP: 12) - Bắn 2 mũi tên gây 180% ATK damage (" << static_cast<int>(effAtk * 1.8f) << " dmg)\n"
              << "  2. Poison Arrow   (MP: 18) - Tên độc gây (" << effAtk * 2 + 10 << " dmg) & Độc DoT (12 dmg/lượt)\n"
              << "  3. Rain of Arrows (MP: 25) - Mưa tên áp đảo gây 300% ATK damage (" << effAtk * 3 << " dmg) & Chảy Máu DoT\n";
}
