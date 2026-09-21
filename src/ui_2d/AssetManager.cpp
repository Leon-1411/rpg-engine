#include "ui_2d/AssetManager.h"
#include <algorithm>
#include <cmath>
#include <vector>

AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}

AssetManager::AssetManager() : m_fontLoaded(false), m_initialized(false) {}

AssetManager::~AssetManager() {
    cleanup();
}

void AssetManager::loadVietnameseFont(const char* fontPath) {
    if (m_fontLoaded) {
        UnloadFont(m_font);
        m_fontLoaded = false;
    }

    std::vector<int> codepoints;
    // 1. ASCII 32 - 126
    for (int i = 32; i <= 126; ++i) codepoints.push_back(i);
    // 2. Latin-1 Supplement 128 - 255
    for (int i = 128; i <= 255; ++i) codepoints.push_back(i);
    // 3. Latin Extended-A & B (Đ, đ, Ă, ă, Ơ, ơ, Ư, ư, v.v.)
    for (int i = 0x0100; i <= 0x024F; ++i) codepoints.push_back(i);
    // 4. Latin Extended Additional (Toàn bộ dấu tiếng Việt: sắc, huyền, hỏi, ngã, nặng)
    for (int i = 0x1EA0; i <= 0x1EF9; ++i) codepoints.push_back(i);
    // 5. Ký tự hình học & biểu tượng
    for (int i = 0x25A0; i <= 0x25FF; ++i) codepoints.push_back(i);
    codepoints.push_back(0x2605); // ★
    codepoints.push_back(0x25B6); // ▶
    codepoints.push_back(0x25BC); // ▼

    const char* path = fontPath;
    if (!FileExists(path)) {
        if (FileExists("C:/Windows/Fonts/arial.ttf")) path = "C:/Windows/Fonts/arial.ttf";
        else if (FileExists("C:/Windows/Fonts/segoeui.ttf")) path = "C:/Windows/Fonts/segoeui.ttf";
    }

    if (FileExists(path)) {
        m_font = LoadFontEx(path, 22, codepoints.data(), (int)codepoints.size());
        SetTextureFilter(m_font.texture, TEXTURE_FILTER_BILINEAR);
        m_fontLoaded = true;
    } else {
        m_font = GetFontDefault();
        m_fontLoaded = false;
    }
}

void AssetManager::drawText(const std::string& text, float x, float y, float fontSize, Color color, float spacing) {
    if (m_fontLoaded) {
        DrawTextEx(m_font, text.c_str(), Vector2{ x, y }, fontSize, spacing, color);
    } else {
        DrawText(text.c_str(), (int)x, (int)y, (int)fontSize, color);
    }
}

float AssetManager::measureText(const std::string& text, float fontSize, float spacing) {
    if (m_fontLoaded) {
        return MeasureTextEx(m_font, text.c_str(), fontSize, spacing).x;
    }
    return (float)MeasureText(text.c_str(), (int)fontSize);
}

void AssetManager::init() {
    if (m_initialized) return;

    // Nạp Font chữ tiếng Việt UTF-8
    loadVietnameseFont("C:/Windows/Fonts/segoeui.ttf");

    // 1. Sinh Texture cho 3 Lớp nhân vật Hero
    std::string heroClasses[] = { "Warrior", "Mage", "Ranger" };
    for (const auto& hClass : heroClasses) {
        Image img = generateHeroImage(hClass);
        m_heroTextures[hClass] = LoadTextureFromImage(img);
        SetTextureFilter(m_heroTextures[hClass], TEXTURE_FILTER_POINT);
        UnloadImage(img);
    }

    // 2. Sinh Texture cho Quái vật & Bosses
    std::string enemies[] = { 
        "goblin", "orc", "armored_orc", "dragon", "ancient_dragon", 
        "shadow_lord", "king", "elena", "skeleton", "wolf" 
    };
    for (const auto& eId : enemies) {
        Image img = generateEnemyImage(eId);
        m_enemyTextures[eId] = LoadTextureFromImage(img);
        SetTextureFilter(m_enemyTextures[eId], TEXTURE_FILTER_POINT);
        UnloadImage(img);
    }

    // 3. Sinh Texture cho các Icon Vật Phẩm
    std::string items[] = { 
        "potion_hp", "potion_mp", "potion_elixir", "sword", 
        "staff", "bow", "armor", "gold", "badge", "book", "ring" 
    };
    for (const auto& iId : items) {
        Image img = generateItemIconImage(iId);
        m_itemTextures[iId] = LoadTextureFromImage(img);
        SetTextureFilter(m_itemTextures[iId], TEXTURE_FILTER_POINT);
        UnloadImage(img);
    }

    m_initialized = true;
}

void AssetManager::cleanup() {
    if (!m_initialized) return;

    if (m_fontLoaded) {
        UnloadFont(m_font);
        m_fontLoaded = false;
    }

    for (auto& pair : m_heroTextures) UnloadTexture(pair.second);
    for (auto& pair : m_enemyTextures) UnloadTexture(pair.second);
    for (auto& pair : m_itemTextures) UnloadTexture(pair.second);

    m_heroTextures.clear();
    m_enemyTextures.clear();
    m_itemTextures.clear();
    m_initialized = false;
}

Texture2D AssetManager::getHeroTexture(const std::string& heroClass) {
    auto it = m_heroTextures.find(heroClass);
    if (it != m_heroTextures.end()) return it->second;
    return m_heroTextures["Warrior"];
}

Texture2D AssetManager::getEnemyTexture(const std::string& enemyId) {
    // Tìm kiếm tương đối nếu ID có chứa từ khóa
    std::string lower = enemyId;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower.find("dragon") != std::string::npos) return m_enemyTextures["ancient_dragon"];
    if (lower.find("shadow") != std::string::npos || lower.find("lord") != std::string::npos) return m_enemyTextures["shadow_lord"];
    if (lower.find("orc") != std::string::npos) return m_enemyTextures["armored_orc"];
    if (lower.find("king") != std::string::npos) return m_enemyTextures["king"];
    if (lower.find("elena") != std::string::npos) return m_enemyTextures["elena"];
    if (lower.find("wolf") != std::string::npos) return m_enemyTextures["wolf"];
    if (lower.find("skeleton") != std::string::npos) return m_enemyTextures["skeleton"];

    auto it = m_enemyTextures.find(lower);
    if (it != m_enemyTextures.end()) return it->second;
    return m_enemyTextures["goblin"];
}

Texture2D AssetManager::getItemIcon(const std::string& itemId) {
    std::string lower = itemId;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower.find("hp") != std::string::npos || lower.find("health") != std::string::npos || lower.find("mau") != std::string::npos) return m_itemTextures["potion_hp"];
    if (lower.find("mp") != std::string::npos || lower.find("mana") != std::string::npos) return m_itemTextures["potion_mp"];
    if (lower.find("elixir") != std::string::npos) return m_itemTextures["potion_elixir"];
    if (lower.find("sword") != std::string::npos || lower.find("kiem") != std::string::npos || lower.find("blade") != std::string::npos) return m_itemTextures["sword"];
    if (lower.find("staff") != std::string::npos || lower.find("truong") != std::string::npos || lower.find("wand") != std::string::npos) return m_itemTextures["staff"];
    if (lower.find("bow") != std::string::npos || lower.find("cung") != std::string::npos) return m_itemTextures["bow"];
    if (lower.find("armor") != std::string::npos || lower.find("giap") != std::string::npos || lower.find("shield") != std::string::npos) return m_itemTextures["armor"];
    if (lower.find("gold") != std::string::npos || lower.find("vang") != std::string::npos || lower.find("coin") != std::string::npos) return m_itemTextures["gold"];
    if (lower.find("badge") != std::string::npos || lower.find("huy_hieu") != std::string::npos) return m_itemTextures["badge"];
    if (lower.find("book") != std::string::npos || lower.find("codex") != std::string::npos || lower.find("diary") != std::string::npos) return m_itemTextures["book"];

    auto it = m_itemTextures.find(lower);
    if (it != m_itemTextures.end()) return it->second;
    return m_itemTextures["potion_hp"];
}

// ----------------- Procedural Pixel Art Generators -----------------

Image AssetManager::generateHeroImage(const std::string& heroClass) {
    int w = 24, h = 24;
    Image img = GenImageColor(w, h, BLANK);

    Color skin = Color{ 255, 218, 185, 255 };
    Color shadowSkin = Color{ 210, 160, 130, 255 };
    Color eyes = Color{ 20, 20, 20, 255 };

    if (heroClass == "Warrior") {
        // Mũ giáp sắt xám
        ImageDrawRectangle(&img, 7, 2, 10, 6, Color{ 140, 150, 165, 255 });
        ImageDrawRectangle(&img, 6, 4, 12, 3, Color{ 110, 120, 135, 255 });
        // Khe ngắm mắt đỏ/xanh rực sáng
        ImageDrawRectangle(&img, 8, 5, 8, 2, Color{ 60, 180, 255, 255 });

        // Mặt và cằm
        ImageDrawRectangle(&img, 8, 8, 8, 3, skin);

        // Áo giáp ngực màu Xanh Royal Cobalt với viền vàng
        ImageDrawRectangle(&img, 6, 11, 12, 7, Color{ 41, 74, 158, 255 });
        ImageDrawRectangle(&img, 8, 12, 8, 5, Color{ 58, 98, 196, 255 });
        ImageDrawRectangle(&img, 10, 13, 4, 3, Color{ 230, 190, 50, 255 }); // Huy hiệu vàng

        // Giáp vai lớn (Pauldrons)
        ImageDrawRectangle(&img, 4, 11, 3, 4, Color{ 160, 170, 185, 255 });
        ImageDrawRectangle(&img, 17, 11, 3, 4, Color{ 160, 170, 185, 255 });

        // Thắt lưng da & chân giáp
        ImageDrawRectangle(&img, 7, 18, 10, 2, Color{ 100, 50, 20, 255 });
        ImageDrawRectangle(&img, 7, 20, 4, 4, Color{ 90, 100, 115, 255 });
        ImageDrawRectangle(&img, 13, 20, 4, 4, Color{ 90, 100, 115, 255 });

        // Thanh Đại Kiếm (Claymore) bên tay phải
        ImageDrawRectangle(&img, 19, 6, 2, 12, Color{ 220, 230, 245, 255 });
        ImageDrawRectangle(&img, 18, 18, 4, 2, Color{ 220, 170, 30, 255 }); // Chuôi kiếm vàng
        ImageDrawRectangle(&img, 19, 20, 2, 3, Color{ 90, 50, 20, 255 });
    }
    else if (heroClass == "Mage") {
        // Mũ Phù Thủy chóp nhọn màu Tím Huyền Bí
        ImageDrawPixel(&img, 11, 1, Color{ 140, 60, 180, 255 });
        ImageDrawRectangle(&img, 10, 2, 3, 2, Color{ 140, 60, 180, 255 });
        ImageDrawRectangle(&img, 9, 4, 5, 2, Color{ 120, 45, 160, 255 });
        ImageDrawRectangle(&img, 5, 6, 14, 2, Color{ 95, 30, 135, 255 }); // Vành mũ rộng
        ImageDrawRectangle(&img, 9, 5, 5, 1, Color{ 240, 200, 60, 255 });  // Đai vàng mũ

        // Khuôn mặt & mắt pháp sư sáng xanh
        ImageDrawRectangle(&img, 8, 8, 8, 4, skin);
        ImageDrawPixel(&img, 10, 9, Color{ 0, 220, 255, 255 });
        ImageDrawPixel(&img, 13, 9, Color{ 0, 220, 255, 255 });

        // Áo choàng pháp sư Tím Thẫm / Xanh Lam
        ImageDrawRectangle(&img, 6, 12, 12, 10, Color{ 110, 40, 150, 255 });
        ImageDrawRectangle(&img, 8, 12, 8, 10, Color{ 80, 25, 115, 255 });
        ImageDrawRectangle(&img, 10, 13, 4, 9, Color{ 0, 180, 220, 255 }); // Dải lụa ma pháp

        // Quyền Trượng Ma Thuật với ngọc phát sáng
        ImageDrawRectangle(&img, 19, 5, 2, 17, Color{ 130, 80, 40, 255 });
        ImageDrawRectangle(&img, 18, 2, 4, 4, Color{ 0, 240, 255, 255 });   // Viên ngọc xanh
        ImageDrawPixel(&img, 19, 3, Color{ 255, 255, 255, 255 });           // Điểm sáng lấp lánh
    }
    else { // Ranger
        // Mũ trùm đầu (Cowl) màu Xanh Lá Rừng Rậm
        ImageDrawRectangle(&img, 7, 2, 10, 6, Color{ 35, 115, 50, 255 });
        ImageDrawRectangle(&img, 6, 4, 12, 4, Color{ 28, 95, 40, 255 });

        // Khuôn mặt
        ImageDrawRectangle(&img, 8, 8, 8, 3, skin);
        ImageDrawPixel(&img, 9, 8, eyes);
        ImageDrawPixel(&img, 13, 8, eyes);
        ImageDrawRectangle(&img, 7, 10, 10, 2, Color{ 45, 135, 60, 255 }); // Khăn che mặt

        // Áo giáp da dẻo dai (Leather Jerkin)
        ImageDrawRectangle(&img, 6, 12, 12, 8, Color{ 120, 75, 40, 255 });
        ImageDrawRectangle(&img, 8, 12, 8, 7, Color{ 150, 95, 50, 255 });
        ImageDrawRectangle(&img, 7, 13, 2, 6, Color{ 35, 115, 50, 255 }); // Quàng khăn xanh
        // Dây cung quàng chéo
        ImageDrawPixel(&img, 8, 13, Color{ 220, 190, 130, 255 });
        ImageDrawPixel(&img, 11, 16, Color{ 220, 190, 130, 255 });
        ImageDrawPixel(&img, 14, 19, Color{ 220, 190, 130, 255 });

        // Quần và ủng da thám hiểm
        ImageDrawRectangle(&img, 7, 20, 4, 4, Color{ 70, 45, 25, 255 });
        ImageDrawRectangle(&img, 13, 20, 4, 4, Color{ 70, 45, 25, 255 });

        // Cung Tên Gỗ bên tay
        ImageDrawPixel(&img, 19, 5, Color{ 180, 110, 50, 255 });
        ImageDrawPixel(&img, 20, 7, Color{ 180, 110, 50, 255 });
        ImageDrawPixel(&img, 20, 15, Color{ 180, 110, 50, 255 });
        ImageDrawPixel(&img, 19, 17, Color{ 180, 110, 50, 255 });
        ImageDrawRectangle(&img, 20, 8, 1, 7, Color{ 180, 110, 50, 255 });
        ImageDrawRectangle(&img, 19, 7, 1, 9, Color{ 230, 230, 230, 255 }); // Dây cung
    }

    return img;
}

Image AssetManager::generateEnemyImage(const std::string& enemyId) {
    if (enemyId == "ancient_dragon" || enemyId == "dragon") {
        // Boss Rồng Cổ Đại Kích thước lớn 48x48
        int w = 48, h = 48;
        Image img = GenImageColor(w, h, BLANK);

        Color darkRed = Color{ 130, 20, 20, 255 };
        Color midRed = Color{ 195, 35, 35, 255 };
        Color brightRed = Color{ 240, 60, 60, 255 };
        Color hornGold = Color{ 230, 185, 45, 255 };
        Color eyeGlow = Color{ 255, 240, 50, 255 };

        // Đôi Cánh Rồng Khổng Lồ
        ImageDrawRectangle(&img, 2, 8, 16, 18, darkRed);
        ImageDrawRectangle(&img, 30, 8, 16, 18, darkRed);
        ImageDrawRectangle(&img, 5, 12, 11, 12, midRed);
        ImageDrawRectangle(&img, 32, 12, 11, 12, midRed);

        // Thân Rồng Vạm Vỡ & Vảy Bụng
        ImageDrawRectangle(&img, 16, 18, 16, 20, midRed);
        ImageDrawRectangle(&img, 19, 22, 10, 14, Color{ 220, 140, 50, 255 }); // Bụng vàng cam

        // Đầu Rồng Uy Dũng
        ImageDrawRectangle(&img, 17, 8, 14, 12, brightRed);
        ImageDrawRectangle(&img, 15, 14, 18, 6, midRed);

        // Sừng Rồng Hoàng Kim
        ImageDrawRectangle(&img, 14, 3, 4, 7, hornGold);
        ImageDrawRectangle(&img, 30, 3, 4, 7, hornGold);
        ImageDrawPixel(&img, 13, 2, hornGold);
        ImageDrawPixel(&img, 34, 2, hornGold);

        // Mắt Rực Lửa
        ImageDrawRectangle(&img, 19, 11, 3, 2, eyeGlow);
        ImageDrawRectangle(&img, 26, 11, 3, 2, eyeGlow);

        // Móng Vuốt & Răng Nanh
        ImageDrawPixel(&img, 17, 19, Color{ 255, 255, 255, 255 });
        ImageDrawPixel(&img, 30, 19, Color{ 255, 255, 255, 255 });
        ImageDrawRectangle(&img, 14, 38, 5, 6, darkRed);
        ImageDrawRectangle(&img, 29, 38, 5, 6, darkRed);
        ImageDrawRectangle(&img, 13, 43, 7, 3, hornGold);
        ImageDrawRectangle(&img, 28, 43, 7, 3, hornGold);

        return img;
    }
    else if (enemyId == "shadow_lord") {
        // Ma Vương Hư Vô Tím Đen (32x32)
        int w = 32, h = 32;
        Image img = GenImageColor(w, h, BLANK);

        Color shadowDark = Color{ 25, 15, 45, 255 };
        Color shadowMid = Color{ 75, 30, 110, 255 };
        Color auraPurple = Color{ 145, 55, 215, 200 };
        Color eyeRed = Color{ 255, 30, 60, 255 };

        // Hào quang bóng tối phát tán
        ImageDrawRectangle(&img, 6, 6, 20, 22, auraPurple);
        ImageDrawRectangle(&img, 8, 4, 16, 24, shadowMid);
        ImageDrawRectangle(&img, 10, 8, 12, 18, shadowDark);

        // Vương Miện Hắc Ám
        ImageDrawRectangle(&img, 10, 2, 3, 4, shadowDark);
        ImageDrawRectangle(&img, 15, 1, 3, 5, shadowDark);
        ImageDrawRectangle(&img, 19, 2, 3, 4, shadowDark);

        // Đôi mắt đỏ phát sáng
        ImageDrawRectangle(&img, 12, 12, 3, 2, eyeRed);
        ImageDrawRectangle(&img, 17, 12, 3, 2, eyeRed);

        // Trái tim bóng tối lơ lửng
        ImageDrawRectangle(&img, 14, 18, 4, 4, Color{ 220, 0, 100, 255 });

        return img;
    }
    else if (enemyId == "king") {
        // Nhà Vua Eldoria (24x24)
        int w = 24, h = 24;
        Image img = GenImageColor(w, h, BLANK);

        Color gold = Color{ 255, 215, 0, 255 };
        Color royalRed = Color{ 180, 20, 30, 255 };
        Color skin = Color{ 255, 218, 185, 255 };
        Color beard = Color{ 230, 230, 230, 255 };

        // Vương miện Vàng đính hồng ngọc
        ImageDrawRectangle(&img, 7, 2, 10, 4, gold);
        ImageDrawPixel(&img, 7, 1, gold);
        ImageDrawPixel(&img, 11, 0, gold);
        ImageDrawPixel(&img, 16, 1, gold);
        ImageDrawPixel(&img, 11, 3, Color{ 255, 0, 50, 255 }); // Ruby

        // Khuôn mặt & râu trắng uy nghiêm
        ImageDrawRectangle(&img, 8, 6, 8, 4, skin);
        ImageDrawPixel(&img, 9, 7, BLACK);
        ImageDrawPixel(&img, 14, 7, BLACK);
        ImageDrawRectangle(&img, 7, 10, 10, 4, beard); // Bộ râu hoàng gia

        // Áo choàng Nhung Đỏ & Lông Thú Trắng
        ImageDrawRectangle(&img, 5, 14, 14, 8, royalRed);
        ImageDrawRectangle(&img, 8, 14, 8, 2, beard);
        ImageDrawRectangle(&img, 10, 16, 4, 6, gold); // Chuỗi ngọc vàng

        return img;
    }
    else if (enemyId == "armored_orc" || enemyId == "orc") {
        // Orc Chiến Binh (32x32)
        int w = 32, h = 32;
        Image img = GenImageColor(w, h, BLANK);

        Color orcSkin = Color{ 60, 120, 50, 255 };
        Color iron = Color{ 120, 130, 145, 255 };
        Color rust = Color{ 140, 80, 40, 255 };

        // Mũ giáp có sừng
        ImageDrawRectangle(&img, 10, 4, 12, 8, iron);
        ImageDrawRectangle(&img, 6, 3, 4, 4, rust);  // Sừng trái
        ImageDrawRectangle(&img, 22, 3, 4, 4, rust); // Sừng phải

        // Khuôn mặt orc hung tợn với răng nanh
        ImageDrawRectangle(&img, 11, 10, 10, 5, orcSkin);
        ImageDrawPixel(&img, 13, 11, Color{ 255, 50, 50, 255 });
        ImageDrawPixel(&img, 18, 11, Color{ 255, 50, 50, 255 });
        ImageDrawPixel(&img, 12, 14, WHITE); // Răng nanh
        ImageDrawPixel(&img, 19, 14, WHITE);

        // Giáp sắt gai góc & cơ bắp
        ImageDrawRectangle(&img, 8, 15, 16, 11, iron);
        ImageDrawRectangle(&img, 5, 15, 4, 6, iron);
        ImageDrawRectangle(&img, 23, 15, 4, 6, iron);

        // Chùy gai khổng lồ
        ImageDrawRectangle(&img, 25, 6, 4, 8, iron);
        ImageDrawRectangle(&img, 26, 14, 2, 12, rust);

        return img;
    }
    else {
        // Goblin Yêu Tinh (24x24)
        int w = 24, h = 24;
        Image img = GenImageColor(w, h, BLANK);

        Color goblinGreen = Color{ 85, 165, 60, 255 };
        Color darkGreen = Color{ 50, 110, 35, 255 };

        // Tai nhọn dài
        ImageDrawPixel(&img, 4, 6, goblinGreen);
        ImageDrawPixel(&img, 5, 7, goblinGreen);
        ImageDrawPixel(&img, 18, 7, goblinGreen);
        ImageDrawPixel(&img, 19, 6, goblinGreen);

        // Đầu Goblin
        ImageDrawRectangle(&img, 6, 5, 12, 8, goblinGreen);
        ImageDrawRectangle(&img, 7, 6, 3, 3, Color{ 255, 200, 0, 255 }); // Mắt vàng
        ImageDrawPixel(&img, 8, 7, Color{ 180, 0, 0, 255 });
        ImageDrawRectangle(&img, 14, 6, 3, 3, Color{ 255, 200, 0, 255 });
        ImageDrawPixel(&img, 15, 7, Color{ 180, 0, 0, 255 });

        // Miệng ngoác cười
        ImageDrawRectangle(&img, 9, 11, 6, 2, darkGreen);
        ImageDrawPixel(&img, 10, 11, WHITE);

        // Thân & Khố Da
        ImageDrawRectangle(&img, 7, 13, 10, 6, goblinGreen);
        ImageDrawRectangle(&img, 7, 18, 10, 3, Color{ 130, 80, 40, 255 });

        // Dao găm rỉ sét
        ImageDrawRectangle(&img, 18, 11, 2, 7, Color{ 180, 180, 190, 255 });
        ImageDrawRectangle(&img, 17, 18, 4, 2, Color{ 100, 50, 20, 255 });

        return img;
    }
}

Image AssetManager::generateItemIconImage(const std::string& itemId) {
    int w = 16, h = 16;
    Image img = GenImageColor(w, h, BLANK);

    if (itemId == "potion_hp") {
        // Bình Máu Đỏ (Ruby Potion)
        ImageDrawRectangle(&img, 6, 2, 4, 2, Color{ 160, 110, 60, 255 }); // Nắp bấc
        ImageDrawRectangle(&img, 5, 4, 6, 2, Color{ 200, 220, 240, 200 }); // Cổ chai thuỷ tinh
        ImageDrawRectangle(&img, 3, 6, 10, 8, Color{ 220, 30, 40, 255 });  // Thân bình đỏ rực
        ImageDrawRectangle(&img, 4, 7, 8, 6, Color{ 255, 60, 70, 255 });
        ImageDrawPixel(&img, 5, 7, Color{ 255, 200, 200, 255 });          // Điểm sáng phản chiếu
        ImageDrawPixel(&img, 6, 8, Color{ 255, 200, 200, 255 });
    }
    else if (itemId == "potion_mp") {
        // Bình Năng Lượng Xanh (Mana Potion)
        ImageDrawRectangle(&img, 6, 2, 4, 2, Color{ 160, 110, 60, 255 });
        ImageDrawRectangle(&img, 5, 4, 6, 2, Color{ 200, 220, 240, 200 });
        ImageDrawRectangle(&img, 3, 6, 10, 8, Color{ 20, 80, 220, 255 });  // Thân bình xanh biển
        ImageDrawRectangle(&img, 4, 7, 8, 6, Color{ 40, 130, 255, 255 });
        ImageDrawPixel(&img, 5, 7, Color{ 200, 240, 255, 255 });
        ImageDrawPixel(&img, 6, 8, Color{ 200, 240, 255, 255 });
    }
    else if (itemId == "potion_elixir") {
        // Bình Vạn Năng Hoàng Kim (Elixir)
        ImageDrawRectangle(&img, 6, 2, 4, 2, Color{ 255, 215, 0, 255 });
        ImageDrawRectangle(&img, 3, 5, 10, 9, Color{ 255, 190, 20, 255 });
        ImageDrawRectangle(&img, 4, 6, 8, 7, Color{ 255, 240, 100, 255 });
        ImageDrawPixel(&img, 7, 9, Color{ 255, 255, 255, 255 });
    }
    else if (itemId == "sword") {
        // Thanh Kiếm Hiệp Sĩ (Knight Sword)
        ImageDrawPixel(&img, 13, 2, Color{ 240, 245, 255, 255 });
        ImageDrawRectangle(&img, 9, 3, 4, 4, Color{ 210, 220, 235, 255 });
        ImageDrawRectangle(&img, 6, 6, 4, 4, Color{ 180, 190, 210, 255 });
        // Chuôi kiếm vàng
        ImageDrawRectangle(&img, 4, 10, 4, 2, Color{ 240, 190, 40, 255 });
        ImageDrawRectangle(&img, 5, 9, 2, 4, Color{ 240, 190, 40, 255 });
        ImageDrawRectangle(&img, 2, 12, 3, 3, Color{ 120, 60, 20, 255 });
    }
    else if (itemId == "staff") {
        // Gậy Pháp Thuật (Arcane Staff)
        ImageDrawRectangle(&img, 9, 2, 5, 5, Color{ 0, 210, 255, 255 });
        ImageDrawPixel(&img, 11, 4, Color{ 255, 255, 255, 255 });
        ImageDrawRectangle(&img, 4, 7, 8, 7, Color{ 130, 80, 40, 255 });
        ImageDrawRectangle(&img, 2, 13, 3, 2, Color{ 100, 60, 30, 255 });
    }
    else if (itemId == "bow") {
        // Cung Tên Săn Bắn (Hunter Bow)
        ImageDrawRectangle(&img, 4, 2, 8, 2, Color{ 150, 90, 40, 255 });
        ImageDrawRectangle(&img, 2, 4, 2, 8, Color{ 150, 90, 40, 255 });
        ImageDrawRectangle(&img, 4, 12, 8, 2, Color{ 150, 90, 40, 255 });
        ImageDrawRectangle(&img, 12, 4, 1, 8, Color{ 240, 240, 240, 255 }); // Dây
    }
    else if (itemId == "armor") {
        // Áo Giáp Sắt (Plate Armor)
        ImageDrawRectangle(&img, 3, 3, 10, 10, Color{ 120, 135, 155, 255 });
        ImageDrawRectangle(&img, 5, 5, 6, 7, Color{ 160, 175, 195, 255 });
        ImageDrawRectangle(&img, 7, 7, 2, 3, Color{ 230, 190, 40, 255 });
    }
    else if (itemId == "gold") {
        // Đống Tiền Vàng (Gold Coins)
        ImageDrawRectangle(&img, 2, 8, 6, 6, Color{ 220, 170, 20, 255 });
        ImageDrawRectangle(&img, 7, 6, 7, 8, Color{ 255, 210, 40, 255 });
        ImageDrawPixel(&img, 9, 8, Color{ 255, 245, 180, 255 });
    }
    else if (itemId == "badge") {
        // Huy Hiệu Hoàng Gia Eldoria
        ImageDrawRectangle(&img, 3, 3, 10, 10, Color{ 230, 180, 30, 255 });
        ImageDrawRectangle(&img, 5, 5, 6, 6, Color{ 40, 70, 150, 255 });
        ImageDrawPixel(&img, 7, 7, Color{ 255, 255, 255, 255 });
    }
    else { // Sách / Codex
        ImageDrawRectangle(&img, 3, 3, 10, 11, Color{ 120, 30, 40, 255 });
        ImageDrawRectangle(&img, 5, 4, 7, 9, Color{ 235, 225, 195, 255 });
        ImageDrawRectangle(&img, 6, 6, 5, 1, Color{ 80, 70, 50, 255 });
        ImageDrawRectangle(&img, 6, 8, 5, 1, Color{ 80, 70, 50, 255 });
    }

    return img;
}

// ----------------- UI Drawing Helpers -----------------

void AssetManager::drawNineSliceBox(Rectangle bounds, Color bgColor, Color borderColor) {
    // 1. Vẽ nền trong suốt mờ
    DrawRectangleRec(bounds, bgColor);

    // 2. Viền ngoài kép pixel chuẩn Retro
    DrawRectangleLinesEx(bounds, 1.0f, borderColor);

    Rectangle inner = { bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4 };
    DrawRectangleLinesEx(inner, 1.0f, Color{ (unsigned char)(borderColor.r * 0.5f), (unsigned char)(borderColor.g * 0.5f), (unsigned char)(borderColor.b * 0.5f), 255 });

    // 4 Góc trang trí vàng óng (Corner Accents)
    DrawRectangle(bounds.x, bounds.y, 3, 3, borderColor);
    DrawRectangle(bounds.x + bounds.width - 3, bounds.y, 3, 3, borderColor);
    DrawRectangle(bounds.x, bounds.y + bounds.height - 3, 3, 3, borderColor);
    DrawRectangle(bounds.x + bounds.width - 3, bounds.y + bounds.height - 3, 3, 3, borderColor);
}

void AssetManager::drawStatBar(float x, float y, float width, float height, int current, int max, Color fillColor, Color bgColor, const char* label) {
    if (max <= 0) max = 1;
    float ratio = std::max(0.0f, std::min(1.0f, (float)current / max));

    // Khung nền đen
    DrawRectangle(x, y, width, height, bgColor);

    // Thanh màu nạp
    DrawRectangle(x + 1, y + 1, (width - 2) * ratio, height - 2, fillColor);

    // Viền pixel
    DrawRectangleLines(x, y, width, height, Color{ 180, 180, 180, 255 });

    // Nhãn chữ số nếu có
    if (label) {
        DrawText(label, x + 3, y - 9, 8, LIGHTGRAY);
    }
}

void AssetManager::drawItemIcon(const std::string& itemId, float x, float y, float size) {
    Texture2D tex = getItemIcon(itemId);
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { x, y, size, size };
    DrawTexturePro(tex, src, dst, Vector2{ 0, 0 }, 0.0f, WHITE);
}

void AssetManager::drawEntitySprite(Texture2D texture, float x, float y, float scale, bool flipX, bool isHurt) {
    Rectangle src = { 0, 0, (float)(flipX ? -texture.width : texture.width), (float)texture.height };
    Rectangle dst = { x, y, (float)texture.width * scale, (float)texture.height * scale };
    Color tint = isHurt ? Color{ 255, 80, 80, 255 } : WHITE;
    DrawTexturePro(texture, src, dst, Vector2{ 0, 0 }, 0.0f, tint);
}
