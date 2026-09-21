#include "ui_2d/StoryScene2D.h"
#include <algorithm>
#include <cmath>

StoryScene2D::StoryScene2D(StoryGraph& storyGraph)
    : m_storyGraph(storyGraph)
    , m_heroClass("Warrior")
    , m_heroGold(100)
    , m_textTimer(0.0f)
    , m_visibleCharCount(0)
    , m_textComplete(false)
    , m_selectedChoiceIndex(0)
    , m_toastTimer(0.0f)
{
    refreshNode();
}

void StoryScene2D::refreshNode() {
    m_textTimer = 0.0f;
    m_visibleCharCount = 0;
    m_textComplete = false;
    m_selectedChoiceIndex = 0;
    m_choiceRects.clear();

    StoryNode node = m_storyGraph.getCurrentNode();
    m_cachedNodeId = node.id;

    // Tự động kiểm tra và trao thưởng nếu là Node REWARD
    if (!node.rewardItems.empty() || node.rewardGold > 0 || node.rewardExp > 0) {
        std::string msg = "★ Nhận thưởng: ";
        if (node.rewardGold > 0) msg += "+" + std::to_string(node.rewardGold) + " Vàng  ";
        if (node.rewardExp > 0) msg += "+" + std::to_string(node.rewardExp) + " EXP  ";
        for (const auto& item : node.rewardItems) msg += "[" + item + "] ";
        m_toastMessage = msg;
        m_toastTimer = 4.0f;
    }

    // Xử lý tự động chuyển tiếp nếu là REQUIREMENT_CHECK node
    if (node.rawType == "REQUIREMENT_CHECK") {
        bool pass = true;
        if (!node.requiredHeroClass.empty() && node.requiredHeroClass != m_heroClass) pass = false;
        if (!node.requiredItem.empty() && !m_storyGraph.getFlag("item_" + node.requiredItem)) pass = false;

        std::string nextId = pass ? node.onPassNodeId : node.onFailNodeId;
        if (!nextId.empty()) {
            m_storyGraph.moveToNode(nextId);
            refreshNode();
        }
    }
}

bool StoryScene2D::isPendingBattle() const {
    StoryNode node = m_storyGraph.getCurrentNode();
    return (node.type == EventType::BATTLE || node.rawType == "BATTLE" || node.rawType == "BOSS") && !node.enemyId.empty();
}

std::string StoryScene2D::getPendingEnemyId() const {
    return m_storyGraph.getCurrentNode().enemyId;
}

bool StoryScene2D::isPendingShop() const {
    StoryNode node = m_storyGraph.getCurrentNode();
    return node.type == EventType::SHOP || node.rawType == "SHOP" || node.id.find("Shop") != std::string::npos;
}

void StoryScene2D::update(float dt, Vector2 vMouse) {
    StoryNode node = m_storyGraph.getCurrentNode();

    // Kiểm tra nếu node hoặc hội thoại đã thay đổi
    if (node.id != m_cachedNodeId) {
        refreshNode();
        return;
    }

    // Cập nhật Toast notification
    if (m_toastTimer > 0.0f) {
        m_toastTimer -= dt;
    }

    // Xác định nội dung chữ cần hiển thị (Hội thoại rẽ nhánh hoặc Mô tả cốt truyện)
    std::string currentText = node.text;
    std::string currentSpeaker = node.npcName.empty() ? "Dẫn Chuyện" : node.npcName;
    std::vector<std::string> choiceTexts;
    std::vector<bool> choiceAvailabilities;

    bool inDialogue = m_storyGraph.isInDialogue();
    if (inDialogue) {
        DialogueNode dNode = m_storyGraph.getCurrentDialogueNode();
        currentSpeaker = dNode.speaker;
        currentText = dNode.text;

        for (size_t i = 0; i < dNode.choices.size(); ++i) {
            choiceTexts.push_back(dNode.choices[i].text);
            choiceAvailabilities.push_back(m_storyGraph.canSelectDialogueChoice((int)i));
        }
    } else {
        for (size_t i = 0; i < node.choices.size(); ++i) {
            choiceTexts.push_back(node.choices[i].text);
            choiceAvailabilities.push_back(m_storyGraph.canSelectChoice((int)i));
        }
    }

    // Hoạt ảnh Typewriter: 35 ký tự mỗi giây
    if (!m_textComplete) {
        m_textTimer += dt;
        m_visibleCharCount = (int)(m_textTimer * 35.0f);
        if (m_visibleCharCount >= (int)currentText.length()) {
            m_visibleCharCount = (int)currentText.length();
            m_textComplete = true;
        }

        // Bấm phím Space / Enter hoặc click chuột để tua nhanh toàn bộ chữ
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            m_visibleCharCount = (int)currentText.length();
            m_textComplete = true;
            return;
        }
    }

    // Xử lý lựa chọn khi chữ đã hiển thị xong
    if (m_textComplete && !choiceTexts.empty()) {
        int choiceCount = (int)choiceTexts.size();

        // Điều khiển phím Mũi tên / W / S
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            m_selectedChoiceIndex = (m_selectedChoiceIndex - 1 + choiceCount) % choiceCount;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            m_selectedChoiceIndex = (m_selectedChoiceIndex + 1) % choiceCount;
        }

        // Phím số [1-9]
        for (int k = 0; k < choiceCount && k < 9; ++k) {
            if (IsKeyPressed(KEY_ONE + k) || IsKeyPressed(KEY_KP_1 + k)) {
                m_selectedChoiceIndex = k;
                if (choiceAvailabilities[k]) {
                    if (inDialogue) m_storyGraph.selectDialogueChoice(k);
                    else m_storyGraph.selectChoice(k);
                    refreshNode();
                    return;
                }
            }
        }

        // Tương tác chuột (Hover & Click)
        for (size_t i = 0; i < m_choiceRects.size() && i < choiceTexts.size(); ++i) {
            if (CheckCollisionPointRec(vMouse, m_choiceRects[i])) {
                m_selectedChoiceIndex = (int)i;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && choiceAvailabilities[i]) {
                    if (inDialogue) m_storyGraph.selectDialogueChoice((int)i);
                    else m_storyGraph.selectChoice((int)i);
                    refreshNode();
                    return;
                }
            }
        }

        // Bấm Enter / Space để chọn
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (m_selectedChoiceIndex >= 0 && m_selectedChoiceIndex < choiceCount) {
                if (choiceAvailabilities[m_selectedChoiceIndex]) {
                    if (inDialogue) m_storyGraph.selectDialogueChoice(m_selectedChoiceIndex);
                    else m_storyGraph.selectChoice(m_selectedChoiceIndex);
                    refreshNode();
                    return;
                }
            }
        }
    }
}

void StoryScene2D::render() {
    StoryNode node = m_storyGraph.getCurrentNode();

    // 1. Vẽ bối cảnh thế giới 2D Pixel theo từng địa danh
    drawBackground(node);

    // 2. Vẽ chân dung và Sprite NPC / Speaker
    std::string speaker = node.npcName.empty() ? "Dẫn Chuyện" : node.npcName;
    if (m_storyGraph.isInDialogue()) {
        speaker = m_storyGraph.getCurrentDialogueNode().speaker;
    }
    drawNPCPortrait(speaker, node.rawType);

    // 3. Chuẩn bị nội dung hiển thị
    std::string fullText = node.text;
    std::vector<std::string> choiceTexts;
    std::vector<bool> choiceAvailabilities;

    if (m_storyGraph.isInDialogue()) {
        DialogueNode dNode = m_storyGraph.getCurrentDialogueNode();
        fullText = dNode.text;
        for (size_t i = 0; i < dNode.choices.size(); ++i) {
            choiceTexts.push_back(dNode.choices[i].text);
            choiceAvailabilities.push_back(m_storyGraph.canSelectDialogueChoice((int)i));
        }
    } else {
        for (size_t i = 0; i < node.choices.size(); ++i) {
            choiceTexts.push_back(node.choices[i].text);
            choiceAvailabilities.push_back(m_storyGraph.canSelectChoice((int)i));
        }
    }

    // 4. Vẽ Hộp Thoại & Typewriter Text
    drawDialogueBox(speaker, fullText);

    // 5. Vẽ danh sách các lựa chọn rẽ nhánh nếu chữ đã hiện xong
    if (m_textComplete && !choiceTexts.empty()) {
        drawChoices(choiceTexts, choiceAvailabilities);
    }

    // 6. Vẽ Toast Notification nếu có
    drawToastNotification();
}

void StoryScene2D::drawBackground(const StoryNode& node) {
    std::string id = node.id;
    std::string title = node.title;

    // Phân loại môi trường bối cảnh
    if (id == "Node01" || title.find("Hoàng Thành") != std::string::npos || title.find("Vua") != std::string::npos) {
        // Đại Điện Hoàng Gia Eldoria
        ClearBackground(Color{ 45, 20, 30, 255 }); // Nền đỏ nhung
        DrawRectangle(0, 0, 480, 140, Color{ 35, 15, 25, 255 });
        // Cột đá vàng hoàng gia
        DrawRectangle(40, 0, 24, 140, Color{ 180, 140, 50, 255 });
        DrawRectangle(440, 0, 24, 140, Color{ 180, 140, 50, 255 });
        DrawRectangle(140, 0, 16, 140, Color{ 140, 110, 40, 255 });
        DrawRectangle(340, 0, 16, 140, Color{ 140, 110, 40, 255 });
        // Thảm đỏ hoàng cung
        DrawRectangle(180, 40, 120, 100, Color{ 160, 20, 35, 255 });
        DrawRectangle(190, 40, 100, 100, Color{ 190, 30, 45, 255 });
        // Ngai vàng xa xa
        DrawRectangle(220, 20, 40, 40, Color{ 230, 180, 30, 255 });
        DrawRectangle(230, 10, 20, 15, Color{ 255, 215, 0, 255 });
    }
    else if (title.find("Rừng") != std::string::npos || title.find("Woods") != std::string::npos) {
        // Rừng Whispering Woods
        ClearBackground(Color{ 18, 35, 25, 255 });
        DrawRectangle(0, 0, 480, 140, Color{ 12, 25, 18, 255 });
        for (int i = 0; i < 8; ++i) {
            DrawRectangle(30 + i * 60, 20, 30, 90, Color{ 25, 75, 40, 255 });
            DrawRectangle(40 + i * 60, 100, 10, 40, Color{ 80, 50, 25, 255 });
        }
    }
    else if (title.find("Pháp Viện") != std::string::npos || title.find("Tàn Tích") != std::string::npos) {
        // Tàn Tích Pháp Viện Cổ
        ClearBackground(Color{ 22, 18, 42, 255 });
        DrawRectangle(0, 0, 480, 140, Color{ 15, 12, 32, 255 });
        // Ký tự cổ ngữ phát sáng
        DrawCircle(240, 60, 35, Color{ 90, 40, 160, 150 });
        DrawCircleLines(240, 60, 40, Color{ 0, 210, 255, 200 });
        DrawRectangle(80, 30, 20, 110, Color{ 70, 65, 95, 255 });
        DrawRectangle(380, 30, 20, 110, Color{ 70, 65, 95, 255 });
    }
    else if (title.find("Rồng") != std::string::npos || title.find("Thung Lũng") != std::string::npos || node.rawType == "BOSS") {
        // Thung Lũng Rồng Quỷ
        ClearBackground(Color{ 45, 12, 12, 255 });
        DrawRectangle(0, 0, 480, 140, Color{ 30, 8, 8, 255 });
        // Nham thạch phát sáng
        DrawRectangle(0, 110, 480, 30, Color{ 220, 60, 20, 200 });
        DrawRectangle(0, 125, 480, 15, Color{ 255, 160, 40, 255 });
    }
    else {
        // Mặc định: Pháo Đài / Hầm Ngục Huyền Bí
        ClearBackground(Color{ 18, 22, 36, 255 });
        DrawRectangle(0, 0, 480, 140, Color{ 12, 15, 26, 255 });
        for (int x = 0; x < 480; x += 32) {
            DrawLine(x, 0, x, 140, Color{ 28, 35, 55, 255 });
        }
    }

    // Header Vị trí & Node ID
    DrawRectangle(10, 8, 460, 22, Color{ 0, 0, 0, 180 });
    DrawRectangleLines(10, 8, 460, 22, Color{ 140, 120, 70, 255 });
    AssetManager& assets = AssetManager::getInstance();
    assets.drawText(TextFormat("📍 %s", node.title.empty() ? node.id.c_str() : node.title.c_str()), 18, 14, 11, GOLD);
    assets.drawText(TextFormat("[%s | %s]", node.id.c_str(), node.rawType.c_str()), 340, 14, 9, LIGHTGRAY);
}

void StoryScene2D::drawNPCPortrait(const std::string& speaker, const std::string& rawType) {
    AssetManager& assets = AssetManager::getInstance();

    // Khung chân dung Hero góc trái
    assets.drawNineSliceBox(Rectangle{ 18, 38, 70, 78 }, Color{ 12, 16, 30, 230 }, SKYBLUE);
    Texture2D heroTex = assets.getHeroTexture(m_heroClass);
    assets.drawEntitySprite(heroTex, 31, 46, 2.0f);
    assets.drawText(m_heroClass, 24, 98, 9, SKYBLUE);

    // Khung chân dung Speaker / NPC / Boss góc phải
    std::string lowerSpeaker = speaker;
    std::transform(lowerSpeaker.begin(), lowerSpeaker.end(), lowerSpeaker.begin(), ::tolower);

    Texture2D npcTex;
    Color borderColor = GOLD;
    std::string npcLabel = speaker;

    if (lowerSpeaker.find("vua") != std::string::npos || lowerSpeaker.find("king") != std::string::npos) {
        npcTex = assets.getEnemyTexture("king");
        borderColor = GOLD;
    } else if (lowerSpeaker.find("rồng") != std::string::npos || lowerSpeaker.find("dragon") != std::string::npos) {
        npcTex = assets.getEnemyTexture("ancient_dragon");
        borderColor = RED;
    } else if (lowerSpeaker.find("ma vương") != std::string::npos || lowerSpeaker.find("shadow") != std::string::npos) {
        npcTex = assets.getEnemyTexture("shadow_lord");
        borderColor = PURPLE;
    } else if (lowerSpeaker.find("orc") != std::string::npos) {
        npcTex = assets.getEnemyTexture("armored_orc");
        borderColor = ORANGE;
    } else if (lowerSpeaker.find("elena") != std::string::npos || lowerSpeaker.find("công chúa") != std::string::npos) {
        npcTex = assets.getEnemyTexture("elena");
        borderColor = PINK;
    } else {
        npcTex = assets.getEnemyTexture("goblin");
        borderColor = LIGHTGRAY;
    }

    // Vẽ khung NPC lớn góc phải
    assets.drawNineSliceBox(Rectangle{ 392, 38, 70, 78 }, Color{ 12, 16, 30, 230 }, borderColor);
    assets.drawEntitySprite(npcTex, 405, 46, 2.0f, true);
    assets.drawText(npcLabel, 396, 98, 9, borderColor);
}

void StoryScene2D::drawDialogueBox(const std::string& speaker, const std::string& fullText) {
    AssetManager& assets = AssetManager::getInstance();

    // Khung Hộp thoại 9-Slice chiếm nửa dưới màn hình
    Rectangle boxRect = { 15, 125, 450, 135 };
    assets.drawNineSliceBox(boxRect, Color{ 14, 18, 34, 245 }, Color{ 200, 170, 95, 255 });

    // Huy hiệu Tên người nói (Speaker Name Tag)
    assets.drawNineSliceBox(Rectangle{ 25, 116, 140, 18 }, Color{ 25, 30, 55, 255 }, GOLD);
    assets.drawText(TextFormat("🗣️ %s", speaker.c_str()), 32, 120, 10, GOLD);

    // Cắt chuỗi theo hiệu ứng gõ chữ Typewriter
    std::string visibleText = fullText.substr(0, m_visibleCharCount);

    // Vẽ văn bản tự động xuống dòng chuẩn Pixel
    float posX = 30;
    float posY = 142;
    float lineSpacing = 14;
    float maxLineWidth = 415;

    std::string currentLine = "";
    std::string word = "";

    for (size_t i = 0; i < visibleText.length(); ++i) {
        char c = visibleText[i];
        if (c == '\n') {
            currentLine += word;
            assets.drawText(currentLine, posX, posY, 10, RAYWHITE);
            posY += lineSpacing;
            currentLine = "";
            word = "";
            continue;
        }

        if (c == ' ') {
            if (assets.measureText(currentLine + word, 10) > maxLineWidth) {
                assets.drawText(currentLine, posX, posY, 10, RAYWHITE);
                posY += lineSpacing;
                currentLine = word + " ";
            } else {
                currentLine += word + " ";
            }
            word = "";
        } else {
            word += c;
        }
    }
    if (!word.empty() || !currentLine.empty()) {
        assets.drawText(currentLine + word, posX, posY, 10, RAYWHITE);
    }

    // Biểu tượng nhấp nháy báo hoàn tất hoặc tua tiếp
    if (!m_textComplete) {
        assets.drawText("▶ [Tua: Space/Click]", 340, 242, 9, GRAY);
    }
}

void StoryScene2D::drawChoices(const std::vector<std::string>& choiceTexts, const std::vector<bool>& choiceAvailabilities) {
    AssetManager& assets = AssetManager::getInstance();
    m_choiceRects.clear();

    int startY = 188;
    int choiceHeight = 16;
    int gap = 3;

    for (size_t i = 0; i < choiceTexts.size(); ++i) {
        Rectangle rect = { 28, (float)(startY + i * (choiceHeight + gap)), 424, (float)choiceHeight };
        m_choiceRects.push_back(rect);

        bool isSelected = (m_selectedChoiceIndex == (int)i);
        bool isAvailable = choiceAvailabilities[i];

        Color bgColor = isSelected ? Color{ 55, 65, 110, 240 } : Color{ 20, 25, 45, 200 };
        Color borderColor = isSelected ? GOLD : (isAvailable ? Color{ 90, 110, 160, 255 } : Color{ 70, 70, 70, 255 });
        Color textColor = isAvailable ? (isSelected ? YELLOW : RAYWHITE) : Color{ 120, 120, 120, 255 };

        // Vẽ hộp lựa chọn
        DrawRectangleRec(rect, bgColor);
        DrawRectangleLinesEx(rect, 1.0f, borderColor);

        // Con trỏ chọn `▶`
        if (isSelected) {
            assets.drawText("▶", rect.x + 6, rect.y + 3, 9, GOLD);
        }

        // Nội dung lựa chọn
        std::string label = "[" + std::to_string(i + 1) + "] " + choiceTexts[i];
        if (!isAvailable) label += " (Chưa đủ điều kiện)";
        assets.drawText(label, rect.x + 20, rect.y + 3, 9, textColor);
    }
}

void StoryScene2D::drawToastNotification() {
    if (m_toastTimer <= 0.0f || m_toastMessage.empty()) return;

    AssetManager& assets = AssetManager::getInstance();
    float alpha = std::min(1.0f, m_toastTimer);

    Rectangle toastRect = { 90, 18, 300, 26 };
    assets.drawNineSliceBox(toastRect, Color{ 20, 45, 25, (unsigned char)(240 * alpha) }, Color{ 80, 220, 120, (unsigned char)(255 * alpha) });
    assets.drawText(m_toastMessage, 105, 24, 10, Color{ 240, 255, 220, (unsigned char)(255 * alpha) });
}
