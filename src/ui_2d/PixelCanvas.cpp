#include "ui_2d/PixelCanvas.h"
#include <algorithm>

PixelCanvas::PixelCanvas(int virtualWidth, int virtualHeight, int scale)
    : m_virtualWidth(virtualWidth)
    , m_virtualHeight(virtualHeight)
    , m_scale(scale)
    , m_windowWidth(virtualWidth * scale)
    , m_windowHeight(virtualHeight * scale)
    , m_initialized(false)
{
}

PixelCanvas::~PixelCanvas() {
    close();
}

bool PixelCanvas::init(const char* title) {
    if (m_initialized) return true;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(m_windowWidth, m_windowHeight, title);
    SetTargetFPS(60);

    m_target = LoadRenderTexture(m_virtualWidth, m_virtualHeight);
    SetTextureFilter(m_target.texture, TEXTURE_FILTER_POINT);

    m_initialized = true;
    return true;
}

void PixelCanvas::close() {
    if (m_initialized) {
        UnloadRenderTexture(m_target);
        CloseWindow();
        m_initialized = false;
    }
}

bool PixelCanvas::isRunning() const {
    return m_initialized && !WindowShouldClose();
}

void PixelCanvas::beginCanvas() {
    BeginTextureMode(m_target);
}

void PixelCanvas::endCanvas() {
    EndTextureMode();
}

void PixelCanvas::renderToScreen() {
    m_windowWidth = GetScreenWidth();
    m_windowHeight = GetScreenHeight();

    // Tính toán tỷ lệ co giãn giữ nguyên tỷ lệ khung hình (Letterbox)
    float scale = std::min((float)m_windowWidth / m_virtualWidth, (float)m_windowHeight / m_virtualHeight);

    Rectangle sourceRec = { 0.0f, 0.0f, (float)m_virtualWidth, -(float)m_virtualHeight }; // Lật Y cho RenderTexture
    Rectangle destRec = {
        (m_windowWidth - ((float)m_virtualWidth * scale)) * 0.5f,
        (m_windowHeight - ((float)m_virtualHeight * scale)) * 0.5f,
        (float)m_virtualWidth * scale,
        (float)m_virtualHeight * scale
    };

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(m_target.texture, sourceRec, destRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    EndDrawing();
}

Vector2 PixelCanvas::getVirtualMousePosition() const {
    Vector2 mouse = GetMousePosition();
    float scale = std::min((float)GetScreenWidth() / m_virtualWidth, (float)GetScreenHeight() / m_virtualHeight);

    Vector2 virtualMouse = { 0 };
    virtualMouse.x = (mouse.x - (GetScreenWidth() - (m_virtualWidth * scale)) * 0.5f) / scale;
    virtualMouse.y = (mouse.y - (GetScreenHeight() - (m_virtualHeight * scale)) * 0.5f) / scale;

    virtualMouse.x = std::max(0.0f, std::min(virtualMouse.x, (float)m_virtualWidth));
    virtualMouse.y = std::max(0.0f, std::min(virtualMouse.y, (float)m_virtualHeight));

    return virtualMouse;
}
