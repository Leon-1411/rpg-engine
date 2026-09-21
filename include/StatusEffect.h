#pragma once

#include <string>

enum class StatusType {
    NONE,
    POISON,        // Damage over time (Độc rút máu mỗi lượt)
    REGENERATION,  // Heal over time (Hồi máu mỗi lượt)
    BURN,          // [TÍNH NĂNG ĐANG ĐƯỢC PHÁT TRIỂN] Thiêu đốt
    FREEZE,        // [TÍNH NĂNG ĐANG ĐƯỢC PHÁT TRIỂN] Đóng băng
    STUN,          // [TÍNH NĂNG ĐANG ĐƯỢC PHÁT TRIỂN] Choáng váng
    BLEED,         // [TÍNH NĂNG ĐANG ĐƯỢC PHÁT TRIỂN] Chảy máu
    DEFENSE_BUFF   // [TÍNH NĂNG ĐANG ĐƯỢC PHÁT TRIỂN] Tăng cường phòng thủ
};

struct StatusEffect {
    StatusType type = StatusType::NONE;
    std::string name;
    int duration = 0; // Number of turns remaining
    int value = 0;    // Damage or healing amount per turn
};
