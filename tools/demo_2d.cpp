/**
 * @file demo_2d.cpp
 * @brief Chương trình kiểm thử và demo đồ họa 2D Pixel Canvas cho RPG Engine
 */

#include "ui_2d/PixelCanvas.h"
#include "raylib.h"
#include <cmath>
#include <string>

int main() {
    // Canvas ảo 480x270, tỉ lệ 16:9, scale 3x lên 1440x810
    PixelCanvas canvas(480, 270, 3);

    if (!canvas.init("RPG Engine - 2D Pixel Art Demo (Step 1)")) {
        return 1;
    }

    float heroX = 100.0f;
    float heroY = 160.0f;
    float animTimer = 0.0f;
    int currentFrame = 0;

    while (canvas.isRunning()) {
        float dt = GetFrameTime();
        animTimer += dt;
        if (animTimer >= 0.15f) {
            animTimer = 0.0f;
            currentFrame = (currentFrame + 1) % 4;
        }

        // Xử lý di chuyển Hero
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) heroX += 80.0f * dt;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) heroX -= 80.0f * dt;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) heroY -= 80.0f * dt;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) heroY += 80.0f * dt;

        Vector2 vMouse = canvas.getVirtualMousePosition();

        // 1. Vẽ lên Pixel Canvas ảo
        canvas.beginCanvas();
        ClearBackground(Color{ 20, 24, 46, 255 }); // Dark Fantasy Blue

        // Vẽ lưới gạch nền thế giới (Tilemap pattern mẫu)
        for (int y = 0; y < 270; y += 16) {
            for (int x = 0; x < 480; x += 16) {
                if ((x / 16 + y / 16) % 2 == 0) {
                    DrawRectangle(x, y, 16, 16, Color{ 26, 32, 58, 255 });
                }
            }
        }

        // Vẽ một số cây rừng 2D Pixel trang trí
        DrawRectangle(40, 60, 24, 40, Color{ 34, 139, 34, 255 });
        DrawRectangle(48, 100, 8, 16, Color{ 101, 67, 33, 255 });

        DrawRectangle(400, 70, 28, 44, Color{ 46, 117, 89, 255 });
        DrawRectangle(410, 114, 8, 16, Color{ 101, 67, 33, 255 });

        // Vẽ NPC Vua Eldoria (Pixel Art tạm mẫu)
        DrawRectangle(320, 150, 20, 28, Color{ 218, 165, 32, 255 }); // Vàng hoàng gia
        DrawRectangle(324, 144, 12, 6, Color{ 255, 215, 0, 255 });   // Vương miện
        DrawText("KING", 316, 132, 10, GOLD);

        // Vẽ Hero (Chiến Binh Pixel Art mẫu) với animation thở nhấp nhô
        int bounce = (currentFrame % 2 == 0) ? 0 : 1;
        DrawRectangle(heroX, heroY - bounce, 18, 26, Color{ 65, 105, 225, 255 }); // Thân áo giáp xanh
        DrawRectangle(heroX + 3, heroY - 8 - bounce, 12, 10, Color{ 255, 220, 177, 255 }); // Mặt
        DrawRectangle(heroX + 18, heroY + 2 - bounce, 10, 3, Color{ 200, 200, 200, 255 }); // Thanh kiếm
        DrawText("HERO (YOU)", heroX - 10, heroY - 20, 10, SKYBLUE);

        // Khung UI Hộp Thoại Pixel 9-Slice phong cách Retro JRPG ở đáy màn hình
        DrawRectangle(20, 190, 440, 70, Color{ 10, 12, 24, 230 });
        DrawRectangleLines(20, 190, 440, 70, Color{ 180, 150, 90, 255 });
        DrawRectangleLines(22, 192, 436, 66, Color{ 80, 60, 40, 255 });

        DrawText("ELDORIA ROYAL DISPATCH - 2D PIXEL ENGINE", 32, 200, 10, GOLD);
        DrawText("Nha Vua: \"Hoi dung si! Hay giai cuu Cong chua Elena!\"", 32, 216, 10, RAYWHITE);
        DrawText("[WASD / Mui ten]: Di chuyen nhan vat | [ESC]: Thoat", 32, 236, 10, GRAY);

        // Hiển thị HUD thông tin toạ độ chuột ảo & FPS
        DrawRectangle(10, 10, 130, 40, Color{ 0, 0, 0, 180 });
        DrawText(TextFormat("FPS: %i", GetFPS()), 16, 16, 10, GREEN);
        DrawText(TextFormat("Canvas: 480x270 (3x)"), 16, 28, 10, LIGHTGRAY);
        DrawText(TextFormat("Hero: (%.0f, %.0f)", heroX, heroY), 16, 38, 10, YELLOW);

        canvas.endCanvas();

        // 2. Render Canvas ảo phóng to lên cửa sổ màn hình thật
        canvas.renderToScreen();
    }

    canvas.close();
    return 0;
}
