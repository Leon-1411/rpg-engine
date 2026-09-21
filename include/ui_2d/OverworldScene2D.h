#ifndef OVERWORLD_SCENE_2D_H
#define OVERWORLD_SCENE_2D_H

#include "Hero.h"
#include "StoryGraph.h"
#include "ui_2d/AssetManager.h"
#include "raylib.h"
#include <string>
#include <vector>

enum class TileType {
    GRASS,
    DIRT_ROAD,
    STONE_ROAD,
    CASTLE_FLOOR,
    WATER,          // Solid
    TREE,           // Solid
    CASTLE_WALL,    // Solid
    LAVA,           // Solid / Hazard
    PORTAL          // Trigger
};

struct WorldEntity {
    std::string id;
    std::string name;
    std::string spriteType; // "king", "merchant", "mage", "dragon", "orc", "elena"
    float x;
    float y;
    std::string triggerNodeId;
    std::string promptText;
    bool isTriggerActive;
};

/**
 * @class OverworldScene2D
 * @brief Quản lý bản đồ thế giới khám phá 2D Top-Down cho RPG Engine.
 * Hỗ trợ di chuyển 4 hướng, va chạm gạch, camera bám theo nhân vật, gặp NPC kích hoạt cốt truyện.
 */
class OverworldScene2D {
public:
    OverworldScene2D(Hero& hero, StoryGraph& storyGraph);
    ~OverworldScene2D() = default;

    // Cập nhật di chuyển nhân vật, va chạm và tương tác mỗi khung hình
    void update(float dt);

    // Vẽ toàn bộ bản đồ thế giới, NPC và nhân vật người chơi
    void render();

    // Kiểm tra xem người chơi vừa kích hoạt tương tác với Node cốt truyện nào
    bool hasTriggeredStory() const { return m_storyTriggered; }
    std::string getTriggeredNodeId() const { return m_triggeredNodeId; }
    void clearStoryTrigger() { m_storyTriggered = false; }

    // Kiểm tra xem người chơi có bước vào trận chiến Boss không
    bool hasTriggeredBattle() const { return m_battleTriggered; }
    std::string getTriggeredEnemyId() const { return m_triggeredEnemyId; }
    void clearBattleTrigger() { m_battleTriggered = false; }

    // Kiểm tra mở túi đồ hoặc mở shop
    bool hasTriggeredInventory() const { return m_inventoryTriggered; }
    void clearInventoryTrigger() { m_inventoryTriggered = false; }
    bool hasTriggeredShop() const { return m_shopTriggered; }
    void clearShopTrigger() { m_shopTriggered = false; }

    void setPlayerPosition(float x, float y) { m_playerX = x; m_playerY = y; }

private:
    void initWorldMap();
    bool isSolidTile(int tileX, int tileY) const;
    void drawTilemap();
    void drawEntities();
    void drawPlayer();
    void drawWorldHUD();
    void drawInteractionPrompt();

    Hero& m_hero;
    StoryGraph& m_storyGraph;

    // Bản đồ lưới Tilemap (30 x 18 tiles, mỗi ô 16x16 pixel = 480x288 pixel)
    static const int MAP_WIDTH = 30;
    static const int MAP_HEIGHT = 18;
    static const int TILE_SIZE = 16;
    TileType m_map[MAP_HEIGHT][MAP_WIDTH];

    // Vị trí & di chuyển nhân vật người chơi
    float m_playerX;
    float m_playerY;
    float m_speed;
    int m_facingDirection; // 0: Down, 1: Up, 2: Left, 3: Right
    float m_walkAnimTimer;
    int m_walkFrame;
    bool m_isMoving;

    // Danh sách NPC và điểm tương tác
    std::vector<WorldEntity> m_entities;
    int m_activeEntityIndex; // -1 nếu không đứng gần ai

    // Cờ kích hoạt chuyển Scene
    bool m_storyTriggered;
    std::string m_triggeredNodeId;
    bool m_battleTriggered;
    std::string m_triggeredEnemyId;
    bool m_inventoryTriggered;
    bool m_shopTriggered;

    // Hiệu ứng mờ môi trường
    float m_ambientTimer;
};

#endif // OVERWORLD_SCENE_2D_H
