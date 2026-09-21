/**
 * @file play_overworld_2d.cpp
 * @brief Trải nghiệm Bản Đồ Thế Giới Khám Phá 2D Top-Down (Step 5)
 */

#include "ui_2d/PixelCanvas.h"
#include "ui_2d/AssetManager.h"
#include "ui_2d/OverworldScene2D.h"
#include "ui_2d/StoryScene2D.h"
#include "ui_2d/BattleScene2D.h"
#include "Warrior.h"
#include "BossMonster.h"
#include "StoryGraph.h"
#include <iostream>

enum class GameMode {
    OVERWORLD,
    STORY_DIALOGUE,
    BATTLE
};

int main() {
    StoryGraph story;
    if (!story.loadStoryGraph("data/story.json")) {
        std::cerr << "Không thể nạp data/story.json!\n";
        return 1;
    }

    Warrior hero("Arthur", 150, 40, 25, 20);
    hero.setGold(150);

    // Canvas ảo 480x288 pixel chuẩn tỷ lệ 30x18 tiles (16x16)
    PixelCanvas canvas(480, 288, 3);
    if (!canvas.init("RPG Engine - 2D Pixel Overworld Exploration (Step 5)")) {
        return 1;
    }

    AssetManager& assets = AssetManager::getInstance();
    assets.init();

    OverworldScene2D overworld(hero, story);
    StoryScene2D storyScene(story);
    storyScene.setHeroClass(hero.getHeroClassName());
    storyScene.setHeroGold(hero.getGold());

    GameMode mode = GameMode::OVERWORLD;
    BossMonster boss("ancient_dragon", "Ancient Dragon", 200, 25, 12, 120, 100);
    std::unique_ptr<BattleScene2D> battleScene = nullptr;

    while (canvas.isRunning()) {
        float dt = GetFrameTime();
        Vector2 vMouse = canvas.getVirtualMousePosition();

        if (mode == GameMode::OVERWORLD) {
            overworld.update(dt);

            if (overworld.hasTriggeredStory()) {
                std::string targetNode = overworld.getTriggeredNodeId();
                story.moveToNode(targetNode);
                storyScene.refreshNode();
                overworld.clearStoryTrigger();
                mode = GameMode::STORY_DIALOGUE;
            }
            else if (overworld.hasTriggeredBattle()) {
                overworld.clearBattleTrigger();
                battleScene = std::make_unique<BattleScene2D>(hero, boss);
                mode = GameMode::BATTLE;
            }
        }
        else if (mode == GameMode::STORY_DIALOGUE) {
            storyScene.update(dt, vMouse);

            // Bấm ESC hoặc hết hội thoại để quay lại Overworld
            if (IsKeyPressed(KEY_ESCAPE) || (story.getCurrentNode().choices.empty() && !story.isInDialogue() && IsKeyPressed(KEY_ENTER))) {
                mode = GameMode::OVERWORLD;
            }
        }
        else if (mode == GameMode::BATTLE) {
            if (battleScene) {
                battleScene->update(dt, vMouse);
                if (battleScene->isFinished()) {
                    battleScene = nullptr;
                    mode = GameMode::OVERWORLD;
                }
            }
        }

        // Render Canvas
        canvas.beginCanvas();
        if (mode == GameMode::OVERWORLD) {
            overworld.render();
        } else if (mode == GameMode::STORY_DIALOGUE) {
            storyScene.render();
            DrawText("[ESC]: Thoát về bản đồ", 330, 270, 8, LIGHTGRAY);
        } else if (mode == GameMode::BATTLE && battleScene) {
            battleScene->render();
        }
        canvas.endCanvas();

        canvas.renderToScreen();
    }

    assets.cleanup();
    canvas.close();
    return 0;
}
