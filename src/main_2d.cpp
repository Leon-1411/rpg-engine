#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include "ui_2d/TitleScene2D.h"
#include "ui_2d/OverworldScene2D.h"
#include "ui_2d/StoryScene2D.h"
#include "ui_2d/BattleScene2D.h"
#include "ui_2d/InventoryShopScene2D.h"
#include "StoryGraph.h"
#include "SaveManager.h"
#include "Shop.h"
#include "BossMonster.h"
#include "Minion.h"
#include "Potion.h"
#include "Weapon.h"
#include "Armor.h"
#include <memory>
#include <iostream>
#include <cmath>

enum class GameState2D {
    TITLE_MENU,
    OVERWORLD,
    STORY_DIALOGUE,
    BATTLE,
    INVENTORY,
    SHOP,
    VICTORY_ENDING,
    GAME_OVER
};

int main() {
    // 1. Khởi tạo Cửa sổ & Virtual Pixel Canvas 480x270 (Phóng to 960x540 sắc nét)
    PixelCanvas canvas(480, 270, 2);
    if (!canvas.init("Eldoria: The Shadow Lord - 2D Pixel RPG Engine")) {
        std::cerr << "[Error] Khởi tạo Pixel Canvas thất bại!\n";
        return 1;
    }

    // 2. Nạp toàn bộ tài nguyên Procedural Pixel Art & Font Tiếng Việt
    AssetManager& assets = AssetManager::getInstance();
    assets.init();

    // 3. Khởi tạo Hệ thống Dữ liệu Game
    SaveManager saveManager("saves/");
    StoryGraph storyGraph;
    if (!storyGraph.loadStoryGraph("data/story.json")) {
        std::cerr << "[Warning] Không thể nạp data/story.json, tạo dữ liệu mặc định...\n";
    }

    Shop royalShop("Tiệm Rèn & Dược Phẩm Hoàng Thành", "Nơi cung cấp trang bị và dược liệu tốt nhất vương quốc.");
    royalShop.loadDefaultStock();

    // 4. Các biến điều khiển vòng lặp chính
    GameState2D currentState = GameState2D::TITLE_MENU;

    std::unique_ptr<TitleScene2D> titleScene = std::make_unique<TitleScene2D>(saveManager);
    std::unique_ptr<Hero> hero = nullptr;
    std::shared_ptr<Enemy> currentEnemy = nullptr;
    std::unique_ptr<OverworldScene2D> overworldScene = nullptr;
    std::unique_ptr<StoryScene2D> storyScene = nullptr;
    std::unique_ptr<BattleScene2D> battleScene = nullptr;
    std::unique_ptr<InventoryShopScene2D> invShopScene = nullptr;

    // Biến thông báo nổi toàn cục
    std::string globalToastText = "";
    float globalToastTimer = 0.0f;
    Color globalToastColor = GREEN;
    float endingTimer = 0.0f;

    SetTargetFPS(60);

    while (canvas.isRunning() && !titleScene->hasQuitRequested()) {
        float dt = GetFrameTime();
        Vector2 vMouse = canvas.getVirtualMousePosition();

        if (globalToastTimer > 0.0f) {
            globalToastTimer -= dt;
        }

        // ==========================================
        // CẬP NHẬT LOGIC (UPDATE) THEO TỪNG TRẠNG THÁI
        // ==========================================
        switch (currentState) {
            case GameState2D::TITLE_MENU: {
                titleScene->update(dt);

                if (titleScene->hasStartedNewGame()) {
                    hero = titleScene->createSelectedHero();

                    // Tặng trang bị và dược phẩm tân thủ
                    hero->getInventory().addItem(std::make_shared<Weapon>("wep_wood", "Kiếm Gỗ Tân Thủ", "Vũ khí khởi đầu tăng 6 sát thương.", 6));
                    hero->getInventory().addItem(std::make_shared<Armor>("arm_leather", "Áo Da Sơ Cấp", "Áo giáp nhẹ tăng 4 phòng ngự.", 4));
                    hero->getInventory().addItem(std::make_shared<Potion>("pot_hp_small", "Bình Máu Nhỏ", "Hồi phục 50 điểm HP lập tức.", 50, false, 2));
                    hero->getInventory().addItem(std::make_shared<Potion>("pot_mp_small", "Bình Mana Nhỏ", "Hồi phục 30 điểm MP lập tức.", 30, true, 2));
                    hero->getInventory().equipWeapon(0);
                    hero->getInventory().equipArmor(1);
                    hero->addGold(150);

                    overworldScene = std::make_unique<OverworldScene2D>(*hero, storyGraph);
                    invShopScene = std::make_unique<InventoryShopScene2D>(*hero, &royalShop);
                    storyScene = std::make_unique<StoryScene2D>(storyGraph);

                    currentState = GameState2D::OVERWORLD;
                    globalToastText = "Chào mừng " + hero->getName() + " đến với Vương quốc Eldoria!";
                    globalToastColor = GOLD;
                    globalToastTimer = 3.0f;
                } else if (titleScene->hasLoadedGame()) {
                    int slot = titleScene->getLoadedSlot();
                    // Tạo hero tạm thời để nạp dữ liệu
                    hero = std::make_unique<Warrior>("Hero");
                    if (saveManager.loadGame(slot, *hero, storyGraph)) {
                        overworldScene = std::make_unique<OverworldScene2D>(*hero, storyGraph);
                        invShopScene = std::make_unique<InventoryShopScene2D>(*hero, &royalShop);
                        storyScene = std::make_unique<StoryScene2D>(storyGraph);
                        currentState = GameState2D::OVERWORLD;
                        globalToastText = TextFormat("Đã nạp thành công Ô Lưu Trữ %d!", slot);
                        globalToastColor = GREEN;
                        globalToastTimer = 3.0f;
                    } else {
                        globalToastText = "Lỗi khi nạp dữ liệu lưu trữ!";
                        globalToastColor = RED;
                        globalToastTimer = 3.0f;
                        titleScene->reset();
                    }
                }
                break;
            }

            case GameState2D::OVERWORLD: {
                if (overworldScene) {
                    overworldScene->update(dt);

                    // Phím F5: Lưu nhanh Game (Quick Save)
                    if (IsKeyPressed(KEY_F5)) {
                        if (saveManager.saveGame(1, *hero, storyGraph)) {
                            globalToastText = "💾 Đã lưu game thành công vào Ô 1!";
                            globalToastColor = GREEN;
                            globalToastTimer = 2.5f;
                        }
                    }

                    // Phím I / B: Mở túi đồ
                    if (overworldScene->hasTriggeredInventory()) {
                        overworldScene->clearInventoryTrigger();
                        invShopScene->open(InventoryShopMode::INVENTORY_ONLY);
                        currentState = GameState2D::INVENTORY;
                    }
                    // Mở Cửa hàng
                    else if (overworldScene->hasTriggeredShop()) {
                        overworldScene->clearShopTrigger();
                        invShopScene->setShop(&royalShop);
                        invShopScene->open(InventoryShopMode::SHOP_BUY);
                        currentState = GameState2D::SHOP;
                    }
                    // Kích hoạt Cốt truyện
                    else if (overworldScene->hasTriggeredStory()) {
                        std::string nodeId = overworldScene->getTriggeredNodeId();
                        overworldScene->clearStoryTrigger();
                        storyGraph.setCurrentNodeId(nodeId);
                        storyScene->refreshNode();
                        storyScene->setHeroClass(hero->getHeroClassName());
                        storyScene->setHeroGold(hero->getGold());
                        currentState = GameState2D::STORY_DIALOGUE;
                    }
                    // Kích hoạt Trận chiến Boss
                    else if (overworldScene->hasTriggeredBattle()) {
                        std::string enemyType = overworldScene->getTriggeredEnemyId();
                        overworldScene->clearBattleTrigger();

                        if (enemyType == "ancient_dragon") {
                            currentEnemy = std::make_shared<BossMonster>("dragon_ancient", "Rồng Hắc Ám Cổ Đại", 280, 24, 8, 250, 150, "Cổ Long ngàn năm ngủ say", "Hỏa Diệm Long Tức", 30);
                        } else {
                            currentEnemy = std::make_shared<Minion>("orc_warlord", "Thống Lĩnh Orc Hắc Ám", MinionType::ORC, 160, 18, 5, 120, 80, "Thủ lĩnh quái vật man rợ", "Búa Tạ Cuồng Nộ");
                        }

                        battleScene = std::make_unique<BattleScene2D>(*hero, *currentEnemy, &hero->getInventory());
                        currentState = GameState2D::BATTLE;
                    }
                }
                break;
            }

            case GameState2D::STORY_DIALOGUE: {
                if (storyScene) {
                    storyScene->update(dt, vMouse);

                    if (storyScene->isPendingBattle()) {
                        std::string enemyId = storyScene->getPendingEnemyId();
                        currentEnemy = std::make_shared<BossMonster>("shadow_malakor", "Shadow Lord Malakor", 350, 28, 10, 500, 300, "Chúa Tể Bóng Tối", "Diệt Thế Hắc Lôi", 40);
                        battleScene = std::make_unique<BattleScene2D>(*hero, *currentEnemy, &hero->getInventory());
                        currentState = GameState2D::BATTLE;
                    } else if (storyScene->isPendingShop()) {
                        invShopScene->setShop(&royalShop);
                        invShopScene->open(InventoryShopMode::SHOP_BUY);
                        currentState = GameState2D::SHOP;
                    } else if (storyGraph.isEnding()) {
                        currentState = GameState2D::VICTORY_ENDING;
                        endingTimer = 0.0f;
                    } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                        currentState = GameState2D::OVERWORLD;
                    }
                }
                break;
            }

            case GameState2D::BATTLE: {
                if (battleScene) {
                    battleScene->update(dt, vMouse);

                    if (battleScene->isFinished()) {
                        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_E)) {
                            if (battleScene->getFinalResult() == CombatState::HERO_VICTORY) {
                                globalToastText = "🎉 Chiến thắng vang dội! Đã nhận EXP và Vàng!";
                                globalToastColor = GOLD;
                                globalToastTimer = 3.5f;
                                currentState = GameState2D::OVERWORLD;
                            } else {
                                currentState = GameState2D::GAME_OVER;
                            }
                        }
                    }
                }
                break;
            }

            case GameState2D::INVENTORY:
            case GameState2D::SHOP: {
                if (invShopScene) {
                    invShopScene->update(dt);
                    if (invShopScene->isClosed()) {
                        currentState = GameState2D::OVERWORLD;
                    }
                }
                break;
            }

            case GameState2D::VICTORY_ENDING: {
                endingTimer += dt;
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE)) {
                    titleScene->reset();
                    currentState = GameState2D::TITLE_MENU;
                }
                break;
            }

            case GameState2D::GAME_OVER: {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    // Hồi sinh với 50% HP và về lại thế giới
                    hero->heal(hero->getMaxHp() / 2);
                    overworldScene->setPlayerPosition(5.5f * 16.0f, 4.0f * 16.0f); // Đưa về điện thờ
                    currentState = GameState2D::OVERWORLD;
                    globalToastText = "Bạn đã được hồi sinh tại Hoàng Thành Eldoria!";
                    globalToastColor = YELLOW;
                    globalToastTimer = 3.0f;
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    titleScene->reset();
                    currentState = GameState2D::TITLE_MENU;
                }
                break;
            }
        }

        // ==========================================
        // KẾT XUẤT ĐỒ HỌA (RENDER) LÊN PIXEL CANVAS
        // ==========================================
        canvas.beginCanvas();
        ClearBackground(BLACK);

        switch (currentState) {
            case GameState2D::TITLE_MENU: {
                titleScene->render();
                break;
            }

            case GameState2D::OVERWORLD: {
                if (overworldScene) {
                    overworldScene->render();
                }
                break;
            }

            case GameState2D::STORY_DIALOGUE: {
                if (storyScene) {
                    storyScene->render();
                }
                break;
            }

            case GameState2D::BATTLE: {
                if (battleScene) {
                    battleScene->render();
                }
                break;
            }

            case GameState2D::INVENTORY:
            case GameState2D::SHOP: {
                if (overworldScene) {
                    overworldScene->render(); // Vẽ nền Overworld mờ bên dưới
                }
                if (invShopScene) {
                    invShopScene->render();
                }
                break;
            }

            case GameState2D::VICTORY_ENDING: {
                DrawRectangleGradientV(0, 0, 480, 270, Color{ 12, 24, 45, 255 }, Color{ 35, 18, 48, 255 });

                float pulse = std::sin(endingTimer * 4.0f) * 2.0f;
                assets.drawText("👑 CHIẾN THẮNG VĨNH CỬU 👑", 120, 35 + pulse, 14, GOLD);
                assets.drawText("Vương quốc Eldoria đã được giải phóng khỏi bóng tối!", 80, 60, 9, SKYBLUE);

                assets.drawNineSliceBox(Rectangle{ 90, 85, 300, 120 }, Color{ 16, 22, 40, 240 }, GOLD);

                Texture2D heroTex = assets.getHeroTexture(hero ? hero->getHeroClassName() : "Warrior");
                assets.drawEntitySprite(heroTex, 110, 110, 2.0f);

                assets.drawText(TextFormat("Anh Hùng: %s (Lv.%d)", hero->getName().c_str(), hero->getLevel()), 170, 100, 10, YELLOW);
                assets.drawText(TextFormat("Lớp nhân vật: %s", hero->getHeroClassName().c_str()), 170, 118, 8, LIGHTGRAY);
                assets.drawText(TextFormat("Vàng tích lũy: %d G", hero->getGold()), 170, 134, 8, GOLD);
                assets.drawText(TextFormat("Sức mạnh tối thượng: %d Sát thương", hero->getAttack() + hero->getInventory().getEquippedWeaponBonus()), 170, 150, 8, Color{ 255, 120, 120, 255 });
                assets.drawText("Cảm ơn bạn đã trải nghiệm RPG Engine 2D Pixel!", 130, 185, 8, GREEN);

                assets.drawText("[ENTER / SPACE / ESC]: Về Màn Hình Chính", 130, 235, 9, YELLOW);
                break;
            }

            case GameState2D::GAME_OVER: {
                DrawRectangle(0, 0, 480, 270, Color{ 20, 0, 0, 240 });

                assets.drawText("💀 BẠN ĐÃ TỬ TRẬN 💀", 160, 60, 16, RED);
                assets.drawText("Bóng tối của Malakor đã nuốt chửng linh hồn bạn...", 110, 90, 9, LIGHTGRAY);

                assets.drawNineSliceBox(Rectangle{ 130, 125, 220, 80 }, Color{ 14, 18, 30, 245 }, RED);
                assets.drawText("▶ [ENTER / SPACE]: Hồi sinh tại Lâu Đài", 145, 145, 8, YELLOW);
                assets.drawText("▶ [ESC]: Quay về Menu Chính", 145, 170, 8, LIGHTGRAY);
                break;
            }
        }

        // ==========================================
        // VẼ THÔNG BÁO NỔI TOÀN CỤC (GLOBAL TOAST)
        // ==========================================
        if (globalToastTimer > 0.0f && !globalToastText.empty()) {
            float tw = assets.measureText(globalToastText, 9) + 24.0f;
            Rectangle toastBox = { 240.0f - tw * 0.5f, 12.0f, tw, 22.0f };
            assets.drawNineSliceBox(toastBox, Color{ 10, 14, 28, 245 }, globalToastColor);
            assets.drawText(globalToastText, toastBox.x + 12, toastBox.y + 5, 9, globalToastColor);
        }

        canvas.endCanvas();
        canvas.renderToScreen();
    }

    // 5. Giải phóng tài nguyên trước khi thoát
    assets.cleanup();
    canvas.close();

    return 0;
}
