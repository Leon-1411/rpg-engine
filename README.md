# RPG Engine

Game nhập vai Console dựa trên C++17 với kiến trúc Mô-đun (Feature-Based Architecture).

## 📁 Cấu trúc Thư mục & Phân công Thành viên

```text
rpg-engine/
├── .gitignore
├── README.md
├── CMakeLists.txt
├── include/                 <-- Thư mục chứa các file .h khung sườn
│   ├── Hero.h
│   ├── Enemy.h
│   ├── Item.h
│   ├── Inventory.h
│   ├── CombatEngine.h
│   ├── StoryGraph.h
│   └── SaveManager.h
├── src/                     <-- Thư mục chứa code triển khai .cpp
│   ├── Hero.cpp             (Phụ trách: Quý)
│   ├── Enemy.cpp            (Phụ trách: Nhật)
│   ├── Item.cpp             (Phụ trách: Sang)
│   ├── Inventory.cpp        (Phụ trách: Sang)
│   ├── CombatEngine.cpp     (Phụ trách: Lợi)
│   ├── StoryGraph.cpp       (Phụ trách: Nghĩa)
│   ├── SaveManager.cpp      (Phụ trách: Phong)
│   └── main.cpp             (Phụ trách: Quang - Ráp nối toàn bộ)
└── tests/                   <-- Thư mục test độc lập
    ├── test_hero.cpp
    ├── test_enemy.cpp
    ├── test_item.cpp
    ├── test_inventory.cpp
    ├── test_combat.cpp
    ├── test_story.cpp
    └── test_save.cpp
```

## 🛠️ Hướng dẫn Biên dịch & Chạy dự án

### Yêu cầu
- C++17 Compiler (GCC, Clang, MSVC)
- CMake 3.14 trở lên

### Các bước Build
```bash
# 1. Tạo thư mục build
cmake -B build -S .

# 2. Biên dịch dự án
cmake --build build

# 3. Chạy Game
./build/rpg_engine           # Linux/macOS
.\build\Debug\rpg_engine.exe # Windows (MSVC)
```

### Chạy các Unit Test
```bash
cd build
ctest --output-on-failure
```
