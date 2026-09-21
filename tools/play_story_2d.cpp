/**
 * @file play_story_2d.cpp
 * @brief Chương trình trải nghiệm cốt truyện 2D Pixel tương tác cho RPG Engine (Step 3)
 */

#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include "ui_2d/StoryScene2D.h"
#include "StoryGraph.h"
#include <iostream>

int main() {
    StoryGraph story;
    if (!story.loadStoryGraph("data/story.json")) {
        std::cerr << "Không thể nạp data/story.json!\n";
        return 1;
    }

    // Canvas ảo 480x270 tỉ lệ 16:9, scale 3x lên 1440x810
    PixelCanvas canvas(480, 270, 3);
    if (!canvas.init("RPG Engine - 2D Pixel Story Player (Step 3)")) {
        return 1;
    }

    AssetManager& assets = AssetManager::getInstance();
    assets.init();

    StoryScene2D storyScene(story);
    storyScene.setHeroClass("Warrior"); // Mặc định là Chiến Binh
    storyScene.setHeroGold(100);

    while (canvas.isRunning()) {
        float dt = GetFrameTime();
        Vector2 vMouse = canvas.getVirtualMousePosition();

        // Đổi class để xem giao diện phản ứng: Phím 7 (Warrior), 8 (Mage), 9 (Ranger)
        if (IsKeyPressed(KEY_F1)) storyScene.setHeroClass("Warrior");
        if (IsKeyPressed(KEY_F2)) storyScene.setHeroClass("Mage");
        if (IsKeyPressed(KEY_F3)) storyScene.setHeroClass("Ranger");

        // Cập nhật logic cốt truyện và tương tác
        storyScene.update(dt, vMouse);

        // 1. Render lên Pixel Canvas ảo
        canvas.beginCanvas();
        storyScene.render();

        // Hiển thị phím tắt trợ giúp
        DrawText("[F1]: Warrior | [F2]: Mage | [F3]: Ranger | [Mũi tên/1-9/Chuột]: Chọn | [ESC]: Thoát", 30, 260, 7, GRAY);
        canvas.endCanvas();

        // 2. Render Canvas ra màn hình thực
        canvas.renderToScreen();
    }

    assets.cleanup();
    canvas.close();
    return 0;
}
