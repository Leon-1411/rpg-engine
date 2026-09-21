/**
 * @file test_asset_manager.cpp
 * @brief Kiểm thử tự động hệ thống quản lý tài nguyên AssetManager
 */

#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "========================================\n";
    std::cout << "  RUNNING ASSET MANAGER UNIT TESTS\n";
    std::cout << "========================================\n";

    // Khởi tạo Canvas (Bắt buộc để OpenGL load texture)
    PixelCanvas canvas(480, 270, 1);
    bool initOk = canvas.init("Asset Test Headless");
    assert(initOk == true);

    AssetManager& assets = AssetManager::getInstance();
    assets.init();
    std::cout << "[Test 1] AssetManager initialization and texture generation: PASSED\n";

    // 1. Kiểm tra Hero Textures
    Texture2D warrior = assets.getHeroTexture("Warrior");
    Texture2D mage = assets.getHeroTexture("Mage");
    Texture2D ranger = assets.getHeroTexture("Ranger");
    assert(warrior.width == 24 && warrior.height == 24);
    assert(mage.width == 24 && mage.height == 24);
    assert(ranger.width == 24 && ranger.height == 24);
    std::cout << "[Test 2] Hero class textures (Warrior, Mage, Ranger): PASSED\n";

    // 2. Kiểm tra Monster & Boss Textures
    Texture2D goblin = assets.getEnemyTexture("goblin");
    Texture2D orc = assets.getEnemyTexture("armored_orc");
    Texture2D dragon = assets.getEnemyTexture("ancient_dragon");
    Texture2D shadow = assets.getEnemyTexture("shadow_lord");
    assert(goblin.width == 24 && goblin.height == 24);
    assert(orc.width == 32 && orc.height == 32);
    assert(dragon.width == 48 && dragon.height == 48);
    assert(shadow.width == 32 && shadow.height == 32);
    std::cout << "[Test 3] Enemy and Boss textures (Goblin, Orc, Dragon 48x48, Shadow Lord): PASSED\n";

    // 3. Kiểm tra Item Icon Textures
    Texture2D hpPot = assets.getItemIcon("potion_hp");
    Texture2D mpPot = assets.getItemIcon("potion_mp");
    Texture2D sword = assets.getItemIcon("sword");
    Texture2D gold = assets.getItemIcon("gold");
    Texture2D badge = assets.getItemIcon("badge");
    assert(hpPot.width == 16 && hpPot.height == 16);
    assert(mpPot.width == 16 && mpPot.height == 16);
    assert(sword.width == 16 && sword.height == 16);
    assert(gold.width == 16 && gold.height == 16);
    assert(badge.width == 16 && badge.height == 16);
    std::cout << "[Test 4] Item icon 16x16 textures (Potions, Weapons, Gold, Crest): PASSED\n";

    // 4. Test render pass
    canvas.beginCanvas();
    assets.drawNineSliceBox(Rectangle{ 10, 10, 200, 100 });
    assets.drawStatBar(20, 30, 100, 10, 50, 100, RED);
    assets.drawEntitySprite(warrior, 20, 50);
    assets.drawItemIcon("sword", 60, 50);
    canvas.endCanvas();
    std::cout << "[Test 5] UI Boxes, Stat Bars and Sprite drawing pass: PASSED\n";

    assets.cleanup();
    canvas.close();
    std::cout << "[Test 6] AssetManager clean cleanup and unload: PASSED\n";

    std::cout << "========================================\n";
    std::cout << " [ALL PASS] AssetManager tests successful!\n";
    std::cout << "========================================\n";
    return 0;
}
