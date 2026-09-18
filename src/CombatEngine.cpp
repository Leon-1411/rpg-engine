/**
 * @file CombatEngine.cpp
 * @brief Implement CombatEngine with Turn-based battle loop, status effects, and victory rewards.
 * @author Leon & Antigravity
 */

#include "CombatEngine.h"
#include "DataLoader.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

CombatEngine::CombatEngine(Hero& hero, Enemy& enemy, Inventory* inv)
    : hero(hero), enemy(enemy), inventory(inv), currentState(CombatState::ONGOING),
      turnCount(1), consecutiveZeroDamageTurns(0) {}

void CombatEngine::setInventory(Inventory* inv) {
    inventory = inv;
}

void CombatEngine::startBattle() {
    currentState = CombatState::ONGOING;
    turnCount = 1;
    consecutiveZeroDamageTurns = 0;
    hero.resetCombatStances();
    enemy.clearStatusEffects();
}

DamageResult CombatEngine::calculateDamage(int baseAttack, float critChance, float critDamage,
                                           int armorPen, int defenderDefense, bool ignoreArmor) const {
    DamageResult result;
    result.damage = 0;
    result.isCrit = false;

    // 1. Calculate effective defense considering Armor Penetration or Ignore Armor trait
    int effectiveDefense = std::max(0, defenderDefense);
    if (ignoreArmor) {
        effectiveDefense = 0;
    } else {
        effectiveDefense = std::max(0, effectiveDefense - armorPen);
    }

    int rawDamage = std::max(1, baseAttack - effectiveDefense);

    // 2. Check critical strike
    if (critChance > 0.0f) {
        float roll = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        if (roll <= critChance) {
            result.isCrit = true;
            rawDamage = static_cast<int>(rawDamage * (1.0f + critDamage));
        }
    }

    result.damage = std::max(1, rawDamage);
    return result;
}

int CombatEngine::calculateDamage(int attackerAtk, int defenderDef) const {
    DamageResult res = calculateDamage(attackerAtk, 0.0f, 0.0f, 0, defenderDef, false);
    return res.damage;
}

void CombatEngine::processStatusEffects() {
    // 1. Hero Poison DoT
    if (hero.isPoisoned()) {
        int poisonDmg = hero.takePoisonDamage();
        std::cout << "[HERO POISON] " << hero.getName() << " suffers "
                  << poisonDmg << " poison damage! (" << hero.getPoisonTurns()
                  << " turns remaining, HP: " << hero.getHp() << "/" << hero.getMaxHp() << ")\n";
    }

    // 2. Hero Regeneration HoT
    if (hero.hasRegen()) {
        int healed = hero.processRegen();
        std::cout << "[HERO REGEN] " << hero.getName() << " heals "
                  << healed << " HP from Regeneration! (" << hero.getRegenTurns()
                  << " turns remaining, HP: " << hero.getHp() << "/" << hero.getMaxHp() << ")\n";
    }

    // 3. Enemy Poison DoT
    if (enemy.isPoisoned()) {
        int poisonDmg = enemy.takePoisonDamage();
        std::cout << "[ENEMY POISON] " << enemy.getName() << " takes "
                  << poisonDmg << " poison damage! (" << enemy.getPoisonTurns()
                  << " turns remaining, HP: " << enemy.getHp() << "/" << enemy.getMaxHp() << ")\n";
    }

    // 4. Enemy Regeneration HoT
    if (enemy.hasRegen()) {
        int healed = enemy.processRegen();
        std::cout << "[ENEMY REGEN] " << enemy.getName() << " regenerates "
                  << healed << " HP! (" << enemy.getRegenTurns()
                  << " turns remaining, HP: " << enemy.getHp() << "/" << enemy.getMaxHp() << ")\n";
    }
}

void CombatEngine::processVictoryRewards() {
    hero.addExp(enemy.getExpReward());
    hero.addGold(enemy.getGoldReward());
    lastLootDrops.clear();

    auto droppedIds = enemy.generateLootDrops();
    for (const auto& itmId : droppedIds) {
        auto itm = DataLoader::loadItemById("data/items.json", itmId);
        if (itm) {
            lastLootDrops.push_back(itm);
            Inventory* targetInv = inventory ? inventory : &hero.getInventory();
            if (targetInv->addItem(itm)) {
                std::cout << "[CHIẾN LỢI PHẨM] " << enemy.getName() << " rơi ra: " << itm->getName() << "!\n";
            } else {
                std::cout << "[TÚI ĐỒ ĐẦY] " << enemy.getName() << " rơi ra: " << itm->getName() << " nhưng túi đồ đã đầy!\n";
            }
        }
    }
}

const std::vector<std::shared_ptr<Item>>& CombatEngine::getLastLootDrops() const {
    return lastLootDrops;
}

CombatState CombatEngine::executeTurn(int actionChoice, int skillOrItemIndex) {
    if (isBattleOver()) {
        return currentState;
    }

    std::cout << "\n--- Lượt " << turnCount << " ---\n";

    // Process Status Effects (Poison DoT & Regeneration HoT) for both Hero and Enemy
    processStatusEffects();

    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " đã gục ngã vì độc tố!\n";
        processVictoryRewards();
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    if (!hero.isAlive()) {
        std::cout << hero.getName() << " đã kiệt sức trước độc tố trong trận đấu...\n";
        currentState = CombatState::ENEMY_VICTORY;
        return currentState;
    }

    int heroHpBefore = hero.getHp();
    int enemyHpBefore = enemy.getHp();

    // 1. Player action
    if (actionChoice == 1) { // Normal Attack
        DamageResult res = calculateDamage(hero.getEffectiveAttack(), hero.getCritChance(), hero.getCritDamage(),
                                           hero.getArmorPenetration(), enemy.getDefense(), hero.isIgnoreArmor());
        enemy.takeDamage(res.damage);
        if (res.isCrit) {
            std::cout << "[BẠO KÍCH / CRITICAL HIT!] ";
        }
        std::cout << hero.getName() << " tấn công " << enemy.getName() << " gây " << res.damage << " sát thương! ("
                  << enemy.getName() << " HP: " << enemy.getHp() << "/" << enemy.getMaxHp() << ")\n";

    } else if (actionChoice == 2) { // Skill
        int skillDmg = 0;
        std::string skillMsg;
        if (hero.useSkill(skillOrItemIndex, skillDmg, skillMsg)) {
            std::cout << skillMsg << "\n";

            if (skillDmg > 0) { // Damaging Skill
                DamageResult res = calculateDamage(skillDmg, hero.getCritChance(), hero.getCritDamage(),
                                                   hero.getArmorPenetration(), enemy.getDefense(), hero.isIgnoreArmor());
                enemy.takeDamage(res.damage);
                if (res.isCrit) {
                    std::cout << "[BẠO KÍCH / CRITICAL HIT!] ";
                }
                std::cout << "Kỹ năng gây " << res.damage << " sát thương lên " << enemy.getName() << "! ("
                          << enemy.getName() << " HP: " << enemy.getHp() << "/" << enemy.getMaxHp() << ")\n";

                // Warrior Shield Block applies Stun
                if (hero.getHeroClass() == HeroClass::WARRIOR && skillOrItemIndex == 2) {
                    enemy.applyStun(1);
                    std::cout << "[HIỆU ỨNG CHOÁNG] " << enemy.getName() << " bị choáng và sẽ mất lượt kế tiếp!\n";
                }

                // Ranger Poison Arrow applies Poison DoT (12 dmg/turn for 3 turns)
                if (hero.getHeroClass() == HeroClass::RANGER && skillOrItemIndex == 2) {
                    enemy.applyPoison(3, 12);
                    std::cout << "[HIỆU ỨNG ĐỘC TỐ] " << enemy.getName() << " bị nhiễm độc trong 3 lượt (12 dmg/lượt)!\n";
                }

                // Ranger Rain of Arrows applies Bleed DoT (8 dmg/turn for 2 turns)
                if (hero.getHeroClass() == HeroClass::RANGER && skillOrItemIndex == 3) {
                    enemy.applyPoison(2, 8);
                    std::cout << "[HIỆU ỨNG CHẢY MÁU] " << enemy.getName() << " bị rách vết thương chảy máu trong 2 lượt (8 dmg/lượt)!\n";
                }
            }
        } else {
            std::cout << "[THẤT BẠI] " << skillMsg << "\n";
            return currentState; // Allow re-action if skill was unavailable
        }

    } else if (actionChoice == 3) { // Item (Potion)
        Inventory* targetInv = inventory ? inventory : &hero.getInventory();
        if (!targetInv || targetInv->getItemCount() == 0) {
            std::cout << "[VẬT PHẨM] Túi đồ trống hoặc không có vật phẩm khả dụng!\n";
            return currentState;
        }
        if (skillOrItemIndex < 0 || skillOrItemIndex >= targetInv->getItemCount()) {
            std::cout << "[VẬT PHẨM] Vị trí vật phẩm không hợp lệ (" << skillOrItemIndex + 1 << ")!\n";
            return currentState;
        }
        Item item = targetInv->getItem(skillOrItemIndex);
        if (item.getType() != ItemType::POTION) {
            std::cout << "[VẬT PHẨM] " << item.getName() << " không phải là Dược phẩm có thể dùng trong chiến đấu!\n";
            return currentState;
        }
        int beforeHp = hero.getHp();
        int beforeMp = hero.getMp();
        if (targetInv->useItem(skillOrItemIndex, hero)) {
            int healedHp = hero.getHp() - beforeHp;
            int restoredMp = hero.getMp() - beforeMp;
            std::cout << hero.getName() << " đã sử dụng " << item.getName();
            if (healedHp > 0) std::cout << ", hồi phục " << healedHp << " HP (Hiện tại: " << hero.getHp() << "/" << hero.getMaxHp() << ")";
            if (restoredMp > 0) std::cout << ", hồi phục " << restoredMp << " MP (Hiện tại: " << hero.getMp() << "/" << hero.getMaxMp() << ")";
            std::cout << "!\n";
        } else {
            std::cout << "[VẬT PHẨM] Không thể sử dụng vật phẩm lúc này!\n";
            return currentState;
        }

    } else if (actionChoice == 4) { // Defend
        hero.setIsDefending(true);
        std::cout << hero.getName() << " vào tư thế phòng thủ vững chắc, giảm 50% sát thương nhận vào trong lượt kế tiếp!\n";

    } else if (actionChoice == 5) { // Flee
        std::cout << hero.getName() << " đã rút lui an toàn khỏi trận chiến!\n";
        currentState = CombatState::FLED;
        return currentState;
    } else {
        std::cout << "[LỰA CHỌN KHÔNG HỢP LỆ] Vui lòng chọn từ 1 đến 5.\n";
        return currentState;
    }

    // Check if enemy defeated after player action
    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " đã bị đánh bại!\n";
        processVictoryRewards();
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    // 2. Enemy Turn
    processEnemyTurn();

    // Check if hero or enemy defeated
    if (!enemy.isAlive()) {
        std::cout << enemy.getName() << " đã bị đánh bại!\n";
        processVictoryRewards();
        currentState = CombatState::HERO_VICTORY;
        return currentState;
    }

    if (!hero.isAlive()) {
        std::cout << hero.getName() << " đã gục ngã trong trận chiến...\n";
        currentState = CombatState::ENEMY_VICTORY;
        return currentState;
    }

    // Check if both sides dealt 0 damage this round (stalemate / stall check)
    if (hero.getHp() == heroHpBefore && enemy.getHp() == enemyHpBefore) {
        consecutiveZeroDamageTurns++;
        if (consecutiveZeroDamageTurns >= 5) {
            int fatigueDamage = std::max(5, (consecutiveZeroDamageTurns - 4) * 3);
            std::cout << "\n[FATIGUE / STALEMATE] Cả hai bên đã không gây sát thương trong "
                      << consecutiveZeroDamageTurns << " lượt liên tiếp! Áp lực chiến trường gây "
                      << fatigueDamage << " sát thương kiệt sức lên cả hai bên!\n";
            hero.takeDamage(fatigueDamage);
            enemy.takeDamage(fatigueDamage);

            if (!enemy.isAlive() && !hero.isAlive()) {
                if (hero.getMaxHp() >= enemy.getMaxHp()) {
                    processVictoryRewards();
                    currentState = CombatState::HERO_VICTORY;
                } else {
                    currentState = CombatState::ENEMY_VICTORY;
                }
                return currentState;
            } else if (!enemy.isAlive()) {
                processVictoryRewards();
                currentState = CombatState::HERO_VICTORY;
                return currentState;
            } else if (!hero.isAlive()) {
                currentState = CombatState::ENEMY_VICTORY;
                return currentState;
            }
        }
    } else {
        consecutiveZeroDamageTurns = 0;
    }

    // End of full round: reduce skill cooldowns and increment turn
    hero.reduceCooldowns();
    turnCount++;

    // Max turns cap check (Anti-Infinite Loop)
    if (turnCount > MAX_BATTLE_TURNS) {
        std::cout << "\n[BATTLE TIMEOUT] Trận chiến đã vượt quá giới hạn " << MAX_BATTLE_TURNS
                  << " lượt! Phân định kết quả dựa trên tỷ lệ % HP còn lại.\n";
        float heroPct = static_cast<float>(hero.getHp()) / hero.getMaxHp();
        float enemyPct = static_cast<float>(enemy.getHp()) / enemy.getMaxHp();
        if (heroPct >= enemyPct) {
            std::cout << hero.getName() << " kiên cường sinh tồn và giành chiến thắng theo % HP!\n";
            processVictoryRewards();
            currentState = CombatState::HERO_VICTORY;
        } else {
            std::cout << enemy.getName() << " áp đảo sinh lực. Bạn đã thất bại!\n";
            currentState = CombatState::ENEMY_VICTORY;
        }
        return currentState;
    }

    return currentState;
}

void CombatEngine::processEnemyTurn() {
    std::cout << "\n[Lượt Kẻ Địch - " << enemy.getName() << "]\n";
    if (!enemy.isAlive()) return;

    if (enemy.isStunned()) {
        enemy.takeStunTurn();
        std::cout << "[CHOÁNG / STUN] " << enemy.getName() << " đang bị choáng và mất lượt hành động trong hiệp này!\n";
        hero.resetCombatStances();
        return;
    }

    int enemyAction = enemy.chooseAction(hero.getHeroClass(), turnCount);

    if (enemyAction == 0) { // Enemy Misses or Hesitates
        std::cout << enemy.getName() << " chần chừ và tung đòn trượt!\n";

        // Warrior Parry penalty: mistimed parry locks skill for 1 turn
        if (hero.getIsParrying()) {
            hero.lockSkills(1);
            std::cout << "[PHẢN ĐÒN HỤT] " << hero.getName()
                      << " phán đoán sai lầm đòn đánh! Kỹ năng bị khóa trong 1 lượt.\n";
        }
        hero.resetCombatStances();

    } else { // Enemy Attacks (Action 1 = Normal, Action 2 = Heavy, Action 3 = Special)
        if (hero.getIsParrying()) {
            // Warrior Parry Success: counterattack
            DamageResult counterRes = calculateDamage(static_cast<int>(hero.getEffectiveAttack() * 1.5f),
                                                     hero.getCritChance(), hero.getCritDamage(),
                                                     hero.getArmorPenetration(), enemy.getDefense(), hero.isIgnoreArmor());
            enemy.takeDamage(counterRes.damage);
            std::cout << "[PHẢN ĐÒN THÀNH CÔNG!] " << hero.getName() << " gạt phăng đòn đánh và phản kích gây "
                      << counterRes.damage << " sát thương!\n";

        } else if (hero.getIsBlocking()) {
            // Warrior Block Success: 0 damage
            std::cout << "[ĐỠ ĐÒN THÀNH CÔNG!] " << hero.getName()
                      << " giương khiên kiên cố, chặn đứng 100% sát thương nhận vào (0 damage)!\n";

        } else if (hero.getIsEvading()) {
            // Ranger Evade Success: dodge 100% and reload 5 ready arrows
            hero.addReadyArrows(5);
            std::cout << "[NÉ TRÁNH HOÀN HẢO!] " << hero.getName()
                      << " thoăn thoắt né tránh đòn đánh của " << enemy.getName()
                      << " (0 damage) và nạp thêm +5 Mũi tên! (Hiện có: "
                      << hero.getReadyArrows() << ")\n";

        } else {
            // Calculate base enemy attack power based on action type
            int baseAtk = enemy.getAttack();
            int pen = enemy.getArmorPenetration();

            if (enemyAction == 2) {
                baseAtk = static_cast<int>(baseAtk * 1.5f);
                std::cout << "[ĐÒN ĐÁNH CỰC MẠNH!] " << enemy.getName() << " tung đòn giáng búa ngàn cân!\n";
            } else if (enemyAction == 3) {
                baseAtk = static_cast<int>(baseAtk * 1.2f);
                pen += 10; // Dark surge has increased armor penetration
                std::cout << "[KỸ NĂNG ĐẶC BIỆT CỦA BOSS!] " << enemy.getName() << " triệu hồi ma thuật hắc ám xuyên giáp!\n";
            }

            DamageResult res = calculateDamage(baseAtk, enemy.getCritChance(), enemy.getCritDamage(),
                                               pen, hero.getEffectiveDefense(), false);

            // Defend stance reduces damage by 50%
            if (hero.getIsDefending()) {
                int originalDamage = res.damage;
                res.damage = std::max(1, res.damage / 2);
                std::cout << "[THẾ THỦ GIẢM SÁT THƯƠNG] " << hero.getName() << " phòng ngự vững vàng! Giảm sát thương từ "
                          << originalDamage << " xuống " << res.damage << ".\n";
            }

            hero.takeDirectDamage(res.damage);
            if (res.isCrit) {
                std::cout << "[BẠO KÍCH / CRITICAL HIT!] ";
            }
            std::cout << enemy.getName() << " tấn công " << hero.getName() << " gây " << res.damage << " sát thương! ("
                      << hero.getName() << " HP: " << hero.getHp() << "/" << hero.getMaxHp() << ")\n";

            // Poisonous Enemy trait: applies poison to Hero on successful hit
            if (enemy.getIsPoisonous() && hero.isAlive()) {
                hero.applyPoison(enemy.getPoisonInflictTurns(), enemy.getPoisonInflictDmg());
                std::cout << "[NHIỄM ĐỘC TỐ] " << enemy.getName() << " găm nọc độc vào người! "
                          << hero.getName() << " bị trúng độc trong " << enemy.getPoisonInflictTurns()
                          << " lượt (" << enemy.getPoisonInflictDmg() << " dmg/lượt)!\n";
            }
        }

        hero.resetCombatStances();
    }
}

bool CombatEngine::isBattleOver() const {
    return currentState != CombatState::ONGOING;
}

CombatState CombatEngine::getState() const {
    return currentState;
}

int CombatEngine::getTurnCount() const {
    return turnCount;
}

std::string CombatEngine::renderBar(int current, int max, int length) {
    if (max <= 0) max = 1;
    current = std::min(max, std::max(0, current));
    int filled = (current * length) / max;
    std::string bar = "[";
    for (int i = 0; i < length; ++i) {
        if (i < filled) bar += "=";
        else bar += " ";
    }
    bar += "] " + std::to_string(current) + "/" + std::to_string(max);
    return bar;
}

void CombatEngine::displayBattleStatus(std::ostream& out) const {
    out << "\n=======================================================\n";
    out << "  " << hero.getName() << " [" << hero.getHeroClassName() << " Lv." << hero.getLevel() << "]"
        << "  vs  " << enemy.getName() << "\n";
    out << "-------------------------------------------------------\n";
    out << "  Hero  HP: " << renderBar(hero.getHp(), hero.getMaxHp(), 15)
        << " | MP: " << renderBar(hero.getMp(), hero.getMaxMp(), 10) << "\n";
    out << "  Enemy HP: " << renderBar(enemy.getHp(), enemy.getMaxHp(), 15) << "\n";

    if (hero.isPoisoned()) {
        out << "  * Hero Poison: " << hero.getPoisonTurns() << " turns (" << hero.getPoisonDamagePerTurn() << " dmg/t)\n";
    }
    if (hero.hasRegen()) {
        out << "  * Hero Regen: " << hero.getRegenTurns() << " turns (+" << hero.getRegenPerTurn() << " HP/t)\n";
    }
    if (enemy.isPoisoned()) {
        out << "  * Enemy Poison: " << enemy.getPoisonTurns() << " turns (" << enemy.getPoisonDamagePerTurn() << " dmg/t)\n";
    }
    if (enemy.hasRegen()) {
        out << "  * Enemy Regen: " << enemy.getRegenTurns() << " turns (+" << enemy.getRegenPerTurn() << " HP/t)\n";
    }
    out << "=======================================================\n";
}

void CombatEngine::runInteractiveBattle(std::istream& in, std::ostream& out) {
    runBattleLoop(in, out);
}

void CombatEngine::runBattleLoop(std::istream& in, std::ostream& out) {
    startBattle();
    out << "\nTrận chiến bắt đầu giữa " << hero.getName() << " và " << enemy.getName() << "!\n";

    while (!isBattleOver()) {
        displayBattleStatus(out);

        out << "\nChọn hành động:\n";
        out << " 1. Đánh thường (Normal Attack)\n";
        out << " 2. Kỹ năng (Skill)\n";
        out << " 3. Dược phẩm (Item / Potion)\n";
        out << " 4. Phòng thủ (Defend - Giảm 50% sát thương)\n";
        out << " 5. Bỏ chạy (Flee)\n";
        out << "Lựa chọn của bạn (1-5): ";

        int choice = 0;
        if (!(in >> choice)) {
            break;
        }

        int subIndex = -1;
        if (choice == 2) {
            out << "Danh sách kỹ năng của " << hero.getName() << ":\n";
            hero.displaySkills();
            out << "Chọn kỹ năng (1-3): ";
            in >> subIndex;
        } else if (choice == 3) {
            Inventory* targetInv = inventory ? inventory : &hero.getInventory();
            if (!targetInv || targetInv->getItemCount() == 0) {
                out << "Túi đồ của bạn đang trống!\n";
            } else {
                out << "Các bình thuốc trong túi đồ:\n";
                std::vector<int> potionIndices;
                for (int i = 0; i < targetInv->getItemCount(); ++i) {
                    auto itemPtr = targetInv->getItemPtr(i);
                    if (itemPtr && itemPtr->getType() == ItemType::POTION) {
                        potionIndices.push_back(i);
                        auto potPtr = std::dynamic_pointer_cast<Potion>(itemPtr);
                        bool isMana = potPtr ? potPtr->isMana() : false;
                        out << " " << potionIndices.size() << ". " << itemPtr->getName()
                            << " (" << (isMana ? "Hồi MP +" : "Hồi HP +")
                            << itemPtr->getStatValue() << ")\n";
                    }
                }
                if (potionIndices.empty()) {
                    out << "Không có bình thuốc nào trong túi đồ!\n";
                } else {
                    out << "Chọn bình thuốc (1-" << potionIndices.size() << "): ";
                    int pSelect = 0;
                    in >> pSelect;
                    if (pSelect >= 1 && pSelect <= static_cast<int>(potionIndices.size())) {
                        subIndex = potionIndices[pSelect - 1];
                    }
                }
            }
        }

        executeTurn(choice, subIndex);
    }

    out << "\n=======================================================\n";
    if (currentState == CombatState::HERO_VICTORY) {
        out << "  CHIẾN THẮNG! " << hero.getName() << " đã đánh bại " << enemy.getName() << "!\n";
        out << "  Phần thưởng: +" << enemy.getExpReward() << " EXP, +" << enemy.getGoldReward() << " Vàng!\n";
    } else if (currentState == CombatState::ENEMY_VICTORY) {
        out << "  THẤT BẠI! " << hero.getName() << " đã ngã xuống nơi chiến trường...\n";
    } else if (currentState == CombatState::FLED) {
        out << "  " << hero.getName() << " đã tẩu thoát an toàn khỏi trận đấu.\n";
    }
    out << "=======================================================\n";
}
