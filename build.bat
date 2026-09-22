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
set INCLUDES=-Iinclude -Iinclude/ui -Iinclude/nlohmann
set CXXFLAGS=-std=c++14 %INCLUDES%
set CORE_SOURCES=src/Hero.cpp src/Warrior.cpp src/Mage.cpp src/Ranger.cpp src/LevelSystem.cpp src/Enemy.cpp src/Minion.cpp src/BossMonster.cpp src/Item.cpp src/Inventory.cpp src/Shop.cpp src/CombatEngine.cpp src/StoryGraph.cpp src/SaveManager.cpp src/DataLoader.cpp src/GameManager.cpp src/ui/ConsoleUI.cpp src/ui/ASCIIArt.cpp src/ui/MainMenu.cpp src/ui/BattleUI.cpp src/ui/InventoryUI.cpp

if "%1"=="clean" goto clean
if "%1"=="test" goto test
if "%1"=="run" goto run

echo [RPG Engine] Building rpg_engine.exe...
%GXX% %CXXFLAGS% %CORE_SOURCES% src/main.cpp -o rpg_engine.exe
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine] Build SUCCESS! Output: rpg_engine.exe
) else (
    echo [RPG Engine] Build FAILED!
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
%GXX% %CXXFLAGS% %CORE_SOURCES% tests/test_level_system.cpp -o test_level_system.exe
.\test_level_system.exe
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
