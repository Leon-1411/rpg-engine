@echo off
set GXX="C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe"
set INCLUDES=-Iinclude -Iinclude/ui -Iinclude/nlohmann
set CXXFLAGS=-std=c++14 %INCLUDES%

if "%1"=="clean" goto clean
if "%1"=="test" goto test
if "%1"=="run" goto run

echo [RPG Engine] Building rpg_engine.exe...
%GXX% %CXXFLAGS% src/Hero.cpp src/Warrior.cpp src/Mage.cpp src/Ranger.cpp src/LevelSystem.cpp src/Enemy.cpp src/Minion.cpp src/BossMonster.cpp src/Item.cpp src/Inventory.cpp src/CombatEngine.cpp src/StoryGraph.cpp src/SaveManager.cpp src/ui/ConsoleUI.cpp src/ui/ASCIIArt.cpp src/ui/MainMenu.cpp src/ui/BattleUI.cpp src/ui/InventoryUI.cpp src/main.cpp -o rpg_engine.exe
if %ERRORLEVEL% equ 0 (
    echo [RPG Engine] Build SUCCESS! Output: rpg_engine.exe
) else (
    echo [RPG Engine] Build FAILED!
)
goto end

:test
echo [RPG Engine] Running Tests...
%GXX% %CXXFLAGS% src/Hero.cpp src/Warrior.cpp src/Mage.cpp src/Ranger.cpp src/LevelSystem.cpp src/Enemy.cpp src/Minion.cpp src/BossMonster.cpp src/Item.cpp src/Inventory.cpp src/CombatEngine.cpp src/StoryGraph.cpp src/SaveManager.cpp src/ui/ConsoleUI.cpp src/ui/ASCIIArt.cpp tests/test_save.cpp -o test_save.exe
.\test_save.exe
%GXX% %CXXFLAGS% src/StoryGraph.cpp tests/test_story.cpp -o test_story.exe
.\test_story.exe
%GXX% %CXXFLAGS% src/ui/ConsoleUI.cpp src/ui/ASCIIArt.cpp tests/test_ui.cpp -o test_ui.exe
.\test_ui.exe
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
