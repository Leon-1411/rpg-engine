#ifndef BATTLE_SCENE_2D_H
#define BATTLE_SCENE_2D_H

#include "Hero.h"
#include "Enemy.h"
#include "CombatEngine.h"
#include "ui_2d/AssetManager.h"
#include "raylib.h"
#include <string>
#include <vector>

struct FloatingText {
    std::string text;
    Vector2 pos;
    Color color;
    float timer;
    float maxTimer;
    float offsetY;
};

enum class BattleMenuState {
    MAIN_MENU,
    SKILL_MENU,
    ITEM_MENU,
    ANIMATING,
    VICTORY_SCREEN,
    DEFEAT_SCREEN,
    FLED_SCREEN
};

/**
 * @class BattleScene2D
 * @brief Quản lý toàn bộ màn hình chiến đấu theo lượt 2D Pixel Art cho RPG Engine.
 * Hỗ trợ hoạt ảnh tung chiêu, số sát thương nảy (Floating Damage), rung màn hình và tương tác phím/chuột.
 */
class BattleScene2D {
public:
    BattleScene2D(Hero& hero, Enemy& enemy, Inventory* inventory = nullptr);
    ~BattleScene2D() = default;

    // Cập nhật hoạt ảnh, thanh máu nội suy và xử lý lượt đánh mỗi khung hình
    void update(float dt, Vector2 virtualMousePos);

    // Vẽ toàn bộ chiến trường 2D Pixel
    void render();

    // Kiểm tra xem trận chiến đã kết thúc hoàn toàn chưa
    bool isFinished() const { return m_finished; }
    CombatState getFinalResult() const { return m_finalResult; }

    // Thêm hiệu ứng chữ số sát thương bay lên
    void addFloatingText(const std::string& text, Vector2 pos, Color color = YELLOW, float duration = 1.2f);

private:
    void drawArenaBackground();
    void drawFighters();
    void drawCombatHUD();
    void drawActionMenu();
    void drawSkillMenu();
    void drawItemMenu();
    void drawFloatingTexts();
    void drawOutcomeOverlay();

    void executeHeroAction(int action, int subIndex = -1);
    void triggerScreenShake(float intensity, float duration);

    Hero& m_hero;
    Enemy& m_enemy;
    Inventory* m_inventory;
    CombatEngine m_engine;

    BattleMenuState m_menuState;
    int m_selectedAction;
    int m_selectedSubIndex;

    // Hoạt ảnh vị trí nhân vật
    float m_heroOffsetAnimX;
    float m_enemyOffsetAnimX;
    float m_heroHurtTimer;
    float m_enemyHurtTimer;
    float m_animTimer;

    // Thanh máu nội suy mượt mà (Smooth Health Bar Interpolation)
    float m_displayHeroHp;
    float m_displayHeroMp;
    float m_displayEnemyHp;

    // Rung lắc màn hình (Screen Shake)
    float m_screenShakeTimer;
    float m_screenShakeIntensity;
    Vector2 m_shakeOffset;

    // Hiệu ứng hạt chiêu thức (Slash / Blast Particle)
    float m_attackEffectTimer;
    Vector2 m_attackEffectPos;
    Color m_attackEffectColor;

    // Danh sách chữ số nảy lên
    std::vector<FloatingText> m_floatingTexts;

    // Nhật ký chiến đấu gần nhất
    std::string m_lastCombatLog;

    // Chuỗi xử lý lượt theo thời gian (Turn Sequencer)
    float m_turnSequenceTimer;
    int m_turnSequencePhase; // 0: Idle, 1: Hero Attacking, 2: Enemy Turn, 3: Outcome Check
    int m_pendingAction;
    int m_pendingSubIndex;

    bool m_finished;
    CombatState m_finalResult;

    std::vector<Rectangle> m_buttonRects;
};

#endif // BATTLE_SCENE_2D_H
