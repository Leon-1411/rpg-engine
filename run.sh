#!/usr/bin/env bash
# RPG Engine - Linux / macOS 1-Click Launcher

set -e

echo "================================================================"
echo "       ⚔  RPG ENGINE - FRACTURED CROWN (C++17 BETA)  ⚔"
echo "================================================================"
echo ""

if [ -f "./build/rpg_engine" ]; then
    ./build/rpg_engine
    exit 0
fi

if command -v docker &> /dev/null && docker info &> /dev/null; then
    echo "[RPG Engine] Launching via Docker environment..."
    docker run --rm -it -v "$(pwd):/app" -w /app rpg-engine:dev bash -c "cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release >/dev/null 2>&1 && cmake --build build >/dev/null 2>&1 && ./build/rpg_engine"
    exit 0
fi

if command -v cmake &> /dev/null; then
    echo "[RPG Engine] Compiling locally..."
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ./build/rpg_engine
    exit 0
fi

echo "[Error] No execution environment found. Please install Docker or CMake + g++."
exit 1
