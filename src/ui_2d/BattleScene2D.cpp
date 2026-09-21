#include "ui_2d/BattleScene2D.h"
#include <algorithm>
#include <cmath>

BattleScene2D::BattleScene2D(Hero& hero, Enemy& enemy, Inventory* inventory)
    : m_hero(hero)
    , m_enemy(enemy)
    , m_inventory(inventory ? inventory : &hero.getInventory())
    , m_engine(hero, enemy, inventory ? inventory : &hero.getInventory())
    , m_menuState(BattleMenuState::MAIN_MENU)
    , m_selectedAction(0)
    , m_selectedSubIndex(0)
    , m_heroOffsetAnimX(0.0f)
    , m_enemyOffsetAnimX(0.0f)
    , m_heroHurtTimer(0.0f)
    , m_enemyHurtTimer(0.0f)
    , m_animTimer(0.0f)
    , m_displayHeroHp((float)hero.getHp())
    , m_displayHeroMp((float)hero.getMp())
    , m_displayEnemyHp((float)enemy.getHp())
    , m_screenShakeTimer(0.0f)
    , m_screenShakeIntensity(0.0f)
    , m_shakeOffset(Vector2{ 0, 0 })
    , m_attackEffectTimer(0.0f)
    , m_attackEffectPos(Vector2{ 0, 0 })
    , m_attackEffectColor(WHITE)
    , m_lastCombatLog("Trận chiến bắt đầu! Hãy chọn hành động.")
    , m_turnSequenceTimer(0.0f)
    , m_turnSequencePhase(0)
    , m_pendingAction(0)
    , m_pendingSubIndex(-1)
    , m_finished(false)
    , m_finalResult(CombatState::ONGOING)
{
    m_engine.startBattle();
}

void BattleScene2D::addFloatingText(const std::string& text, Vector2 pos, Color color, float duration) {
    FloatingText ft;
    ft.text = text;
    ft.pos = pos;
    ft.color = color;
    ft.timer = duration;
    ft.maxTimer = duration;
    ft.offsetY = 0.0f;
    m_floatingTexts.push_back(ft);
}

void BattleScene2D::triggerScreenShake(float intensity, float duration) {
    m_screenShakeIntensity = intensity;
    m_screenShakeTimer = duration;
}

void BattleScene2D::update(float dt, Vector2 vMouse) {
    m_animTimer += dt;

    // 1. Cập nhật Screen Shake
    if (m_screenShakeTimer > 0.0f) {
        m_screenShakeTimer -= dt;
        m_shakeOffset.x = ((float)(rand() % 100) / 50.0f - 1.0f) * m_screenShakeIntensity;
        m_shakeOffset.y = ((float)(rand() % 100) / 50.0f - 1.0f) * m_screenShakeIntensity;
    } else {
        m_shakeOffset = Vector2{ 0, 0 };
    }

    // 2. Cập nhật hoạt ảnh chớp đau (Hurt timers)
    if (m_heroHurtTimer > 0.0f) m_heroHurtTimer -= dt;
    if (m_enemyHurtTimer > 0.0f) m_enemyHurtTimer -= dt;
    if (m_attackEffectTimer > 0.0f) m_attackEffectTimer -= dt;

    // 3. Nội suy thanh máu / mana mượt mà (Smooth bar interpolation)
    float targetHeroHp = (float)m_hero.getHp();
    float targetHeroMp = (float)m_hero.getMp();
    float targetEnemyHp = (float)m_enemy.getHp();

    m_displayHeroHp += (targetHeroHp - m_displayHeroHp) * std::min(1.0f, dt * 10.0f);
    m_displayHeroMp += (targetHeroMp - m_displayHeroMp) * std::min(1.0f, dt * 10.0f);
    m_displayEnemyHp += (targetEnemyHp - m_displayEnemyHp) * std::min(1.0f, dt * 10.0f);

    // 4. Cập nhật Floating Combat Texts
    for (auto it = m_floatingTexts.begin(); it != m_floatingTexts.end();) {
        it->timer -= dt;
        it->offsetY -= dt * 25.0f; // Bay lên trên
        if (it->timer <= 0.0f) {
            it = m_floatingTexts.erase(it);
        } else {
            ++it;
        }
    }

    // 5. Chuỗi xử lý lượt đánh theo thời gian (Turn Sequencer)
    if (m_menuState == BattleMenuState::ANIMATING) {
        m_turnSequenceTimer += dt;

        if (m_turnSequencePhase == 1) { // Hero lao lên tấn công
            m_heroOffsetAnimX = std::sin(m_turnSequenceTimer * 10.0f) * 35.0f;
            if (m_turnSequenceTimer >= 0.45f) {
                m_heroOffsetAnimX = 0.0f;
                // Thực thi hành động của Hero qua CombatEngine
                int prevEnemyHp = m_enemy.getHp();
                CombatState state = m_engine.executeTurn(m_pendingAction, m_pendingSubIndex);
                int dmgDealt = prevEnemyHp - m_enemy.getHp();

                if (dmgDealt > 0) {
                    m_enemyHurtTimer = 0.4f;
                    triggerScreenShake(3.0f, 0.25f);
                    m_attackEffectTimer = 0.3f;
                    m_attackEffectPos = Vector2{ 340, 110 };
                    m_attackEffectColor = YELLOW;
                    addFloatingText("-" + std::to_string(dmgDealt), Vector2{ 340, 85 }, YELLOW);
                    m_lastCombatLog = m_hero.getName() + " gây " + std::to_string(dmgDealt) + " sát thương lên " + m_enemy.getName() + "!";
                } else if (m_pendingAction == 4) {
                    m_lastCombatLog = m_hero.getName() + " vào thế phòng thủ kiên cố!";
                    addFloatingText("🛡️ PHÒNG THỦ", Vector2{ 110, 85 }, SKYBLUE);
                } else if (m_pendingAction == 5) {
                    if (state == CombatState::FLED) {
                        m_lastCombatLog = m_hero.getName() + " đã đào tẩu thành công!";
                        addFloatingText("🏃 ĐÀO TẨU", Vector2{ 110, 85 }, GREEN);
                        m_menuState = BattleMenuState::FLED_SCREEN;
                        m_finalResult = CombatState::FLED;
                        return;
                    } else {
                        m_lastCombatLog = "Không thể bỏ chạy khỏi Trùm Cuối (Boss)!";
                        addFloatingText("❌ THẤT BẠI", Vector2{ 110, 85 }, RED);
                    }
                }

                if (!m_enemy.isAlive() || state == CombatState::HERO_VICTORY) {
                    m_menuState = BattleMenuState::VICTORY_SCREEN;
                    m_finalResult = CombatState::HERO_VICTORY;
                    m_lastCombatLog = "★ " + m_enemy.getName() + " đã bị tiêu diệt hoàn toàn!";
                    addFloatingText("★ VICTORY! ★", Vector2{ 200, 70 }, GOLD, 2.5f);
                    return;
                }

                // Chuyển sang lượt quái vật phản công
                m_turnSequencePhase = 2;
                m_turnSequenceTimer = 0.0f;
            }
        }
        else if (m_turnSequencePhase == 2) { // Quái vật lao sang phản kích
            m_enemyOffsetAnimX = -std::sin(m_turnSequenceTimer * 10.0f) * 35.0f;
            if (m_turnSequenceTimer >= 0.45f) {
                m_enemyOffsetAnimX = 0.0f;
                int prevHeroHp = m_hero.getHp();
                m_engine.processEnemyTurn();
                int dmgTaken = prevHeroHp - m_hero.getHp();

                if (dmgTaken > 0) {
                    m_heroHurtTimer = 0.4f;
                    triggerScreenShake(4.0f, 0.3f);
                    m_attackEffectTimer = 0.3f;
                    m_attackEffectPos = Vector2{ 110, 110 };
                    m_attackEffectColor = RED;
                    addFloatingText("-" + std::to_string(dmgTaken), Vector2{ 110, 85 }, RED);
                    m_lastCombatLog = m_enemy.getName() + " phản kích gây " + std::to_string(dmgTaken) + " sát thương!";
                }

                if (!m_hero.isAlive() || m_engine.getState() == CombatState::ENEMY_VICTORY) {
                    m_menuState = BattleMenuState::DEFEAT_SCREEN;
                    m_finalResult = CombatState::ENEMY_VICTORY;
                    m_lastCombatLog = "💀 Bạn đã thất trận trước " + m_enemy.getName() + "...";
                    return;
                }

                // Kết thúc lượt, trả quyền điều khiển về người chơi
                m_menuState = BattleMenuState::MAIN_MENU;
                m_turnSequencePhase = 0;
            }
        }
        return;
    }

    // 6. Xử lý màn hình Kết Quả (Victory / Defeat / Fled)
    if (m_menuState == BattleMenuState::VICTORY_SCREEN || 
        m_menuState == BattleMenuState::DEFEAT_SCREEN || 
        m_menuState == BattleMenuState::FLED_SCREEN) 
    {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            m_finished = true;
        }
        return;
    }

    // 7. Xử lý Menu Chiến đấu chính & Menu Kỹ năng/Vật phẩm
    if (m_menuState == BattleMenuState::MAIN_MENU) {
        // Phím mũi tên / WASD
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            m_selectedAction = (m_selectedAction - 1 + 5) % 5;
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            m_selectedAction = (m_selectedAction + 1) % 5;
        }

        // Phím số 1-5
        for (int k = 0; k < 5; ++k) {
            if (IsKeyPressed(KEY_ONE + k) || IsKeyPressed(KEY_KP_1 + k)) {
                executeHeroAction(k + 1);
                return;
            }
        }

        // Tương tác chuột trên 5 nút hành động
        for (size_t i = 0; i < m_buttonRects.size() && i < 5; ++i) {
            if (CheckCollisionPointRec(vMouse, m_buttonRects[i])) {
                m_selectedAction = (int)i;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    executeHeroAction((int)i + 1);
                    return;
                }
            }
        }

        // Phím Enter / Space
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            executeHeroAction(m_selectedAction + 1);
        }
    }
    else if (m_menuState == BattleMenuState::SKILL_MENU) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
            m_menuState = BattleMenuState::MAIN_MENU;
            return;
        }

        // 3 kỹ năng của từng class
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) m_selectedSubIndex = (m_selectedSubIndex - 1 + 3) % 3;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) m_selectedSubIndex = (m_selectedSubIndex + 1) % 3;

        for (int k = 0; k < 3; ++k) {
            if (IsKeyPressed(KEY_ONE + k) || IsKeyPressed(KEY_KP_1 + k)) {
                m_selectedSubIndex = k;
                executeHeroAction(2, k);
                return;
            }
        }

        for (size_t i = 0; i < m_buttonRects.size() && i < 3; ++i) {
            if (CheckCollisionPointRec(vMouse, m_buttonRects[i])) {
                m_selectedSubIndex = (int)i;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    executeHeroAction(2, (int)i);
                    return;
                }
            }
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            executeHeroAction(2, m_selectedSubIndex);
        }
    }
    else if (m_menuState == BattleMenuState::ITEM_MENU) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
            m_menuState = BattleMenuState::MAIN_MENU;
            return;
        }

        std::vector<std::shared_ptr<Item>> potions;
        for (const auto& item : m_hero.getInventory().getItems()) {
            if (item && item->getType() == ItemType::POTION) {
                potions.push_back(item);
            }
        }
        int potCount = (int)potions.size();
        if (potCount == 0) {
            m_menuState = BattleMenuState::MAIN_MENU;
            return;
        }

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) m_selectedSubIndex = (m_selectedSubIndex - 1 + potCount) % potCount;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) m_selectedSubIndex = (m_selectedSubIndex + 1) % potCount;

        for (int k = 0; k < potCount; ++k) {
            if (IsKeyPressed(KEY_ONE + k) || IsKeyPressed(KEY_KP_1 + k)) {
                executeHeroAction(3, k);
                return;
            }
        }

        for (size_t i = 0; i < m_buttonRects.size() && i < (size_t)potCount; ++i) {
            if (CheckCollisionPointRec(vMouse, m_buttonRects[i])) {
                m_selectedSubIndex = (int)i;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    executeHeroAction(3, (int)i);
                    return;
                }
            }
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            executeHeroAction(3, m_selectedSubIndex);
        }
    }
}

void BattleScene2D::executeHeroAction(int action, int subIndex) {
    if (action == 2 && subIndex == -1) {
        m_menuState = BattleMenuState::SKILL_MENU;
        m_selectedSubIndex = 0;
        return;
    }
    if (action == 3 && subIndex == -1) {
        bool hasPotion = false;
        for (const auto& item : m_hero.getInventory().getItems()) {
            if (item && item->getType() == ItemType::POTION) {
                hasPotion = true;
                break;
            }
        }
        if (!hasPotion) {
            m_lastCombatLog = "Túi đồ không còn dược phẩm nào!";
            addFloatingText("HẾT DƯỢC PHẨM", Vector2{ 110, 85 }, GRAY);
            return;
        }
        m_menuState = BattleMenuState::ITEM_MENU;
        m_selectedSubIndex = 0;
        return;
    }

    // Chuyển sang pha hoạt ảnh chiến đấu
    m_menuState = BattleMenuState::ANIMATING;
    m_turnSequencePhase = 1;
    m_turnSequenceTimer = 0.0f;
    m_pendingAction = action;
    m_pendingSubIndex = subIndex;
}

void BattleScene2D::render() {
    // Áp dụng độ rung lắc màn hình (Screen Shake)
    Camera2D cam = { 0 };
    cam.offset = m_shakeOffset;
    cam.zoom = 1.0f;

    BeginMode2D(cam);

    // 1. Vẽ Đấu trường bối cảnh 2D
    drawArenaBackground();

    // 2. Vẽ 2 bên tham chiến (Hero & Quái vật/Boss)
    drawFighters();

    // 3. Vẽ hiệu ứng chém / đòn đánh ma thuật nếu có
    if (m_attackEffectTimer > 0.0f) {
        DrawCircle((int)m_attackEffectPos.x, (int)m_attackEffectPos.y, 20.0f, m_attackEffectColor);
        DrawCircleLines((int)m_attackEffectPos.x, (int)m_attackEffectPos.y, 26.0f, WHITE);
    }

    // 4. Vẽ chữ số sát thương bay lên (Floating Numbers)
    drawFloatingTexts();

    // 5. Vẽ HUD thông tin chỉ số HP/MP trên đầu
    drawCombatHUD();

    // 6. Vẽ Menu hành động & Nhật ký trận đấu ở nửa dưới màn hình
    if (m_menuState == BattleMenuState::MAIN_MENU || m_menuState == BattleMenuState::ANIMATING) {
        drawActionMenu();
    } else if (m_menuState == BattleMenuState::SKILL_MENU) {
        drawSkillMenu();
    } else if (m_menuState == BattleMenuState::ITEM_MENU) {
        drawItemMenu();
    } else if (m_menuState == BattleMenuState::VICTORY_SCREEN || 
               m_menuState == BattleMenuState::DEFEAT_SCREEN || 
               m_menuState == BattleMenuState::FLED_SCREEN) 
    {
        drawOutcomeOverlay();
    }

    EndMode2D();
}

void BattleScene2D::drawArenaBackground() {
    // Bầu trời đấu trường gradient đen-tím
    ClearBackground(Color{ 18, 14, 28, 255 });

    // Tường thành hầm ngục rêu phong
    DrawRectangle(0, 0, 480, 150, Color{ 14, 10, 22, 255 });
    for (int x = 0; x < 480; x += 40) {
        DrawRectangle(x, 40, 38, 18, Color{ 24, 18, 36, 255 });
        DrawRectangle(x + 20, 60, 38, 18, Color{ 24, 18, 36, 255 });
    }

    // Đuốc phát sáng 2 bên đấu trường
    DrawRectangle(60, 50, 6, 25, Color{ 90, 50, 20, 255 });
    DrawCircle(63, 45, 8, Color{ 255, 140, 20, 180 });
    DrawCircle(63, 45, 4, Color{ 255, 230, 80, 255 });

    DrawRectangle(410, 50, 6, 25, Color{ 90, 50, 20, 255 });
    DrawCircle(413, 45, 8, Color{ 255, 140, 20, 180 });
    DrawCircle(413, 45, 4, Color{ 255, 230, 80, 255 });

    // Nền sàn gạch chiến trường
    DrawRectangle(0, 140, 480, 130, Color{ 35, 30, 48, 255 });
    DrawLine(0, 140, 480, 140, Color{ 80, 70, 100, 255 });

    // Vòng ma pháp / Sàn đấu Hero & Quái vật
    DrawEllipse(120, 145, 40, 12, Color{ 20, 18, 32, 200 });
    DrawEllipse(360, 145, 55, 15, Color{ 20, 18, 32, 200 });
}

void BattleScene2D::drawFighters() {
    AssetManager& assets = AssetManager::getInstance();

    // 1. Hero Side (Bên Trái)
    float heroBounce = std::sin(m_animTimer * 4.0f) * 2.0f;
    float heroX = 105.0f + m_heroOffsetAnimX;
    float heroY = 100.0f + heroBounce;
    Texture2D heroTex = assets.getHeroTexture(m_hero.getHeroClassName());
    assets.drawEntitySprite(heroTex, heroX, heroY, 2.2f, false, m_heroHurtTimer > 0.0f);

    // 2. Enemy / Boss Side (Bên Phải)
    bool isBoss = (m_enemy.getType() == EnemyType::BOSS);
    float enemyBounce = std::cos(m_animTimer * 3.5f) * 2.0f;
    float enemyX = 330.0f + m_enemyOffsetAnimX;
    float enemyY = (isBoss ? 65.0f : 90.0f) + enemyBounce;
    Texture2D enemyTex = assets.getEnemyTexture(m_enemy.getName());
    float enemyScale = isBoss ? 2.2f : 2.0f;
    assets.drawEntitySprite(enemyTex, enemyX, enemyY, enemyScale, true, m_enemyHurtTimer > 0.0f);
}

void BattleScene2D::drawCombatHUD() {
    AssetManager& assets = AssetManager::getInstance();

    // HUD Bên Hero (Góc Trái Trên)
    assets.drawNineSliceBox(Rectangle{ 15, 10, 180, 45 }, Color{ 15, 20, 38, 230 }, SKYBLUE);
    assets.drawText(TextFormat("⚔️ %s (Lv.%d)", m_hero.getName().c_str(), m_hero.getLevel()), 24, 15, 10, YELLOW);
    assets.drawStatBar(24, 28, 160, 9, (int)m_displayHeroHp, m_hero.getMaxHp(), Color{ 220, 40, 50, 255 }, Color{ 30, 30, 30, 255 });
    assets.drawStatBar(24, 40, 160, 7, (int)m_displayHeroMp, m_hero.getMaxMp(), Color{ 40, 130, 240, 255 }, Color{ 30, 30, 30, 255 });

    // HUD Bên Quái vật / Boss (Góc Phải Trên)
    bool isBoss = (m_enemy.getType() == EnemyType::BOSS);
    Color enemyBorder = isBoss ? RED : ORANGE;
    assets.drawNineSliceBox(Rectangle{ 285, 10, 180, 45 }, Color{ 25, 15, 20, 230 }, enemyBorder);
    assets.drawText(TextFormat("💀 %s %s", m_enemy.getName().c_str(), isBoss ? "[BOSS]" : ""), 294, 15, 10, enemyBorder);
    assets.drawStatBar(294, 28, 160, 11, (int)m_displayEnemyHp, m_enemy.getMaxHp(), Color{ 220, 30, 40, 255 }, Color{ 30, 30, 30, 255 });
    assets.drawText(TextFormat("HP: %d/%d", m_enemy.getHp(), m_enemy.getMaxHp()), 340, 41, 8, LIGHTGRAY);
}

void BattleScene2D::drawActionMenu() {
    AssetManager& assets = AssetManager::getInstance();
    m_buttonRects.clear();

    // Khung bảng điều khiển chiến đấu ở đáy màn hình
    assets.drawNineSliceBox(Rectangle{ 15, 160, 450, 100 }, Color{ 12, 16, 30, 245 }, GOLD);

    // Nhật ký trận đấu
    DrawRectangle(25, 168, 430, 18, Color{ 20, 25, 45, 200 });
    assets.drawText(TextFormat("📢 %s", m_lastCombatLog.c_str()), 32, 172, 9, RAYWHITE);

    // 5 Nút hành động chính
    std::string actions[] = { "1.Tấn Công", "2.Kỹ Năng", "3.Dược Phẩm", "4.Phòng Thủ", "5.Bỏ Chạy" };
    for (int i = 0; i < 5; ++i) {
        Rectangle btn = { (float)(25 + i * 86), 195.0f, 82.0f, 55.0f };
        m_buttonRects.push_back(btn);

        bool isSelected = (m_selectedAction == i);
        Color bgColor = isSelected ? Color{ 60, 75, 130, 255 } : Color{ 25, 30, 55, 220 };
        Color border = isSelected ? GOLD : Color{ 80, 95, 140, 255 };

        DrawRectangleRec(btn, bgColor);
        DrawRectangleLinesEx(btn, 1.0f, border);

        if (isSelected) {
            assets.drawText("▶", btn.x + 4, btn.y + 6, 9, GOLD);
        }

        assets.drawText(actions[i], btn.x + (isSelected ? 16 : 8), btn.y + 8, 9, isSelected ? YELLOW : RAYWHITE);

        // Icon phụ bên dưới nút
        if (i == 0) assets.drawItemIcon("sword", btn.x + 32, btn.y + 28, 16);
        else if (i == 1) assets.drawItemIcon("staff", btn.x + 32, btn.y + 28, 16);
        else if (i == 2) assets.drawItemIcon("potion_hp", btn.x + 32, btn.y + 28, 16);
        else if (i == 3) assets.drawItemIcon("armor", btn.x + 32, btn.y + 28, 16);
        else assets.drawText("🏃", btn.x + 34, btn.y + 28, 14, WHITE);
    }
}

void BattleScene2D::drawSkillMenu() {
    AssetManager& assets = AssetManager::getInstance();
    m_buttonRects.clear();

    assets.drawNineSliceBox(Rectangle{ 15, 160, 450, 100 }, Color{ 14, 18, 34, 250 }, VIOLET);
    assets.drawText("✨ CHỌN TUYỆT KỸ MA THUẬT / CHIÊU THỨC ([ESC]: Quay lại)", 25, 168, 9, VIOLET);

    // 3 Kỹ năng mặc định theo class
    std::string skillNames[3];
    int mpCosts[3] = { 10, 15, 25 };

    std::string hClass = m_hero.getHeroClassName();
    if (hClass == "Warrior" || hClass == "Chiến Binh") {
        skillNames[0] = "1. Trảm Kích (Power Strike)";
        skillNames[1] = "2. Phản Đòn (Parry Stance)";
        skillNames[2] = "3. Khiên Hoàng Gia (Shield Slam)";
    } else if (hClass == "Mage" || hClass == "Pháp Sư") {
        skillNames[0] = "1. Cầu Lửa (Fireball)";
        skillNames[1] = "2. Băng Tiễn (Frost Bolt)";
        skillNames[2] = "3. Thiên Thạch (Meteor)";
    } else {
        skillNames[0] = "1. Bắn Đôi (Double Shot)";
        skillNames[1] = "2. Mưa Tên (Arrow Rain)";
        skillNames[2] = "3. Nhắm Bắn Chuẩn Xác (Snipe)";
    }

    for (int i = 0; i < 3; ++i) {
        Rectangle btn = { 25.0f, (float)(185 + i * 22), 430.0f, 20.0f };
        m_buttonRects.push_back(btn);

        bool isSelected = (m_selectedSubIndex == i);
        bool hasMp = (m_hero.getMp() >= mpCosts[i]);

        Color bgColor = isSelected ? Color{ 60, 40, 90, 255 } : Color{ 25, 20, 40, 220 };
        Color border = isSelected ? GOLD : Color{ 90, 70, 130, 255 };
        Color textColor = hasMp ? (isSelected ? YELLOW : RAYWHITE) : GRAY;

        DrawRectangleRec(btn, bgColor);
        DrawRectangleLinesEx(btn, 1.0f, border);

        assets.drawText(skillNames[i], btn.x + 10, btn.y + 4, 9, textColor);
        assets.drawText(TextFormat("[Tiêu hao: %d MP]", mpCosts[i]), btn.x + 310, btn.y + 4, 9, hasMp ? SKYBLUE : RED);
    }
}

void BattleScene2D::drawItemMenu() {
    AssetManager& assets = AssetManager::getInstance();
    m_buttonRects.clear();

    assets.drawNineSliceBox(Rectangle{ 15, 160, 450, 100 }, Color{ 14, 18, 34, 250 }, GREEN);
    assets.drawText("🎒 CHỌN DƯỢC PHẨM SỬ DỤNG ([ESC]: Quay lại)", 25, 168, 9, GREEN);

    std::vector<std::shared_ptr<Item>> potions;
    for (const auto& item : m_hero.getInventory().getItems()) {
        if (item && item->getType() == ItemType::POTION) {
            potions.push_back(item);
        }
    }
    for (size_t i = 0; i < potions.size() && i < 3; ++i) {
        Rectangle btn = { 25.0f, (float)(185 + i * 22), 430.0f, 20.0f };
        m_buttonRects.push_back(btn);

        bool isSelected = (m_selectedSubIndex == (int)i);
        Color bgColor = isSelected ? Color{ 30, 60, 40, 255 } : Color{ 20, 35, 25, 220 };
        Color border = isSelected ? GOLD : Color{ 60, 110, 70, 255 };

        DrawRectangleRec(btn, bgColor);
        DrawRectangleLinesEx(btn, 1.0f, border);

        assets.drawItemIcon(potions[i]->getName(), btn.x + 6, btn.y + 2, 16);
        std::string itemText = "[" + std::to_string(i + 1) + "] " + potions[i]->getName() + " - " + potions[i]->getDescription();
        assets.drawText(itemText, btn.x + 28, btn.y + 4, 9, isSelected ? YELLOW : RAYWHITE);
    }
}

void BattleScene2D::drawFloatingTexts() {
    AssetManager& assets = AssetManager::getInstance();
    for (const auto& ft : m_floatingTexts) {
        float alpha = std::min(1.0f, ft.timer / (ft.maxTimer * 0.5f));
        Color c = ft.color;
        c.a = (unsigned char)(255 * alpha);
        assets.drawText(ft.text, ft.pos.x, ft.pos.y + ft.offsetY, 11, c);
    }
}

void BattleScene2D::drawOutcomeOverlay() {
    AssetManager& assets = AssetManager::getInstance();

    if (m_menuState == BattleMenuState::VICTORY_SCREEN) {
        Rectangle winBox = { 60, 165, 360, 90 };
        assets.drawNineSliceBox(winBox, Color{ 15, 35, 20, 245 }, GOLD);
        assets.drawText("★ CHIẾN THẮNG QUANG VINH ★", 120, 176, 12, GOLD);
        assets.drawText(TextFormat("+ Nhận được: %d EXP và %d Vàng!", m_enemy.getExpReward(), m_enemy.getGoldReward()), 130, 200, 10, YELLOW);
        assets.drawText("▶ Nhấn [SPACE / ENTER / CLICK] để tiếp tục hành trình...", 90, 225, 9, LIGHTGRAY);
    }
    else if (m_menuState == BattleMenuState::DEFEAT_SCREEN) {
        Rectangle loseBox = { 60, 165, 360, 90 };
        assets.drawNineSliceBox(loseBox, Color{ 35, 12, 15, 245 }, RED);
        assets.drawText("💀 BẠN ĐÃ TỬ TRẬN 💀", 160, 176, 12, RED);
        assets.drawText("Sức mạnh của bóng tối đã nuốt chửng dũng sĩ...", 120, 200, 10, RAYWHITE);
        assets.drawText("▶ Nhấn [SPACE / ENTER / CLICK] để tiếp tục...", 120, 225, 9, LIGHTGRAY);
    }
    else if (m_menuState == BattleMenuState::FLED_SCREEN) {
        Rectangle fledBox = { 60, 165, 360, 90 };
        assets.drawNineSliceBox(fledBox, Color{ 20, 25, 45, 245 }, SKYBLUE);
        assets.drawText("🏃 ĐÀO TẨU AN TOÀN", 170, 176, 12, SKYBLUE);
        assets.drawText("Bạn đã nhanh nhẹn rút lui khỏi trận chiến nguy hiểm.", 115, 200, 10, RAYWHITE);
        assets.drawText("▶ Nhấn [SPACE / ENTER / CLICK] để tiếp tục hành trình...", 90, 225, 9, LIGHTGRAY);
    }
}
