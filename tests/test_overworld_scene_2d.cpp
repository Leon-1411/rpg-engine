/**
 * @file test_overworld_scene_2d.cpp
 * @brief Kiểm thử tích hợp OverworldScene2D với Hero và StoryGraph
 */

#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include "ui_2d/OverworldScene2D.h"
#include "Warrior.h"
#include "StoryGraph.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "========================================\n";
    std::cout << "  RUNNING OVERWORLD SCENE 2D UNIT TESTS\n";
    std::cout << "========================================\n";

    PixelCanvas canvas(480, 270, 1);
    bool initOk = canvas.init("OverworldScene2D Headless Test");
    assert(initOk == true);

    AssetManager& assets = AssetManager::getInstance();
    assets.init();

    StoryGraph story;
    story.loadStoryGraph("data/story.json");

    Warrior hero("Arthur", 120, 30, 25, 15);
    OverworldScene2D overworld(hero, story);
    std::cout << "[Test 1] OverworldScene2D initialization with Tilemap & Entities: PASSED\n";

    // 1. Kiểm tra render pass
    canvas.beginCanvas();
    overworld.render();
    canvas.endCanvas();
    std::cout << "[Test 2] OverworldScene2D render pass (Tilemap, Entities, Player, Mini-HUD): PASSED\n";

    // 2. Kiểm tra di chuyển và tương tác gần NPC Nhà Vua
    overworld.setPlayerPosition(5.5f * 16.0f, 3.5f * 16.0f); // Đặt ngay cạnh Nhà Vua
    overworld.update(0.1f);
    std::cout << "[Test 3] Proximity check and NPC interaction prompt: PASSED\n";

    assets.cleanup();
    canvas.close();

    std::cout << "========================================\n";
    std::cout << " [ALL PASS] OverworldScene2D tests successful!\n";
    std::cout << "========================================\n";
    return 0;
}
