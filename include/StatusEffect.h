#pragma once

#include <string>

enum class StatusType {
    NONE,
    POISON,        // Damage over time (Độc rút máu mỗi lượt)
    REGENERATION   // Heal over time (Hồi máu mỗi lượt)
};

struct StatusEffect {
    StatusType type = StatusType::NONE;
    std::string name;
    int duration = 0; // Number of turns remaining
    int value = 0;    // Damage or healing amount per turn
};
