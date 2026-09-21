/**
 * @file play_battle_2d.cpp
 * @brief Chương trình trải nghiệm Chiến Đấu 2D Pixel Art tương tác (Step 4)
 */

#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include "ui_2d/BattleScene2D.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "BossMonster.h"
#include "Minion.h"
#include "Potion.h"
#include <iostream>

int main() {
    // 1. Tạo Hero và Boss/Quái vật
    Warrior hero("Arthur", 150, 40, 25, 20); // HP 150, MP 40, ATK 25, DEF 20
    hero.getInventory().addItem(std::make_shared<Potion>("potion_hp", "Health Potion", "Hồi 50 HP", 50, false, 2));
    hero.getInventory().addItem(std::make_shared<Potion>("potion_mp", "Mana Potion", "Hồi 30 MP", 30, true, 2));

    BossMonster boss("ancient_dragon", "Ancient Dragon", 220, 28, 12, 100, 150);

    // 2. Khởi tạo Canvas ảo 480x270 scale 3x lên 1440x810
    PixelCanvas canvas(480, 270, 3);
    if (!canvas.init("RPG Engine - 2D Pixel Battle Arena (Step 4)")) {
        return 1;
    }

    AssetManager& assets = AssetManager::getInstance();
    assets.init();

    BattleScene2D battle(hero, boss);

    while (canvas.isRunning()) {
        float dt = GetFrameTime();
        Vector2 vMouse = canvas.getVirtualMousePosition();

        battle.update(dt, vMouse);

        canvas.beginCanvas();
        battle.render();

        // Gợi ý điều khiển
        DrawText("[1-5 / Chuột]: Chọn Hành Động | [ESC]: Thoát", 30, 260, 7, LIGHTGRAY);
        canvas.endCanvas();

        canvas.renderToScreen();

        if (battle.isFinished()) {
            break;
        }
    }

    assets.cleanup();
    canvas.close();
    return 0;
}
