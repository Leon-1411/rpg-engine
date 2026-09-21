#include "ui_2d/TitleScene2D.h"
#include <cmath>
#include <algorithm>

TitleScene2D::TitleScene2D(SaveManager& saveManager)
    : m_saveManager(saveManager)
    , m_state(TitleMenuState::MAIN_MENU)
    , m_menuIndex(0)
    , m_classIndex(0)
    , m_loadSlotIndex(0)
    , m_selectedLoadSlot(-1)
    , m_startNewGame(false)
    , m_gameLoaded(false)
    , m_quitRequested(false)
    , m_animTimer(0.0f)
    , m_heroName("Arthur")
{
}

std::unique_ptr<Hero> TitleScene2D::createSelectedHero() {
    std::string name = m_heroName.empty() ? "Anh Hùng" : m_heroName;
    if (m_classIndex == 0) {
        return std::make_unique<Warrior>(name);
    } else if (m_classIndex == 1) {
        return std::make_unique<Mage>(name);
    } else {
        return std::make_unique<Ranger>(name);
    }
}

void TitleScene2D::update(float dt) {
    m_animTimer += dt;

    switch (m_state) {
        case TitleMenuState::MAIN_MENU:    updateMainMenu(); break;
        case TitleMenuState::CLASS_SELECT: updateClassSelect(); break;
        case TitleMenuState::LOAD_GAME:    updateLoadGame(); break;
        case TitleMenuState::CREDITS:      updateCredits(); break;
    }
}

void TitleScene2D::updateMainMenu() {
    int maxOptions = 4;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_menuIndex = (m_menuIndex - 1 + maxOptions) % maxOptions;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_menuIndex = (m_menuIndex + 1) % maxOptions;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_E)) {
        if (m_menuIndex == 0) {
            m_state = TitleMenuState::CLASS_SELECT;
            m_classIndex = 0;
            m_heroName = "Arthur";
        } else if (m_menuIndex == 1) {
            m_state = TitleMenuState::LOAD_GAME;
            m_loadSlotIndex = 0;
        } else if (m_menuIndex == 2) {
            m_state = TitleMenuState::CREDITS;
        } else if (m_menuIndex == 3) {
            m_quitRequested = true;
        }
    }
}

void TitleScene2D::updateClassSelect() {
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        m_classIndex = (m_classIndex - 1 + 3) % 3;
        if (m_classIndex == 0) m_heroName = "Arthur";
        else if (m_classIndex == 1) m_heroName = "Merlin";
        else m_heroName = "Robin";
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        m_classIndex = (m_classIndex + 1) % 3;
        if (m_classIndex == 0) m_heroName = "Arthur";
        else if (m_classIndex == 1) m_heroName = "Merlin";
        else m_heroName = "Robin";
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        m_state = TitleMenuState::MAIN_MENU;
        return;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_E)) {
        m_startNewGame = true;
    }
}

void TitleScene2D::updateLoadGame() {
    int maxSlots = 3;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_loadSlotIndex = (m_loadSlotIndex - 1 + maxSlots) % maxSlots;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_loadSlotIndex = (m_loadSlotIndex + 1) % maxSlots;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        m_state = TitleMenuState::MAIN_MENU;
        return;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_E)) {
        int targetSlot = m_loadSlotIndex + 1;
        if (m_saveManager.slotExists(targetSlot)) {
            m_selectedLoadSlot = targetSlot;
            m_gameLoaded = true;
        }
    }
}

void TitleScene2D::updateCredits() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_E)) {
        m_state = TitleMenuState::MAIN_MENU;
    }
}

void TitleScene2D::render() {
    // Nền bầu trời đêm ma thuật với sao lấp lánh
    DrawRectangleGradientV(0, 0, 480, 270, Color{ 8, 10, 24, 255 }, Color{ 22, 16, 42, 255 });

    // Hiệu ứng hạt sao rơi / lấp lánh
    for (int i = 0; i < 40; ++i) {
        int sx = (i * 97 + (int)(m_animTimer * 10.0f)) % 480;
        int sy = (i * 61 + (int)(m_animTimer * (5.0f + (i % 5)))) % 220;
        float alpha = (std::sin(m_animTimer * 3.0f + i) + 1.0f) * 0.5f;
        DrawPixel(sx, sy, Color{ 200, 220, 255, (unsigned char)(alpha * 220 + 35) });
    }

    // Dãy núi bóng mờ xa xa ở dưới đáy
    for (int x = 0; x < 480; x += 40) {
        DrawTriangle(
            Vector2{ (float)x - 20, 270.0f },
            Vector2{ (float)x + 20, 180.0f + (x % 30) },
            Vector2{ (float)x + 60, 270.0f },
            Color{ 12, 14, 30, 220 }
        );
    }

    switch (m_state) {
        case TitleMenuState::MAIN_MENU:    renderMainMenu(); break;
        case TitleMenuState::CLASS_SELECT: renderClassSelect(); break;
        case TitleMenuState::LOAD_GAME:    renderLoadGame(); break;
        case TitleMenuState::CREDITS:      renderCredits(); break;
    }
}

void TitleScene2D::renderMainMenu() {
    AssetManager& assets = AssetManager::getInstance();

    // 1. Logo Tiêu Đề Phát Sáng
    float glow = (std::sin(m_animTimer * 4.0f) + 1.0f) * 0.5f;
    float titleY = 32.0f + std::sin(m_animTimer * 2.0f) * 3.0f;

    // Bóng chữ
    assets.drawText("ELDORIA: THE SHADOW LORD", 240 - 150 + 2, titleY + 2, 16, Color{ 0, 0, 0, 200 });
    // Chữ viền vàng
    assets.drawText("ELDORIA: THE SHADOW LORD", 240 - 150, titleY, 16, Color{ (unsigned char)(230 + glow * 25), (unsigned char)(190 + glow * 40), 50, 255 });

    assets.drawText("2D Retro Pixel RPG Engine", 240 - 75, titleY + 22, 9, SKYBLUE);

    // 2. Hiển thị 3 Hero đứng bảo vệ ở 2 bên
    Texture2D warTex = assets.getHeroTexture("Warrior");
    Texture2D magTex = assets.getHeroTexture("Mage");
    Texture2D ranTex = assets.getHeroTexture("Ranger");

    assets.drawEntitySprite(warTex, 60, 130 + std::sin(m_animTimer * 3.0f) * 2.0f, 1.8f);
    assets.drawEntitySprite(magTex, 480 - 100, 130 + std::sin(m_animTimer * 3.0f + 1.5f) * 2.0f, 1.8f, true);

    // 3. Khung Menu Lựa Chọn Ở Giữa
    Rectangle menuBox = { 160, 95, 160, 135 };
    assets.drawNineSliceBox(menuBox, Color{ 14, 18, 36, 245 }, GOLD);

    const char* options[] = {
        "⚔️  Chơi Mới (New Game)",
        "💾  Tiếp Tục (Load Game)",
        "📜  Thông Tin & Hướng Dẫn",
        "🚪  Thoát Trò Chơi (Exit)"
    };

    for (int i = 0; i < 4; ++i) {
        int y = 110 + i * 28;
        bool isSelected = (i == m_menuIndex);

        if (isSelected) {
            float pulse = std::sin(m_animTimer * 8.0f) * 2.0f;
            DrawRectangle(166, y - 4, 148, 22, Color{ 45, 65, 120, 220 });
            DrawRectangleLines(166, y - 4, 148, 22, GOLD);
            assets.drawText(TextFormat("▶ %s", options[i]), 170 + pulse, y, 9, YELLOW);
        } else {
            assets.drawText(options[i], 174, y, 9, LIGHTGRAY);
        }
    }

    // Đáy màn hình
    assets.drawText("[W/S/Mũi tên]: Chọn Menu | [ENTER / SPACE]: Xác nhận", 95, 252, 8, GRAY);
}

void TitleScene2D::renderClassSelect() {
    AssetManager& assets = AssetManager::getInstance();

    // Tiêu Đề
    assets.drawText("CHỌN LỚP NHÂN VẬT ANH HÙNG", 130, 16, 12, GOLD);
    assets.drawText("Sử dụng [A / D] hoặc [Mũi tên Trái/Phải] để đổi lớp, [ENTER] để bắt đầu!", 65, 34, 8, LIGHTGRAY);

    const char* classNames[] = { "CHIẾN BINH (WARRIOR)", "PHÁP SƯ (MAGE)", "XẠ THỦ (RANGER)" };
    const char* classTypes[] = { "Warrior", "Mage", "Ranger" };

    for (int i = 0; i < 3; ++i) {
        int boxX = 24 + i * 148;
        int boxY = 52;
        int boxW = 136;
        int boxH = 188;

        bool isSelected = (i == m_classIndex);
        Color borderColor = isSelected ? GOLD : Color{ 60, 80, 120, 255 };
        Color bgColor = isSelected ? Color{ 22, 32, 60, 245 } : Color{ 12, 16, 30, 230 };

        assets.drawNineSliceBox(Rectangle{ (float)boxX, (float)boxY, (float)boxW, (float)boxH }, bgColor, borderColor);

        // Tên Lớp
        assets.drawText(classNames[i], boxX + 8, boxY + 8, 8, isSelected ? YELLOW : WHITE);

        // Sprite Lớn
        Texture2D tex = assets.getHeroTexture(classTypes[i]);
        float bob = isSelected ? std::sin(m_animTimer * 5.0f) * 3.0f : 0.0f;
        assets.drawEntitySprite(tex, boxX + 48, boxY + 28 - bob, 2.0f);

        // Chỉ Số
        int hp = (i == 0) ? 120 : (i == 1) ? 70 : 90;
        int mp = (i == 0) ? 30 : (i == 1) ? 80 : 50;
        int atk = (i == 0) ? 18 : (i == 1) ? 14 : 16;
        int def = (i == 0) ? 8 : (i == 1) ? 3 : 5;

        int statY = boxY + 85;
        assets.drawText(TextFormat("Máu HP: %d", hp), boxX + 10, statY, 7, Color{ 255, 120, 120, 255 });
        assets.drawStatBar(boxX + 10, statY + 10, 116, 5, hp, 120, Color{ 220, 45, 55, 255 }, Color{ 30, 30, 30, 255 });

        assets.drawText(TextFormat("Mana MP: %d", mp), boxX + 10, statY + 18, 7, Color{ 120, 180, 255, 255 });
        assets.drawStatBar(boxX + 10, statY + 28, 116, 5, mp, 80, Color{ 35, 130, 245, 255 }, Color{ 30, 30, 30, 255 });

        assets.drawText(TextFormat("⚔️ Sát thương: %d", atk), boxX + 10, statY + 38, 7, Color{ 255, 200, 120, 255 });
        assets.drawText(TextFormat("🛡️ Phòng ngự:  %d", def), boxX + 10, statY + 48, 7, Color{ 160, 220, 255, 255 });

        // Đặc Điểm
        const char* traits[] = {
            "Trâu bò, chém mạnh,\nkỹ năng Berserk cực sốc",
            "Sát thương phép khủng,\nthiên thạch Meteor quét sạch",
            "Tốc độ, bão tên diện rộng,\nđòn đánh độc tố liên hoàn"
        };
        assets.drawText(traits[i], boxX + 10, statY + 62, 7, LIGHTGRAY);

        if (isSelected) {
            DrawRectangleLines(boxX + 2, boxY + 2, boxW - 4, boxH - 4, YELLOW);
            assets.drawText("▶ ĐÃ CHỌN ◀", boxX + 34, boxY + boxH - 14, 8, YELLOW);
        }
    }

    assets.drawText("[ESC]: Quay Lại | [ENTER / SPACE]: Xuất Trận!", 135, 252, 8, GRAY);
}

void TitleScene2D::renderLoadGame() {
    AssetManager& assets = AssetManager::getInstance();

    assets.drawText("TIẾP TỤC HÀNH TRÌNH (LOAD GAME)", 125, 18, 12, GOLD);
    assets.drawText("Chọn một ô lưu trữ để tải lại tiến trình:", 135, 36, 8, LIGHTGRAY);

    for (int i = 0; i < 3; ++i) {
        int slot = i + 1;
        int y = 58 + i * 58;
        bool isSelected = (i == m_loadSlotIndex);
        bool exists = m_saveManager.slotExists(slot);

        Rectangle slotBox = { 80, (float)y, 320, 50 };
        Color borderColor = isSelected ? GOLD : Color{ 60, 80, 120, 255 };
        Color bgColor = isSelected ? Color{ 24, 34, 65, 245 } : Color{ 14, 18, 34, 230 };

        assets.drawNineSliceBox(slotBox, bgColor, borderColor);

        if (exists) {
            SavedGameState preview;
            m_saveManager.getSlotPreview(slot, preview);

            assets.drawText(TextFormat("💾 Ô LƯU TRỮ %d: %s (Lv.%d)", slot, preview.heroName.c_str(), preview.level), 94, y + 10, 9, isSelected ? YELLOW : WHITE);
            assets.drawText(TextFormat("Lớp: %s | Vàng: %d G | Nơi ở: %s",
                (preview.heroClass == HeroClass::WARRIOR ? "Chiến Binh" : (preview.heroClass == HeroClass::MAGE ? "Pháp Sư" : "Xạ Thủ")),
                preview.gold, preview.currentStoryNodeId.c_str()), 94, y + 28, 8, LIGHTGRAY);
        } else {
            assets.drawText(TextFormat("💾 Ô LƯU TRỮ %d: (Trống)", slot), 94, y + 18, 9, GRAY);
        }

        if (isSelected) {
            assets.drawText("▶", 84, y + 18, 10, GOLD);
        }
    }

    assets.drawText("[W/S]: Chọn ô | [ENTER]: Tải Game | [ESC]: Quay lại Menu", 100, 250, 8, GRAY);
}

void TitleScene2D::renderCredits() {
    AssetManager& assets = AssetManager::getInstance();

    Rectangle infoBox = { 60, 30, 360, 210 };
    assets.drawNineSliceBox(infoBox, Color{ 14, 20, 38, 250 }, GOLD);

    assets.drawText("📜 THÔNG TIN & HƯỚNG DẪN TRÒ CHƠI", 110, 42, 11, GOLD);
    DrawLine(75, 58, 405, 58, Color{ 70, 90, 140, 255 });

    assets.drawText("🎮 HỆ THỐNG ĐIỀU KHIỂN:", 75, 68, 9, YELLOW);
    assets.drawText("• [W / A / S / D] hoặc [Mũi tên]: Di chuyển nhân vật trên bản đồ thế giới.", 75, 84, 8, WHITE);
    assets.drawText("• [E] hoặc [SPACE]: Tương tác với Nhà Vua, Thương Nhân và Rồng Boss.", 75, 98, 8, WHITE);
    assets.drawText("• [I] hoặc [B]: Mở túi Hành trang, kiểm tra chỉ số & mặc trang bị.", 75, 112, 8, WHITE);
    assets.drawText("• [TAB]: Chuyển chế độ Mua / Bán khi ở Cửa Hàng.", 75, 126, 8, WHITE);

    assets.drawText("⚔️ CƠ CHẾ CHIẾN ĐẤU & CỐT TRUYỆN:", 75, 146, 9, YELLOW);
    assets.drawText("• Cốt truyện phân nhánh đa kết cục dựa trên StoryGraph.", 75, 162, 8, LIGHTGRAY);
    assets.drawText("• Đồ họa 2D Retro Pixel Art kết xuất trực tiếp qua Raylib.", 75, 176, 8, LIGHTGRAY);
    assets.drawText("• Tạo bởi Antigravity Engine C++ 2026.", 75, 190, 8, SKYBLUE);

    assets.drawText("[ESC / ENTER]: Quay lại Menu Chính", 155, 222, 8, GOLD);
}
