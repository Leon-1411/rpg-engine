#ifndef PIXEL_CANVAS_H
#define PIXEL_CANVAS_H

#include "raylib.h"

/**
 * @class PixelCanvas
 * @brief Quản lý khung hình ảo (Virtual Canvas) chuẩn Pixel-Art cho RPG Engine.
 * Tự động scale độ phân giải thấp (320x180 hoặc 480x270) lên màn hình thật với bộ lọc Point (không vỡ pixel).
 */
class PixelCanvas {
public:
    PixelCanvas(int virtualWidth = 480, int virtualHeight = 270, int scale = 3);
    ~PixelCanvas();

    bool init(const char* title = "RPG Engine - 2D Pixel Edition");
    void close();

    bool isRunning() const;

    // Bắt đầu vẽ lên Canvas độ phân giải Pixel ảo
    void beginCanvas();
    // Kết thúc vẽ lên Canvas
    void endCanvas();

    // Bắt đầu vẽ màn hình thật (scale & letterbox)
    void renderToScreen();

    int getVirtualWidth() const { return m_virtualWidth; }
    int getVirtualHeight() const { return m_virtualHeight; }
    int getWindowWidth() const { return m_windowWidth; }
    int getWindowHeight() const { return m_windowHeight; }

    // Chuyển đổi toạ độ chuột từ màn hình thật sang toạ độ Canvas ảo
    Vector2 getVirtualMousePosition() const;

private:
    int m_virtualWidth;
    int m_virtualHeight;
    int m_windowWidth;
    int m_windowHeight;
    int m_scale;
    RenderTexture2D m_target;
    bool m_initialized;
};

#endif // PIXEL_CANVAS_H
