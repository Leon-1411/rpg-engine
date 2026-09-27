@echo off
title RPG Engine - Fractured Crown
chcp 65001 >nul 2>nul

echo ================================================================
echo        ⚔  RPG ENGINE - FRACTURED CROWN (C++17 BETA)  ⚔
echo ================================================================
echo.

if exist "rpg_engine.exe" (
    rpg_engine.exe
) else (
    echo [Loi] Khong tim thay file rpg_engine.exe!
)

echo.
pause
