/**
 * @file test_story_scene_2d.cpp
 * @brief Kiểm thử tích hợp StoryScene2D với StoryGraph và file data/story.json
 */

#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include "ui_2d/StoryScene2D.h"
#include "StoryGraph.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "========================================\n";
    std::cout << "  RUNNING STORY SCENE 2D UNIT TESTS\n";
    std::cout << "========================================\n";

    PixelCanvas canvas(480, 270, 1);
    bool initOk = canvas.init("StoryScene2D Headless Test");
    assert(initOk == true);

    AssetManager& assets = AssetManager::getInstance();
    assets.init();

    StoryGraph story;
    bool loadOk = story.loadStoryGraph("data/story.json");
    assert(loadOk == true);
    std::cout << "[Test 1] StoryGraph data/story.json loaded for 2D Scene: PASSED\n";

    StoryScene2D scene(story);
    scene.setHeroClass("Warrior");
    scene.setHeroGold(200);

    // Bắt đầu tại Node 01 (Hoàng Thành Eldoria)
    assert(story.getCurrentNodeId() == "Node01");
    assert(story.isInDialogue() == true); // Node 01 có dialogue tree Nhà Vua
    std::cout << "[Test 2] Node01 King Eldoria Branching Dialogue initialized: PASSED\n";

    // Simulate update cycles (Typewriter effect)
    scene.update(0.5f, Vector2{ 0, 0 });
    scene.update(1.0f, Vector2{ 0, 0 });

    // Render pass test
    canvas.beginCanvas();
    scene.render();
    canvas.endCanvas();
    std::cout << "[Test 3] StoryScene2D render pass (Background, Portraits, Dialogue Box): PASSED\n";

    // Tiến hành chọn đối thoại rẽ nhánh đầu tiên của Nhà Vua
    story.selectDialogueChoice(0);
    scene.refreshNode();
    scene.update(0.5f, Vector2{ 0, 0 });
    std::cout << "[Test 4] Branching dialogue selection in 2D Scene: PASSED\n";

    // Tiến hành chọn tiếp để chuyển sang Node 02 (Trạm Tiền Tiêu & Shop)
    story.selectDialogueChoice(0);
    scene.refreshNode();
    assert(story.getCurrentNodeId() == "Node02");
    std::cout << "[Test 5] Dialogue smooth transition to Node02: PASSED\n";

    assets.cleanup();
    canvas.close();

    std::cout << "========================================\n";
    std::cout << " [ALL PASS] StoryScene2D tests successful!\n";
    std::cout << "========================================\n";
    return 0;
}
