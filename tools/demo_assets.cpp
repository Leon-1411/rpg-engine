/**
 * @file demo_assets.cpp
 * @brief Chương trình kiểm thử và trưng bày toàn bộ Pixel Art Assets (Hero, Boss, Items, UI)
 */

#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include "raylib.h"
#include <cmath>

int main() {
    // Độ phân giải 480x270, tỉ lệ 16:9, scale 3x lên 1440x810
    PixelCanvas canvas(480, 270, 3);
    if (!canvas.init("RPG Engine - 2D Pixel Asset Gallery (Step 2)")) {
        return 1;
    }

    AssetManager& assets = AssetManager::getInstance();
    assets.init();

    float timer = 0.0f;
    int tab = 0; // 0: Heroes & Bosses, 1: Items & UI

    while (canvas.isRunning()) {
        float dt = GetFrameTime();
        timer += dt;

        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_SPACE)) {
            tab = (tab + 1) % 2;
        }

        bool hurtEffect = ((int)(timer * 3.0f) % 4 == 0);

        canvas.beginCanvas();
        ClearBackground(Color{ 16, 18, 32, 255 }); // Dark Fantasy Navy

        // Tiêu đề & Menu Tab
        assets.drawNineSliceBox(Rectangle{ 10, 8, 460, 32 }, Color{ 22, 26, 48, 240 }, GOLD);
        DrawText("ELDORIA 2D PIXEL ASSET GALLERY", 20, 14, 12, GOLD);
        DrawText("[SPACE / TAB]: Chuyen Tab | [ESC]: Thoat", 200, 24, 8, LIGHTGRAY);

        if (tab == 0) {
            // ================== TAB 1: HEROES & MONSTERS / BOSSES ==================
            DrawText("=== HERO CLASSES (24x24 Pixel Art) ===", 20, 48, 8, SKYBLUE);

            // 1. Warrior
            assets.drawNineSliceBox(Rectangle{ 20, 60, 90, 80 });
            Texture2D warriorTex = assets.getHeroTexture("Warrior");
            assets.drawEntitySprite(warriorTex, 45, 68, 2.0f);
            DrawText("WARRIOR", 40, 120, 8, YELLOW);

            // 2. Mage
            assets.drawNineSliceBox(Rectangle{ 120, 60, 90, 80 });
            Texture2D mageTex = assets.getHeroTexture("Mage");
            assets.drawEntitySprite(mageTex, 145, 68, 2.0f);
            DrawText("MAGE", 150, 120, 8, VIOLET);

            // 3. Ranger
            assets.drawNineSliceBox(Rectangle{ 220, 60, 90, 80 });
            Texture2D rangerTex = assets.getHeroTexture("Ranger");
            assets.drawEntitySprite(rangerTex, 245, 68, 2.0f);
            DrawText("RANGER", 245, 120, 8, GREEN);

            // 4. King NPC
            assets.drawNineSliceBox(Rectangle{ 320, 60, 90, 80 });
            Texture2D kingTex = assets.getEnemyTexture("king");
            assets.drawEntitySprite(kingTex, 345, 68, 2.0f);
            DrawText("KING", 355, 120, 8, GOLD);

            DrawText("=== ENEMIES & BOSSES (Goblin / Orc / Ancient Dragon / Shadow Lord) ===", 20, 148, 8, RED);

            // Goblin
            assets.drawNineSliceBox(Rectangle{ 20, 160, 75, 75 });
            Texture2D gobTex = assets.getEnemyTexture("goblin");
            assets.drawEntitySprite(gobTex, 38, 168, 2.0f);
            DrawText("Goblin", 42, 218, 8, LIME);

            // Armored Orc
            assets.drawNineSliceBox(Rectangle{ 105, 160, 85, 75 });
            Texture2D orcTex = assets.getEnemyTexture("armored_orc");
            assets.drawEntitySprite(orcTex, 120, 164, 1.8f);
            DrawText("Armored Orc", 115, 218, 8, ORANGE);

            // Shadow Lord
            assets.drawNineSliceBox(Rectangle{ 200, 160, 95, 75 });
            Texture2D shadowTex = assets.getEnemyTexture("shadow_lord");
            assets.drawEntitySprite(shadowTex, 218, 164, 1.8f);
            DrawText("Shadow Lord", 215, 218, 8, PURPLE);

            // Ancient Dragon (BOSS)
            assets.drawNineSliceBox(Rectangle{ 305, 148, 155, 95 }, Color{ 30, 15, 15, 240 }, RED);
            Texture2D dragonTex = assets.getEnemyTexture("ancient_dragon");
            assets.drawEntitySprite(dragonTex, 335, 152, 1.8f, false, hurtEffect);
            DrawText("★ ANCIENT DRAGON (BOSS) ★", 315, 230, 8, RED);
        }
        else {
            // ================== TAB 2: ITEMS, ICONS & UI BARS ==================
            DrawText("=== ITEM ICONS (16x16 Pixel Art Grid) ===", 20, 48, 8, GOLD);

            assets.drawNineSliceBox(Rectangle{ 20, 60, 440, 95 });

            std::string itemNames[] = { "potion_hp", "potion_mp", "potion_elixir", "sword", "staff", "bow", "armor", "gold", "badge", "book" };
            std::string labels[] = { "HP Potion", "MP Potion", "Elixir", "Claymore", "Staff", "Bow", "Armor", "Gold", "Badge", "Codex" };

            for (int i = 0; i < 10; ++i) {
                int col = i % 5;
                int row = i / 5;
                float ix = 35 + col * 85;
                float iy = 70 + row * 40;

                // Khung ô đồ
                DrawRectangle(ix - 4, iy - 4, 24, 24, Color{ 35, 40, 65, 255 });
                DrawRectangleLines(ix - 4, iy - 4, 24, 24, Color{ 90, 100, 140, 255 });
                assets.drawItemIcon(itemNames[i], ix, iy, 16.0f);
                DrawText(labels[i].c_str(), ix + 24, iy + 4, 8, RAYWHITE);
            }

            // Thanh Stat Bars HP / MP / EXP
            DrawText("=== DYNAMIC STAT BARS ===", 20, 165, 8, GREEN);
            assets.drawNineSliceBox(Rectangle{ 20, 178, 440, 75 });

            int hpVal = 75 + (int)(sin(timer * 2.0f) * 20.0f);
            int mpVal = 30 + (int)(cos(timer * 2.5f) * 10.0f);
            int expVal = 62;

            assets.drawStatBar(35, 195, 180, 12, hpVal, 100, Color{ 220, 40, 50, 255 }, Color{ 30, 30, 30, 255 }, "HP (Mau): 80/100");
            assets.drawStatBar(35, 222, 180, 12, mpVal, 40, Color{ 30, 120, 240, 255 }, Color{ 30, 30, 30, 255 }, "MP (Mana): 30/40");

            assets.drawStatBar(245, 195, 195, 12, expVal, 100, Color{ 230, 180, 30, 255 }, Color{ 30, 30, 30, 255 }, "EXP (Kinh Nghiem): Lv.5 [62%]");
            assets.drawStatBar(245, 222, 195, 12, 85, 100, Color{ 60, 200, 90, 255 }, Color{ 30, 30, 30, 255 }, "DEFENSE (Giap Phong Thu): 85/100");
        }

        canvas.endCanvas();
        canvas.renderToScreen();
    }

    assets.cleanup();
    canvas.close();
    return 0;
}
