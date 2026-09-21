@echo off
set GXX=g++

where g++ >nul 2>nul
if %ERRORLEVEL% equ 0 goto found_compiler

if exist "C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe" (
    set GXX="C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe"
    goto found_compiler
)

if exist "C:\Program Files\Git\mingw64\bin\g++.exe" (
    set GXX="C:\Program Files\Git\mingw64\bin\g++.exe"
    goto found_compiler
)

where docker >nul 2>nul
if %ERRORLEVEL% equ 0 goto use_docker

goto found_compiler

:use_docker
echo [RPG Engine] Compiler not found in Windows PATH, using Docker environment...
set DOCKER_CORE=src/Hero.cpp src/Warrior.cpp src/Mage.cpp src/Ranger.cpp src/LevelSystem.cpp src/Enemy.cpp src/Minion.cpp src/BossMonster.cpp src/Item.cpp src/Inventory.cpp src/Shop.cpp src/CombatEngine.cpp src/StoryGraph.cpp src/SaveManager.cpp src/DataLoader.cpp src/GameManager.cpp src/ui/ConsoleUI.cpp src/ui/ASCIIArt.cpp src/ui/MainMenu.cpp src/ui/BattleUI.cpp src/ui/InventoryUI.cpp
if "%1"=="test" (
    docker run --rm -v "%cd%":/app -w /app rpg-engine:dev bash -c "g++ -std=c++17 -Iinclude -Iinclude/ui -Iinclude/nlohmann %DOCKER_CORE% tests/test_item.cpp -o test_item && ./test_item && g++ -std=c++17 -Iinclude -Iinclude/ui -Iinclude/nlohmann %DOCKER_CORE% tests/test_inventory.cpp -o test_inventory && ./test_inventory && g++ -std=c++17 -Iinclude -Iinclude/ui -Iinclude/nlohmann %DOCKER_CORE% tests/test_combat.cpp -o test_combat && ./test_combat && g++ -std=c++17 -Iinclude -Iinclude/ui -Iinclude/nlohmann %DOCKER_CORE% tests/test_save.cpp -o test_save && ./test_save && g++ -std=c++17 -Iinclude -Iinclude/ui -Iinclude/nlohmann %DOCKER_CORE% tests/test_story.cpp -o test_story && ./test_story && g++ -std=c++17 -Iinclude -Iinclude/ui -Iinclude/nlohmann %DOCKER_CORE% tests/test_shop.cpp -o test_shop && ./test_shop"
    goto end
)
if "%1"=="clean" (
    del /q *.exe *.o test_save test_story test_shop test_item test_inventory test_combat 2>nul
    echo [RPG Engine] Cleaned build artifacts.
    goto end
)
docker run --rm -v "%cd%":/app -w /app rpg-engine:dev bash -c "g++ -std=c++17 -Iinclude -Iinclude/ui -Iinclude/nlohmann %DOCKER_CORE% src/main.cpp -o rpg_engine"
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine] Build SUCCESS! Output: rpg_engine
) else (
    echo [RPG Engine] Build FAILED!
)
goto end

:found_compiler
set INCLUDES=-Iinclude -Iinclude/ui -Iinclude/ui_2d -Iinclude/raylib -Iinclude/nlohmann
set RAYLIB_LIBS=-Llib -lraylib -lopengl32 -lgdi32 -lwinmm
set CXXFLAGS=-std=c++14 %INCLUDES%
set CORE_SOURCES=src/Hero.cpp src/Warrior.cpp src/Mage.cpp src/Ranger.cpp src/LevelSystem.cpp src/Enemy.cpp src/Minion.cpp src/BossMonster.cpp src/Item.cpp src/Inventory.cpp src/Shop.cpp src/CombatEngine.cpp src/StoryGraph.cpp src/SaveManager.cpp src/DataLoader.cpp src/GameManager.cpp src/ui/ConsoleUI.cpp src/ui/ASCIIArt.cpp src/ui/MainMenu.cpp src/ui/BattleUI.cpp src/ui/InventoryUI.cpp
set SOURCES_2D=src/ui_2d/PixelCanvas.cpp src/ui_2d/AssetManager.cpp src/ui_2d/StoryScene2D.cpp src/ui_2d/BattleScene2D.cpp src/ui_2d/OverworldScene2D.cpp src/ui_2d/InventoryShopScene2D.cpp src/ui_2d/TitleScene2D.cpp

if "%1"=="clean" goto clean
if "%1"=="test" goto test
if "%1"=="run" goto run
if "%1"=="2d" goto run2d
if "%1"=="build2d" goto build2d
if "%1"=="demo2d" goto demo2d
if "%1"=="demoassets" goto demoassets
if "%1"=="playstory2d" goto playstory2d
if "%1"=="playbattle2d" goto playbattle2d
if "%1"=="playoverworld2d" goto playoverworld2d

echo [RPG Engine] Building rpg_engine.exe...
%GXX% %CXXFLAGS% %CORE_SOURCES% src/main.cpp -o rpg_engine.exe
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine] Build SUCCESS! Output: rpg_engine.exe
) else (
    echo [RPG Engine] Build FAILED!
)
goto end

:build2d
echo [RPG Engine 2D] Building rpg_engine_2d.exe...
%GXX% -std=c++17 %INCLUDES% src/main_2d.cpp %CORE_SOURCES% %SOURCES_2D% -o rpg_engine_2d.exe %RAYLIB_LIBS%
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine 2D] Build SUCCESS! Output: rpg_engine_2d.exe
) else (
    echo [RPG Engine 2D] Build FAILED!
)
goto end

:run2d
if not exist rpg_engine_2d.exe (
    call %0 build2d
)
echo [RPG Engine 2D] Launching Eldoria 2D Pixel Edition...
.\rpg_engine_2d.exe
goto end

:demo2d
echo [RPG Engine 2D] Building demo_2d.exe...
%GXX% %CXXFLAGS% %SOURCES_2D% tools/demo_2d.cpp -o demo_2d.exe %RAYLIB_LIBS%
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine 2D] Build SUCCESS! Output: demo_2d.exe
    echo [RPG Engine 2D] Running demo_2d.exe...
    .\demo_2d.exe
) else (
    echo [RPG Engine 2D] Build FAILED!
)
goto end

:demoassets
echo [RPG Engine 2D] Building demo_assets.exe...
%GXX% %CXXFLAGS% %SOURCES_2D% tools/demo_assets.cpp -o demo_assets.exe %RAYLIB_LIBS%
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine 2D] Build SUCCESS! Output: demo_assets.exe
    echo [RPG Engine 2D] Running demo_assets.exe...
    .\demo_assets.exe
) else (
    echo [RPG Engine 2D] Build FAILED!
)
goto end

:playstory2d
echo [RPG Engine 2D] Building play_story_2d.exe...
%GXX% %CXXFLAGS% %CORE_SOURCES% %SOURCES_2D% tools/play_story_2d.cpp -o play_story_2d.exe %RAYLIB_LIBS%
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine 2D] Build SUCCESS! Output: play_story_2d.exe
    echo [RPG Engine 2D] Running play_story_2d.exe...
    .\play_story_2d.exe
) else (
    echo [RPG Engine 2D] Build FAILED!
)
goto end

:playbattle2d
echo [RPG Engine 2D] Building play_battle_2d.exe...
%GXX% %CXXFLAGS% %CORE_SOURCES% %SOURCES_2D% tools/play_battle_2d.cpp -o play_battle_2d.exe %RAYLIB_LIBS%
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine 2D] Build SUCCESS! Output: play_battle_2d.exe
    echo [RPG Engine 2D] Running play_battle_2d.exe...
    .\play_battle_2d.exe
) else (
    echo [RPG Engine 2D] Build FAILED!
)
goto end

:playoverworld2d
echo [RPG Engine 2D] Building play_overworld_2d.exe...
%GXX% %CXXFLAGS% %CORE_SOURCES% %SOURCES_2D% tools/play_overworld_2d.cpp -o play_overworld_2d.exe %RAYLIB_LIBS%
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine 2D] Build SUCCESS! Output: play_overworld_2d.exe
    echo [RPG Engine 2D] Running play_overworld_2d.exe...
    .\play_overworld_2d.exe
) else (
    echo [RPG Engine 2D] Build FAILED!
)
goto end

:test
echo [RPG Engine] Running Tests...
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_item.cpp -o test_item.exe
.\test_item.exe
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_inventory.cpp -o test_inventory.exe
.\test_inventory.exe
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_combat.cpp -o test_combat.exe
.\test_combat.exe
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_save.cpp -o test_save.exe
.\test_save.exe
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_story.cpp -o test_story.exe
.\test_story.exe
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_ui.cpp -o test_ui.exe
.\test_ui.exe
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_hero.cpp -o test_hero.exe
.\test_hero.exe
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_boss.cpp -o test_boss.exe
.\test_boss.exe
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_shop.cpp -o test_shop.exe
.\test_shop.exe
goto end

:run
if not exist rpg_engine.exe (
    call %0
)
.\rpg_engine.exe
goto end

:clean
del /q *.exe *.o 2>nul
echo [RPG Engine] Cleaned build artifacts.
goto end

:end

