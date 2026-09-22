# 🎮 RPG Engine: Fractured Crown - Quick Start Guide (Hướng Dẫn Trải Nghiệm Nhanh)

Cảm ơn bạn đã tải bản dựng **Beta Standalone** của **RPG Engine**! Bản dựng này đã được đóng gói độc lập toàn diện và có thể chạy ngay trên mọi máy tính Windows mà **không cần cài đặt bất kỳ công cụ lập trình hay thư viện phụ trợ nào**.

---

## ⚡ 1. Cách Chơi Game Ngay Lập Tức (1-Click Run)

Bạn có 2 cách siêu đơn giản để mở game:

* **Cách 1 (Khuyên dùng)**: Click đúp (Double-click) trực tiếp vào file **`rpg_engine.exe`**.
* **Cách 2**: Click đúp vào file **`run.bat`** (Tự động kích hoạt font UTF-8 và bảng màu sắc ANSI).

---

## 📂 2. Cấu Trúc Gói Phát Hành

```text
dist/
├── 🎮 rpg_engine.exe      <--- File game chính (Click đúp để chơi)
├── ⚡ run.bat             <--- Script hỗ trợ 1-click
├── 📖 QUICK_START.md      <--- Hướng dẫn sử dụng này
├── 📂 data/               <--- Dữ liệu cốt truyện, quái vật, vật phẩm
│   ├── story.json         (22 Nodes cốt truyện Fractured Crown)
│   ├── enemies.json       (3 Canon Minions & 6 Faction Bosses)
│   └── items.json         (Vũ khí, Giáp, Dược phẩm)
└── 📂 saves/              <--- Thư mục tự động lưu tiến trình chơi (slot 1-9)
```

---

## 🕹️ 3. Phím Tắt & Thao Tác Trong Game

* **Nhập số [1, 2, 3...]**: Chọn hành động, kỹ năng, vật phẩm hoặc ngã rẽ cốt truyện theo menu.
* **Phím Enter**: Xác nhận lựa chọn hoặc tiếp tục cốt truyện.
* **Lưu game**: Chọn mục `Lưu game (Save Slot 1)` trong menu tiện ích khi đang khám phá cốt truyện.
