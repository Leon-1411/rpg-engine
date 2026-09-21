#include "ui_2d/OverworldScene2D.h"
#include <algorithm>
#include <cmath>

OverworldScene2D::OverworldScene2D(Hero& hero, StoryGraph& storyGraph)
    : m_hero(hero)
    , m_storyGraph(storyGraph)
    , m_playerX(220.0f)
    , m_playerY(140.0f)
    , m_speed(90.0f)
    , m_facingDirection(0)
    , m_walkAnimTimer(0.0f)
    , m_walkFrame(0)
    , m_isMoving(false)
    , m_activeEntityIndex(-1)
    , m_storyTriggered(false)
    , m_battleTriggered(false)
    , m_inventoryTriggered(false)
    , m_shopTriggered(false)
    , m_ambientTimer(0.0f)
{
    initWorldMap();
}

void OverworldScene2D::initWorldMap() {
    // 1. Khởi tạo toàn bộ nền cỏ xanh
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            m_map[y][x] = TileType::GRASS;
        }
    }

    // 2. Viền cây rừng bao quanh thế giới
    for (int x = 0; x < MAP_WIDTH; ++x) {
        m_map[0][x] = TileType::TREE;
        m_map[MAP_HEIGHT - 1][x] = TileType::TREE;
    }
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        m_map[y][0] = TileType::TREE;
        m_map[y][MAP_WIDTH - 1] = TileType::TREE;
    }

    // 3. Khu vực Hoàng Thành Eldoria (Góc Tây Bắc: x=1..10, y=1..6)
    for (int y = 1; y <= 6; ++y) {
        for (int x = 1; x <= 10; ++x) {
            m_map[y][x] = TileType::CASTLE_FLOOR;
        }
    }
    // Tường thành Eldoria
    for (int x = 1; x <= 10; ++x) m_map[1][x] = TileType::CASTLE_WALL;
    for (int y = 1; y <= 6; ++y) m_map[y][1] = TileType::CASTLE_WALL;
    for (int y = 1; y <= 6; ++y) m_map[y][10] = TileType::CASTLE_WALL;
    m_map[6][5] = TileType::STONE_ROAD; // Cổng thành mở
    m_map[6][6] = TileType::STONE_ROAD;

    // 4. Con đường mòn đất nối liền các vùng đất
    for (int y = 6; y <= 14; ++y) {
        m_map[y][5] = TileType::DIRT_ROAD;
        m_map[y][6] = TileType::DIRT_ROAD;
    }
    for (int x = 5; x <= 25; ++x) {
        m_map[10][x] = TileType::DIRT_ROAD;
        m_map[11][x] = TileType::DIRT_ROAD;
    }

    // 5. Dòng sông vắt ngang bản đồ (y=8, x=9..20)
    for (int x = 9; x <= 22; ++x) {
        if (x != 14 && x != 15) { // Cầu đá bắc qua sông
            m_map[8][x] = TileType::WATER;
            m_map[9][x] = TileType::WATER;
        } else {
            m_map[8][x] = TileType::STONE_ROAD;
            m_map[9][x] = TileType::STONE_ROAD;
        }
    }

    // 6. Khu Tàn Tích Pháp Viện Cổ (Góc Đông Bắc: x=20..28, y=1..6)
    for (int y = 2; y <= 5; ++y) {
        for (int x = 21; x <= 27; ++x) {
            m_map[y][x] = TileType::STONE_ROAD;
        }
    }
    m_map[2][21] = TileType::CASTLE_WALL;
    m_map[2][27] = TileType::CASTLE_WALL;
    m_map[5][21] = TileType::CASTLE_WALL;
    m_map[5][27] = TileType::CASTLE_WALL;

    // 7. Thung Lũng Nham Thạch Rồng Quỷ (Góc Đông Nam: x=20..28, y=12..16)
    for (int y = 13; y <= 16; ++y) {
        for (int x = 21; x <= 28; ++x) {
            m_map[y][x] = TileType::LAVA;
        }
    }
    m_map[14][24] = TileType::PORTAL; // Cổng dịch chuyển vào hang Boss

    // 8. Thêm các cụm cây rừng trang trí
    m_map[3][13] = TileType::TREE;
    m_map[3][14] = TileType::TREE;
    m_map[4][14] = TileType::TREE;
    m_map[14][2] = TileType::TREE;
    m_map[15][3] = TileType::TREE;
    m_map[15][12] = TileType::TREE;
    m_map[16][13] = TileType::TREE;

    // 9. Khởi tạo danh sách NPC trên thế giới
    m_entities.clear();

    // NPC 1: Nhà Vua Eldoria trong đại điện
    WorldEntity king;
    king.id = "king_eldoria";
    king.name = "Nhà Vua Eldoria";
    king.spriteType = "king";
    king.x = 5.5f * TILE_SIZE;
    king.y = 3.0f * TILE_SIZE;
    king.triggerNodeId = "Node01";
    king.promptText = "Nói chuyện với Bệ Hạ";
    king.isTriggerActive = true;
    m_entities.push_back(king);

    // NPC 2: Thương Nhân Trạm Tiền Tiêu (Shop)
    WorldEntity merchant;
    merchant.id = "outpost_merchant";
    merchant.name = "Thương Nhân Tiền Tiêu";
    merchant.spriteType = "merchant";
    merchant.x = 5.5f * TILE_SIZE;
    merchant.y = 12.0f * TILE_SIZE;
    merchant.triggerNodeId = "Node02_Shop";
    merchant.promptText = "Vào Cửa Hàng Mua Sắm";
    merchant.isTriggerActive = true;
    m_entities.push_back(merchant);

    // NPC 3: Pháp Sư Học Giả tại Tàn Tích
    WorldEntity mageScholar;
    mageScholar.id = "ruins_scholar";
    mageScholar.name = "Pháp Sư Tàn Tích";
    mageScholar.spriteType = "mage";
    mageScholar.x = 24.0f * TILE_SIZE;
    mageScholar.y = 3.5f * TILE_SIZE;
    mageScholar.triggerNodeId = "Node04";
    mageScholar.promptText = "Giải mã Cổ Thư Pháp Viện";
    mageScholar.isTriggerActive = true;
    m_entities.push_back(mageScholar);

    // NPC 4: Rồng Cổ Đại (Boss Lair Portal)
    WorldEntity dragonBoss;
    dragonBoss.id = "ancient_dragon_boss";
    dragonBoss.name = "Ancient Dragon (BOSS)";
    dragonBoss.spriteType = "dragon";
    dragonBoss.x = 24.0f * TILE_SIZE;
    dragonBoss.y = 14.0f * TILE_SIZE;
    dragonBoss.triggerNodeId = "BATTLE:ancient_dragon";
    dragonBoss.promptText = "⚔️ KHIÊU CHIẾN RỒNG CỔ ĐẠI (BOSS)";
    dragonBoss.isTriggerActive = true;
    m_entities.push_back(dragonBoss);
}

bool OverworldScene2D::isSolidTile(int tileX, int tileY) const {
    if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT) {
        return true;
    }
    TileType t = m_map[tileY][tileX];
    return (t == TileType::WATER || t == TileType::TREE || t == TileType::CASTLE_WALL || t == TileType::LAVA);
}

void OverworldScene2D::update(float dt) {
    m_ambientTimer += dt;
    m_isMoving = false;

    float moveX = 0.0f;
    float moveY = 0.0f;

    // Phím di chuyển WASD / Mũi tên
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { moveX -= 1.0f; m_facingDirection = 2; m_isMoving = true; }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { moveX += 1.0f; m_facingDirection = 3; m_isMoving = true; }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) { moveY -= 1.0f; m_facingDirection = 1; m_isMoving = true; }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) { moveY += 1.0f; m_facingDirection = 0; m_isMoving = true; }

    // Chuẩn hóa vector di chuyển chéo
    if (moveX != 0.0f && moveY != 0.0f) {
        moveX *= 0.7071f;
        moveY *= 0.7071f;
    }

    // Xử lý hoạt ảnh bước chân
    if (m_isMoving) {
        m_walkAnimTimer += dt;
        if (m_walkAnimTimer >= 0.15f) {
            m_walkAnimTimer = 0.0f;
            m_walkFrame = (m_walkFrame + 1) % 4;
        }
    } else {
        m_walkFrame = 0;
    }

    // Kiểm tra va chạm hộp (Box Collision với Tilemap)
    float newX = m_playerX + moveX * m_speed * dt;
    float newY = m_playerY + moveY * m_speed * dt;

    int pLeft = (int)((newX + 4) / TILE_SIZE);
    int pRight = (int)((newX + 12) / TILE_SIZE);
    int pTop = (int)((m_playerY + 8) / TILE_SIZE);
    int pBottom = (int)((m_playerY + 15) / TILE_SIZE);

    // Di chuyển trục X
    if (!isSolidTile(pLeft, pTop) && !isSolidTile(pLeft, pBottom) &&
        !isSolidTile(pRight, pTop) && !isSolidTile(pRight, pBottom)) {
        m_playerX = newX;
    }

    // Di chuyển trục Y
    pLeft = (int)((m_playerX + 4) / TILE_SIZE);
    pRight = (int)((m_playerX + 12) / TILE_SIZE);
    pTop = (int)((newY + 8) / TILE_SIZE);
    pBottom = (int)((newY + 15) / TILE_SIZE);

    if (!isSolidTile(pLeft, pTop) && !isSolidTile(pLeft, pBottom) &&
        !isSolidTile(pRight, pTop) && !isSolidTile(pRight, pBottom)) {
        m_playerY = newY;
    }

    // Giới hạn trong bản đồ
    m_playerX = std::max((float)TILE_SIZE, std::min(m_playerX, (float)(MAP_WIDTH - 2) * TILE_SIZE));
    m_playerY = std::max((float)TILE_SIZE, std::min(m_playerY, (float)(MAP_HEIGHT - 2) * TILE_SIZE));

    // Kiểm tra khoảng cách gần NPC để tương tác
    m_activeEntityIndex = -1;
    float closestDist = 28.0f;

    for (size_t i = 0; i < m_entities.size(); ++i) {
        float dist = std::hypot((m_playerX + 8) - (m_entities[i].x + 8), (m_playerY + 8) - (m_entities[i].y + 8));
        if (dist < closestDist) {
            closestDist = dist;
            m_activeEntityIndex = (int)i;
        }
    }

    // Phím [I] hoặc [B]: Mở túi đồ & bảng chỉ số
    if (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_B)) {
        m_inventoryTriggered = true;
    }

    // Kích hoạt tương tác khi bấm phím [E], [SPACE] hoặc [ENTER]
    if (m_activeEntityIndex != -1) {
        if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            const auto& entity = m_entities[m_activeEntityIndex];
            if (entity.triggerNodeId == "Node02_Shop") {
                m_shopTriggered = true;
            } else if (entity.triggerNodeId.find("BATTLE:") == 0) {
                m_battleTriggered = true;
                m_triggeredEnemyId = entity.triggerNodeId.substr(7);
            } else {
                m_storyTriggered = true;
                m_triggeredNodeId = entity.triggerNodeId;
            }
        }
    }
}

void OverworldScene2D::render() {
    // 1. Vẽ toàn bộ bề mặt gạch Tilemap thế giới
    drawTilemap();

    // 2. Vẽ các NPC / Điểm tương tác
    drawEntities();

    // 3. Vẽ nhân vật người chơi (Hero)
    drawPlayer();

    // 4. Vẽ bảng hiển thị thông báo tương tác (Interaction Prompt)
    if (m_activeEntityIndex != -1) {
        drawInteractionPrompt();
    }

    // 5. Vẽ HUD thông tin vị trí và chỉ số
    drawWorldHUD();
}

void OverworldScene2D::drawTilemap() {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            int px = x * TILE_SIZE;
            int py = y * TILE_SIZE;
            TileType t = m_map[y][x];

            switch (t) {
                case TileType::GRASS:
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, ((x + y) % 2 == 0) ? Color{ 42, 105, 48, 255 } : Color{ 48, 118, 54, 255 });
                    break;
                case TileType::DIRT_ROAD:
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, Color{ 135, 95, 55, 255 });
                    DrawRectangle(px + 3, py + 3, 2, 2, Color{ 110, 75, 40, 255 });
                    break;
                case TileType::STONE_ROAD:
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, Color{ 120, 125, 140, 255 });
                    DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, Color{ 90, 95, 110, 255 });
                    break;
                case TileType::CASTLE_FLOOR:
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, Color{ 145, 30, 45, 255 }); // Sàn nhung đỏ
                    DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, Color{ 180, 140, 50, 255 });
                    break;
                case TileType::WATER: {
                    float wave = std::sin(m_ambientTimer * 3.0f + x) * 1.5f;
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, Color{ 30, 90, 190, 255 });
                    DrawRectangle(px + 2, py + 4 + (int)wave, 12, 2, Color{ 80, 160, 255, 180 });
                    break;
                }
                case TileType::TREE:
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, Color{ 35, 80, 40, 255 });
                    DrawRectangle(px + 2, py + 1, 12, 10, Color{ 25, 125, 45, 255 }); // Tán lá
                    DrawRectangle(px + 6, py + 10, 4, 6, Color{ 90, 55, 25, 255 });   // Thân cây
                    break;
                case TileType::CASTLE_WALL:
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, Color{ 70, 75, 90, 255 });
                    DrawRectangle(px + 2, py + 2, 5, 4, Color{ 95, 100, 120, 255 });
                    DrawRectangle(px + 9, py + 8, 5, 4, Color{ 95, 100, 120, 255 });
                    DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, Color{ 40, 45, 60, 255 });
                    break;
                case TileType::LAVA: {
                    float lavaGlow = std::sin(m_ambientTimer * 4.0f + x + y) * 20.0f;
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, Color{ (unsigned char)(210 + lavaGlow), 45, 15, 255 });
                    DrawCircle(px + 8, py + 8, 3, Color{ 255, 180, 40, 200 });
                    break;
                }
                case TileType::PORTAL: {
                    float pulse = std::sin(m_ambientTimer * 5.0f) * 3.0f;
                    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, Color{ 20, 10, 30, 255 });
                    DrawCircle(px + 8, py + 8, 6.0f + pulse, Color{ 180, 40, 220, 220 });
                    DrawCircle(px + 8, py + 8, 3.0f, Color{ 255, 200, 255, 255 });
                    break;
                }
            }
        }
    }
}

void OverworldScene2D::drawEntities() {
    AssetManager& assets = AssetManager::getInstance();

    for (const auto& entity : m_entities) {
        Texture2D tex = assets.getEnemyTexture(entity.spriteType);
        float scale = (entity.spriteType == "dragon") ? 1.0f : 0.85f;
        assets.drawEntitySprite(tex, entity.x - 2, entity.y - 4, scale);

        // Tên NPC nổi trên đầu
        float nameWidth = assets.measureText(entity.name, 8);
        DrawRectangle((int)(entity.x + 8 - nameWidth * 0.5f - 3), (int)(entity.y - 12), (int)(nameWidth + 6), 11, Color{ 0, 0, 0, 180 });
        assets.drawText(entity.name, entity.x + 8 - nameWidth * 0.5f, entity.y - 12, 8, GOLD);
    }
}

void OverworldScene2D::drawPlayer() {
    AssetManager& assets = AssetManager::getInstance();
    Texture2D heroTex = assets.getHeroTexture(m_hero.getHeroClassName());

    // Hoạt ảnh nhấp nhô khi bước đi
    float bob = (m_isMoving && (m_walkFrame % 2 == 1)) ? 1.0f : 0.0f;
    bool flipX = (m_facingDirection == 2); // Quay trái lật sprite

    assets.drawEntitySprite(heroTex, m_playerX - 4, m_playerY - 6 - bob, 0.9f, flipX);

    // Tên người chơi
    std::string pName = m_hero.getName();
    float pNameWidth = assets.measureText(pName, 8);
    DrawRectangle((int)(m_playerX + 8 - pNameWidth * 0.5f - 2), (int)(m_playerY - 14), (int)(pNameWidth + 4), 10, Color{ 0, 0, 0, 180 });
    assets.drawText(pName, m_playerX + 8 - pNameWidth * 0.5f, m_playerY - 14, 8, SKYBLUE);
}

void OverworldScene2D::drawInteractionPrompt() {
    AssetManager& assets = AssetManager::getInstance();
    const auto& entity = m_entities[m_activeEntityIndex];

    float pulse = std::sin(m_ambientTimer * 6.0f) * 2.0f;
    std::string prompt = "👉 [E / SPACE]: " + entity.promptText;
    float promptWidth = assets.measureText(prompt, 10);

    Rectangle promptBox = {
        240.0f - promptWidth * 0.5f - 10.0f,
        215.0f + pulse,
        promptWidth + 20.0f,
        24.0f
    };

    assets.drawNineSliceBox(promptBox, Color{ 16, 22, 42, 245 }, GOLD);
    assets.drawText(prompt, promptBox.x + 10, promptBox.y + 6, 10, YELLOW);
}

void OverworldScene2D::drawWorldHUD() {
    AssetManager& assets = AssetManager::getInstance();

    // Bảng HUD Góc Trái Trên
    assets.drawNineSliceBox(Rectangle{ 8, 8, 175, 42 }, Color{ 14, 18, 34, 230 }, GOLD);
    assets.drawText(TextFormat("🏰 %s (Lv.%d)", m_hero.getName().c_str(), m_hero.getLevel()), 16, 12, 9, YELLOW);
    assets.drawStatBar(16, 24, 155, 7, m_hero.getHp(), m_hero.getMaxHp(), Color{ 220, 40, 50, 255 }, Color{ 30, 30, 30, 255 });
    assets.drawStatBar(16, 34, 155, 6, m_hero.getMp(), m_hero.getMaxMp(), Color{ 40, 130, 240, 255 }, Color{ 30, 30, 30, 255 });

    // Hiển thị Vàng ở Góc Phải Trên
    assets.drawNineSliceBox(Rectangle{ 370, 8, 102, 22 }, Color{ 14, 18, 34, 230 }, GOLD);
    assets.drawItemIcon("gold", 376, 11, 16);
    assets.drawText(TextFormat("%d Vàng", m_hero.getGold()), 396, 13, 9, GOLD);

    // Hướng dẫn ở đáy màn hình
    DrawRectangle(0, 274, 480, 14, Color{ 0, 0, 0, 200 });
    assets.drawText("[WASD]: Di chuyển | [E/Space]: Tương tác | [I/B]: Túi đồ | [ESC]: Tạm dừng", 30, 276, 8, LIGHTGRAY);
}
