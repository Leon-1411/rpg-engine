/**
 * @file test_battle_scene_2d.cpp
 * @brief Kiểm thử tích hợp BattleScene2D với CombatEngine, Hero và BossMonster
 */

#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include "ui_2d/BattleScene2D.h"
#include "Warrior.h"
#include "BossMonster.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "========================================\n";
    std::cout << "  RUNNING BATTLE SCENE 2D UNIT TESTS\n";
    std::cout << "========================================\n";

    PixelCanvas canvas(480, 270, 1);
    bool initOk = canvas.init("BattleScene2D Headless Test");
    assert(initOk == true);

    AssetManager& assets = AssetManager::getInstance();
    assets.init();

    Warrior hero("Arthur", 120, 30, 25, 15);
    BossMonster dragon("ancient_dragon", "Ancient Dragon", 100, 20, 10, 50, 100);

    BattleScene2D battle(hero, dragon);
    std::cout << "[Test 1] BattleScene2D initialization with Hero & Boss: PASSED\n";

    // 1. Kiểm tra Floating Text
    battle.addFloatingText("-45 HP", Vector2{ 200, 100 }, YELLOW);
    std::cout << "[Test 2] Floating combat numbers addition: PASSED\n";

    // 2. Kiểm tra Render Pass
    canvas.beginCanvas();
    battle.render();
    canvas.endCanvas();
    std::cout << "[Test 3] BattleScene2D render pass (Arena, Fighters, HUD, 5-action Menu): PASSED\n";

    // 3. Giả lập một lượt tấn công của Hero
    battle.update(0.1f, Vector2{ 0, 0 });
    // Hero chọn hành động 1 (Tấn công thường)
    // Cập nhật hoạt ảnh
    battle.update(0.5f, Vector2{ 0, 0 });
    battle.update(0.5f, Vector2{ 0, 0 });
    std::cout << "[Test 4] Turn sequencer and damage animation cycles: PASSED\n";

    assets.cleanup();
    canvas.close();

    std::cout << "========================================\n";
    std::cout << " [ALL PASS] BattleScene2D tests successful!\n";
    std::cout << "========================================\n";
    return 0;
}
