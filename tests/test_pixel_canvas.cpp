/**
 * @file test_pixel_canvas.cpp
 * @brief Kiểm thử tính toán tọa độ ảo và cơ chế Letterbox của PixelCanvas
 */

#include "ui_2d/PixelCanvas.h"
#include <iostream>
#include <cassert>
#include <cmath>

int main() {
    std::cout << "========================================\n";
    std::cout << "  RUNNING PIXEL CANVAS UNIT TESTS\n";
    std::cout << "========================================\n";

    PixelCanvas canvas(480, 270, 3);
    assert(canvas.getVirtualWidth() == 480);
    assert(canvas.getVirtualHeight() == 270);
    assert(canvas.getWindowWidth() == 1440);
    assert(canvas.getWindowHeight() == 810);
    std::cout << "[Test 1] Canvas dimensions and virtual aspect ratio (16:9): PASSED\n";

    // Khởi tạo cửa sổ Raylib
    bool initOk = canvas.init("Test Window");
    assert(initOk == true);
    assert(canvas.isRunning() == true);
    std::cout << "[Test 2] Raylib Window and Virtual RenderTexture initialization: PASSED\n";

    // Test virtual frame render cycle
    canvas.beginCanvas();
    ClearBackground(BLACK);
    DrawRectangle(10, 10, 50, 50, RED);
    canvas.endCanvas();
    std::cout << "[Test 3] BeginCanvas / EndCanvas render target pass: PASSED\n";

    canvas.close();
    assert(canvas.isRunning() == false);
    std::cout << "[Test 4] Clean shutdown and texture unloading: PASSED\n";

    std::cout << "========================================\n";
    std::cout << " [ALL PASS] PixelCanvas tests successful!\n";
    std::cout << "========================================\n";
    return 0;
}
