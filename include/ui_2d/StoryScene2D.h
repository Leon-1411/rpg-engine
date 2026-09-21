#ifndef STORY_SCENE_2D_H
#define STORY_SCENE_2D_H

#include "StoryGraph.h"
#include "ui_2d/AssetManager.h"
#include "raylib.h"
#include <string>
#include <vector>

/**
 * @class StoryScene2D
 * @brief Quản lý hiển thị và tương tác cốt truyện 2D Pixel Art cho StoryGraph.
 * Hỗ trợ bối cảnh 2D, chân dung NPC, chữ gõ Typewriter, menu rẽ nhánh đa phương thức.
 */
class StoryScene2D {
public:
    StoryScene2D(StoryGraph& storyGraph);
    ~StoryScene2D() = default;

    // Cập nhật trạng thái, animation chữ và phím bấm mỗi khung hình
    void update(float dt, Vector2 virtualMousePos);

    // Vẽ toàn bộ màn hình cốt truyện 2D
    void render();

    // Thiết lập Class của người chơi (Warrior, Mage, Ranger) để hiển thị chân dung & kiểm tra điều kiện
    void setHeroClass(const std::string& heroClass) { m_heroClass = heroClass; }
    void setHeroGold(int gold) { m_heroGold = gold; }

    // Kiểm tra xem cốt truyện có yêu cầu chuyển sang màn hình Chiến đấu (BATTLE) hay Cửa hàng (SHOP) không
    bool isPendingBattle() const;
    std::string getPendingEnemyId() const;
    bool isPendingShop() const;

    // Reset lại hoạt ảnh chữ gõ cho node mới
    void refreshNode();

private:
    void drawBackground(const StoryNode& node);
    void drawNPCPortrait(const std::string& speakerOrNpc, const std::string& rawType);
    void drawDialogueBox(const std::string& speaker, const std::string& fullText);
    void drawChoices(const std::vector<std::string>& choiceTexts, const std::vector<bool>& choiceAvailabilities);
    void drawToastNotification();

    StoryGraph& m_storyGraph;
    std::string m_heroClass;
    int m_heroGold;

    // Trạng thái gõ chữ Typewriter
    float m_textTimer;
    int m_visibleCharCount;
    bool m_textComplete;
    std::string m_cachedNodeId;
    std::string m_cachedDialogueId;

    // Điều hướng lựa chọn
    int m_selectedChoiceIndex;

    // Thông báo Toast khi nhận thưởng / cờ sự kiện
    std::string m_toastMessage;
    float m_toastTimer;

    // Tương tác chuột
    std::vector<Rectangle> m_choiceRects;
};

#endif // STORY_SCENE_2D_H
