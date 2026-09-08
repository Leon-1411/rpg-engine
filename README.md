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

---

## 🐳 Hướng dẫn Chạy bằng Docker & Docker Compose

Nếu máy bạn chưa cài sẵn CMake hoặc C++ Toolchain, bạn có thể build và chạy toàn bộ ứng dụng qua Docker.

### 1. Yêu cầu tiên quyết
- Đã cài đặt [Docker Desktop](https://www.docker.com/products/docker-desktop/) và đang khởi chạy.
- Trên Windows: đảm bảo WSL2 backend đã được cài đặt (`wsl --install`).

### 2. Build Docker Images
Biên dịch các stage `builder` và `runner`:
```bash
docker compose build
```

### 3. Chơi Game (Interactive Console UI)
Khởi chạy console game có tương tác bàn phím (lưu dữ liệu tự động vào thư mục `./saves` trên máy thật):
```bash
docker compose run --rm game
```
> **Lưu ý:** Sử dụng `run --rm` thay vì `docker compose up` để kết nối trực tiếp bàn phím (stdin/TTY) cho việc nhập liệu trong game.

### 4. Chạy toàn bộ Unit Tests
```bash
docker compose run --rm test
```

### 5. Mở môi trường Development (C++ Toolchain trong Linux)
Mount toàn bộ mã nguồn vào container để dev, biên dịch và debug trực tiếp:
```bash
docker compose run --rm dev
```
Trong môi trường container, bạn có thể trực tiếp gõ các lệnh như:
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/rpg_engine
```

