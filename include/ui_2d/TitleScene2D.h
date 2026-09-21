#ifndef TITLE_SCENE_2D_H
#define TITLE_SCENE_2D_H

#include "Hero.h"
#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"
#include "SaveManager.h"
#include "ui_2d/AssetManager.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <memory>

enum class TitleMenuState {
    MAIN_MENU,
    CLASS_SELECT,
    LOAD_GAME,
    CREDITS
};

class TitleScene2D {
public:
    TitleScene2D(SaveManager& saveManager);
    ~TitleScene2D() = default;

    void update(float dt);
    void render();

    bool hasStartedNewGame() const { return m_startNewGame; }
    bool hasLoadedGame() const { return m_gameLoaded; }
    bool hasQuitRequested() const { return m_quitRequested; }
    int getLoadedSlot() const { return m_selectedLoadSlot; }

    std::unique_ptr<Hero> createSelectedHero();

    void reset() {
        m_state = TitleMenuState::MAIN_MENU;
        m_startNewGame = false;
        m_gameLoaded = false;
        m_quitRequested = false;
        m_menuIndex = 0;
        m_classIndex = 0;
    }

private:
    void updateMainMenu();
    void updateClassSelect();
    void updateLoadGame();
    void updateCredits();

    void renderMainMenu();
    void renderClassSelect();
    void renderLoadGame();
    void renderCredits();

    SaveManager& m_saveManager;
    TitleMenuState m_state;

    int m_menuIndex;
    int m_classIndex; // 0: Warrior, 1: Mage, 2: Ranger
    int m_loadSlotIndex;
    int m_selectedLoadSlot;

    bool m_startNewGame;
    bool m_gameLoaded;
    bool m_quitRequested;

    float m_animTimer;
    std::string m_heroName;
};

#endif // TITLE_SCENE_2D_H
