@echo off
title RPG Engine - Fractured Crown (Beta)
chcp 65001 >nul 2>nul

echo ================================================================
echo        ⚔  RPG ENGINE - FRACTURED CROWN (C++17 BETA)  ⚔
echo ================================================================
echo.

if exist "dist\rpg_engine.exe" (
    echo [RPG Engine] Khoi chay ban dung Beta standalone tu dist\...
    cd dist
    rpg_engine.exe
    cd ..
    goto end
)

if exist "rpg_engine.exe" (
    echo [RPG Engine] Khoi chay ban dung san co...
    rpg_engine.exe
    goto end
)

where docker >nul 2>nul
if %ERRORLEVEL% equ 0 (
    docker info >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        echo [RPG Engine] Khoi chay game qua Docker container...
        docker run --rm -it -v "%cd%":/app -w /app rpg-engine:dev bash -c "cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release >/dev/null 2>&1 && cmake --build build >/dev/null 2>&1 && ./build/rpg_engine"
        goto end
    )
)

echo [RPG Engine] Khong tim thay file thuc thi, dang tien hanh build tu dong qua build.bat...
call build.bat
if exist "rpg_engine.exe" (
    rpg_engine.exe
)

:end
echo.
echo [RPG Engine] Cam on ban da trai nghiem game!
pause
