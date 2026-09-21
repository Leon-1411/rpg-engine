#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"
#include <string>
#include <unordered_map>

/**
 * @class AssetManager
 * @brief Quản lý tập trung toàn bộ tài nguyên đồ họa 2D Pixel Art cho RPG Engine.
 * Hỗ trợ tạo tự động (Procedural Pixel Art) và nạp file ngoài nếu có.
 */
class AssetManager {
public:
    static AssetManager& getInstance();

    // Khởi tạo và sinh bộ Texture Pixel Art chuẩn
    void init();
    // Giải phóng bộ nhớ toàn bộ textures và fonts
    void cleanup();

    // Nạp font chữ Unicode Tiếng Việt
    void loadVietnameseFont(const char* fontPath = "C:/Windows/Fonts/segoeui.ttf");

    // Lấy Font Unicode tiếng Việt
    Font getFont() const { return m_font; }

    // Vẽ chữ Tiếng Việt chuẩn UTF-8 không bị lỗi dấu
    void drawText(const std::string& text, float x, float y, float fontSize, Color color, float spacing = 1.0f);

    // Tính toán độ dài văn bản Tiếng Việt
    float measureText(const std::string& text, float fontSize, float spacing = 1.0f);

    // Lấy Texture cho Class nhân vật ("Warrior", "Mage", "Ranger")
    Texture2D getHeroTexture(const std::string& heroClass);

    // Lấy Texture cho Quái vật / Boss ("goblin", "orc", "dragon", "shadow_lord", "king", "elena", v.v.)
    Texture2D getEnemyTexture(const std::string& enemyId);

    // Lấy Texture Icon vật phẩm ("potion_hp", "potion_mp", "sword", "staff", "bow", "armor", "gold", "badge")
    Texture2D getItemIcon(const std::string& itemId);

    // Vẽ khung hộp thoại hoặc bảng UI chuẩn Retro 9-Slice
    void drawNineSliceBox(Rectangle bounds, Color bgColor = Color{ 15, 18, 30, 240 }, Color borderColor = Color{ 190, 160, 90, 255 });

    // Vẽ thanh chỉ số HP / MP phong cách Pixel sắc nét
    void drawStatBar(float x, float y, float width, float height, int current, int max, Color fillColor, Color bgColor = Color{ 40, 40, 40, 255 }, const char* label = nullptr);

    // Vẽ Icon vật phẩm với kích thước tuỳ chỉnh
    void drawItemIcon(const std::string& itemId, float x, float y, float size = 16.0f);

    // Vẽ Sprite nhân vật với hiệu ứng nhấp nháy khi trúng đòn
    void drawEntitySprite(Texture2D texture, float x, float y, float scale = 2.0f, bool flipX = false, bool isHurt = false);

private:
    AssetManager();
    ~AssetManager();
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    // Sinh ảnh Pixel Art theo từng chủ đề
    Image generateHeroImage(const std::string& heroClass);
    Image generateEnemyImage(const std::string& enemyId);
    Image generateItemIconImage(const std::string& itemId);

    std::unordered_map<std::string, Texture2D> m_heroTextures;
    std::unordered_map<std::string, Texture2D> m_enemyTextures;
    std::unordered_map<std::string, Texture2D> m_itemTextures;

    Font m_font;
    bool m_fontLoaded;
    bool m_initialized;
};

#endif // ASSET_MANAGER_H
