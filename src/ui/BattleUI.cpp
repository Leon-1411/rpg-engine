#include "ui/BattleUI.h"
#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include "CombatEngine.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

static std::string heroClassToString(HeroClass c) {
    switch (c) {
        case HeroClass::WARRIOR: return "Chiến Binh (Warrior)";
        case HeroClass::MAGE:    return "Pháp Sư (Mage)";
        case HeroClass::RANGER:  return "Xạ Thủ (Ranger)";
        default:                 return "Hiệp Sĩ";
    }
}

void BattleUI::renderBattleScreen(const Hero& hero, const Enemy& enemy, const std::string& battleMessage) {
    ConsoleUI::clearScreen();
    ASCIIArt::printBattleBanner();

    // Display Hero, VS, and Enemy/Boss ASCII Art
    ASCIIArt::printBattleVersus(hero, enemy);

    ConsoleUI::printDivider('=', 64, ConsoleUI::Colors::BRIGHT_RED);

    // Hero Status (Left side) & Enemy Status (Right side)
    std::string heroHeader = hero.getName() + " (" + heroClassToString(hero.getHeroClass()) + " Lv." + std::to_string(hero.getLevel()) + ")";
    std::string enemyHeader = enemy.getName() + (enemy.getType() == EnemyType::BOSS ? " [BOSS TỐI CAO]" : " [Minion]");

    std::cout << "  " 
              << ConsoleUI::colorize(heroHeader, ConsoleUI::Colors::BRIGHT_GREEN)
              << std::string(std::max(2, 34 - static_cast<int>(heroHeader.length())), ' ')
              << ConsoleUI::colorize(enemyHeader, ConsoleUI::Colors::BRIGHT_RED)
              << "\n";

    // HP Bars: Rendered in BRIGHT_RED for both Hero and Enemy/Boss
    std::string heroHpBar = ConsoleUI::formatProgressBar(hero.getHp(), hero.getMaxHp(), 14, ConsoleUI::Colors::BRIGHT_RED);
    std::string enemyHpBar = ConsoleUI::formatProgressBar(enemy.getHp(), enemy.getMaxHp(), 14, ConsoleUI::Colors::BRIGHT_RED);

    std::cout << "  HP: " << heroHpBar
              << std::string(std::max(2, 30 - 24), ' ')
              << "HP: " << enemyHpBar << "\n";

    // MP Bar: Rendered in BRIGHT_BLUE for Hero
    std::string heroMpBar = ConsoleUI::formatProgressBar(hero.getMp(), hero.getMaxMp(), 14, ConsoleUI::Colors::BRIGHT_BLUE);
    std::cout << "  MP: " << heroMpBar << "\n";

    // Hero Stats & Equipment Bonuses
    const auto& inv = hero.getInventory();
    int wpnBonus = inv.getEquippedWeaponBonus();
    int armBonus = inv.getEquippedArmorBonus();
    std::string atkStr = std::to_string(hero.getAttack()) + (wpnBonus > 0 ? " (+" + std::to_string(wpnBonus) + ")" : "");
    std::string defStr = std::to_string(hero.getDefense()) + (armBonus > 0 ? " (+" + std::to_string(armBonus) + ")" : "");
    std::cout << "  ⚔ ATK: " << ConsoleUI::colorize(atkStr, ConsoleUI::Colors::BRIGHT_YELLOW)
              << "   🛡 DEF: " << ConsoleUI::colorize(defStr, ConsoleUI::Colors::CYAN) << "\n";

    // Real-time Inventory Status (Potion counts, Capacity, Gold)
    int hpPots = 0;
    int mpPots = 0;
    for (int i = 0; i < inv.getItemCount(); ++i) {
        auto itm = inv.getItemPtr(i);
        if (itm && itm->getType() == ItemType::POTION) {
            auto pot = std::dynamic_pointer_cast<Potion>(itm);
            if (pot) {
                if (pot->isMana()) mpPots += pot->getQuantity();
                else hpPots += pot->getQuantity();
            }
        }
    }
    std::string invSummary = "🎒 Túi đồ: [HP Potion x" + std::to_string(hpPots) + "] [MP Potion x" + std::to_string(mpPots) + "] | "
                           + std::to_string(inv.getItemCount()) + "/" + std::to_string(inv.getCapacity()) + " ô | "
                           + std::to_string(hero.getGold()) + " Vàng";
    std::cout << "  " << ConsoleUI::colorize(invSummary, ConsoleUI::Colors::BRIGHT_MAGENTA) << "\n";

    ConsoleUI::printDivider('-', 64, ConsoleUI::Colors::DIM);

    // Event/Log message
    if (!battleMessage.empty()) {
        std::cout << "\n  " << ConsoleUI::colorize("▶ ", ConsoleUI::Colors::BRIGHT_YELLOW)
                  << battleMessage << "\n\n";
        ConsoleUI::printDivider('-', 64, ConsoleUI::Colors::DIM);
    }

    // Battle Actions Menu
    std::cout << "\n  " << ConsoleUI::colorize("1. Tấn công (Attack)", ConsoleUI::Colors::BRIGHT_RED)
              << "    " << ConsoleUI::colorize("2. Kỹ năng (Skill)", ConsoleUI::Colors::BRIGHT_BLUE)
              << "    " << ConsoleUI::colorize("3. Dược phẩm (Item)", ConsoleUI::Colors::BRIGHT_YELLOW) << "\n"
              << "  " << ConsoleUI::colorize("4. Phòng thủ (Defend)", ConsoleUI::Colors::CYAN)
              << "   " << ConsoleUI::colorize("5. Bỏ chạy (Run)", ConsoleUI::Colors::DIM) << "\n\n";
}

BattleAction BattleUI::getPlayerAction(std::istream& in) {
    int choice = ConsoleUI::getIntInput(1, 5, "Chọn hành động chiến đấu [1-5]: ", in);
    return static_cast<BattleAction>(choice);
}

CombatState BattleUI::runBattle(Hero& hero, Enemy& enemy, std::istream& in) {
    CombatEngine engine(hero, enemy);
    engine.startBattle();

    std::string lastMessage = "Một kẻ địch đã xuất hiện: " + enemy.getName() + "!";
    if (enemy.getType() == EnemyType::BOSS) {
        lastMessage = "CẢNH BÁO NGUY HIỂM: TRẬN CHIẾN BOSS TỐI CAO - " + enemy.getName() + " BẮT ĐẦU!";
    }

    while (!engine.isBattleOver()) {
        renderBattleScreen(hero, enemy, lastMessage);
        BattleAction action = getPlayerAction(in);

        int actionCode = static_cast<int>(action);
        int itemOrSkillIndex = -1;

        if (action == BattleAction::SKILL) {
            itemOrSkillIndex = 1;
        } else if (action == BattleAction::ITEM) {
            auto& inv = hero.getInventory();
            std::vector<int> potionIndices;
            for (int i = 0; i < inv.getItemCount(); ++i) {
                auto itm = inv.getItemPtr(i);
                if (itm && itm->getType() == ItemType::POTION) {
                    potionIndices.push_back(i);
                }
            }

            if (potionIndices.empty()) {
                lastMessage = "Túi đồ không còn bình thuốc (Potion) nào để sử dụng!";
                continue;
            }

            if (potionIndices.size() == 1) {
                itemOrSkillIndex = potionIndices[0];
            } else {
                std::cout << "\n" << ConsoleUI::colorize("Danh sách thuốc trong túi:", ConsoleUI::Colors::BRIGHT_YELLOW) << "\n";
                for (size_t p = 0; p < potionIndices.size(); ++p) {
                    auto itm = inv.getItemPtr(potionIndices[p]);
                    auto pot = std::dynamic_pointer_cast<Potion>(itm);
                    std::string qtyStr = pot ? (" (x" + std::to_string(pot->getQuantity()) + ")") : "";
                    std::string statDesc = (pot && pot->isMana()) ? ("Hồi " + std::to_string(itm->getStatValue()) + " MP") : ("Hồi " + std::to_string(itm->getStatValue()) + " HP");
                    std::cout << "  " << (p + 1) << ". " << itm->getName() << qtyStr << " [" << statDesc << "]\n";
                }
                std::cout << "  0. Quay lại\n";
                int pChoice = ConsoleUI::getIntInput(0, static_cast<int>(potionIndices.size()), "Chọn bình thuốc muốn dùng: ", in);
                if (pChoice == 0) {
                    continue;
                }
                itemOrSkillIndex = potionIndices[pChoice - 1];
            }
        }

        engine.executeTurn(actionCode, itemOrSkillIndex);
        lastMessage = "Lượt đấu vừa diễn ra!";
    }

    if (engine.getState() == CombatState::HERO_VICTORY) {
        renderBattleScreen(hero, enemy, enemy.getName() + " đã bị tiêu diệt hoàn toàn!");
        showVictory(enemy, engine.getLastLootDrops(), in);
        return CombatState::HERO_VICTORY;
    } else if (engine.getState() == CombatState::ENEMY_VICTORY) {
        renderBattleScreen(hero, enemy, hero.getName() + " đã kiệt sức và ngã xuống trên chiến trường...");
        showDefeat(in);
        return CombatState::ENEMY_VICTORY;
    } else {
        renderBattleScreen(hero, enemy, hero.getName() + " đã nhanh nhẹn rút lui an toàn khỏi trận chiến!");
        ConsoleUI::pause("Nhấn Enter để tiếp tục...", in);
        return CombatState::FLED;
    }
}

void BattleUI::printCombatLog(const std::string& message) {
    std::cout << "  " << ConsoleUI::colorize("» ", ConsoleUI::Colors::CYAN) << message << "\n";
}

void BattleUI::showVictory(const Enemy& enemy, const std::vector<std::shared_ptr<Item>>& lootDrops, std::istream& in) {
    std::cout << "\n";
    ASCIIArt::printVictoryBanner();
    std::cout << "  " << ConsoleUI::colorize("+ Nhận được: ", ConsoleUI::Colors::BRIGHT_YELLOW)
              << ConsoleUI::colorize(std::to_string(enemy.getExpReward()) + " EXP", ConsoleUI::Colors::BRIGHT_GREEN)
              << " và "
              << ConsoleUI::colorize(std::to_string(enemy.getGoldReward()) + " Vàng", ConsoleUI::Colors::BRIGHT_YELLOW)
              << "!\n";

    if (!lootDrops.empty()) {
        std::cout << "\n  " << ConsoleUI::colorize("🎁 CHIẾN LỢI PHẨM RỚT RA (Loot Drops):", ConsoleUI::Colors::BRIGHT_CYAN) << "\n";
        for (const auto& item : lootDrops) {
            if (!item) continue;
            std::string typeBadge;
            std::string desc = item->getDescription();
            switch (item->getType()) {
                case ItemType::WEAPON:
                    typeBadge = "[Vũ khí - Weapon]";
                    break;
                case ItemType::ARMOR:
                    typeBadge = "[Giáp bảo hộ - Armor]";
                    break;
                case ItemType::POTION:
                    typeBadge = "[Dược phẩm - Potion]";
                    break;
                case ItemType::KEY_ITEM:
                    typeBadge = "[Vật phẩm nhiệm vụ - Key Item]";
                    break;
                default:
                    typeBadge = "[Vật phẩm]";
                    break;
            }
            std::cout << "    ✦ " << ConsoleUI::colorize(typeBadge, ConsoleUI::Colors::BRIGHT_YELLOW)
                      << " " << ConsoleUI::colorize(item->getName(), ConsoleUI::Colors::BRIGHT_WHITE)
                      << " (+" << item->getStatValue() << ")"
                      << " - " << desc << "\n";
        }
        std::cout << "  " << ConsoleUI::colorize("✔ Tất cả vật phẩm đã được tự động cất vào túi đồ (Inventory)!", ConsoleUI::Colors::BRIGHT_GREEN) << "\n";
    }
    std::cout << "\n";
    ConsoleUI::pause("Nhấn Enter để tiếp tục...", in);
}

void BattleUI::showDefeat(std::istream& in) {
    std::cout << "\n";
    ASCIIArt::printGameOverBanner();
    ConsoleUI::pause("Trò chơi kết thúc. Nhấn Enter để quay lại...", in);
}
