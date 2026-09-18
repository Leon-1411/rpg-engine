/**
 * @file Mage.cpp
 * @brief Hiện thực lớp Mage (Morrigan) kế thừa Hero với sát thương phép cộng từ vũ khí & hồi phục Mana.
 */

#include "Mage.h"
#include <iostream>

Mage::Mage(const std::string& name, int hp, int mp, int attack, int defense)
    : Hero(name, HeroClass::MAGE, hp, mp, attack, defense, 0, 0.0f, 0.0f, true) {}

bool Mage::useSkill(int skillIndex, int& outDamage, std::string& outMessage) {
    outDamage = 0;
    int effAtk = getEffectiveAttack();

    switch (skillIndex) {
        case 1: { // Fireball
            const int cost = 15;
            if (mp >= cost) {
                mp -= cost;
                outDamage = effAtk * 2 + 10;
                outMessage = "[Mage] " + name + " niệm chú phóng [Fireball] rực lửa gây " +
                             std::to_string(outDamage) + " sát thương phép xuyên giáp!";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Fireball! (Cần 15 MP, hiện có: " + std::to_string(mp) + ")";
                return false;
            }
        }
        case 2: { // Ice Blast (Sát thương & Hồi phục 10 Mana)
            const int cost = 20;
            if (mp >= cost) {
                mp -= cost;
                outDamage = effAtk * 2 + 20;
                restoreMp(10);
                outMessage = "[Mage] " + name + " triệu hồi hàn băng [Ice Blast] gây " +
                             std::to_string(outDamage) + " sát thương phép và hấp thụ hồi phục +10 MP! (MP: " +
                             std::to_string(mp) + "/" + std::to_string(maxMp) + ")";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Ice Blast! (Cần 20 MP, hiện có: " + std::to_string(mp) + ")";
                return false;
            }
        }
        case 3: { // Meteor
            const int cost = 35;
            if (mp >= cost) {
                mp -= cost;
                outDamage = effAtk * 3 + 40;
                outMessage = "[Mage] " + name + " gọi mưa thiên thạch hủy diệt [Meteor] giáng xuống gây " +
                             std::to_string(outDamage) + " sát thương ma thuật cực đại!";
                return true;
            } else {
                outMessage = "Không đủ MP để dùng Meteor! (Cần 35 MP, hiện có: " + std::to_string(mp) + ")";
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

bool Mage::useSkill(int skillIndex, int& outDamage) {
    std::string msg;
    bool result = useSkill(skillIndex, outDamage, msg);
    if (result) {
        std::cout << msg << "\n";
    }
    return result;
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
    int effAtk = getEffectiveAttack();
    std::cout << "[Mage Skills - " << name << "]\n"
              << "  1. Fireball   (MP: 15) - Cầu lửa gây (" << effAtk * 2 + 10 << " dmg) phép xuyên giáp\n"
              << "  2. Ice Blast  (MP: 20) - Băng tiễn gây (" << effAtk * 2 + 20 << " dmg) phép & Hồi phục +10 MP\n"
              << "  3. Meteor     (MP: 35) - Thiên thạch hủy diệt gây (" << effAtk * 3 + 40 << " dmg) phép cực đại\n";
}
