#include "ui/ConsoleUI.h"
#include <iomanip>
#include <limits>
#include <algorithm>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace ConsoleUI {

static bool g_colorEnabled = true;

void initConsole() {
#ifdef _WIN32
    // Enable Virtual Terminal Processing for ANSI escape codes on Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    // Enable UTF-8 encoding for Unicode symbols and box characters
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    // Flush stdout
    std::cout << std::flush;
}

void setColorEnabled(bool enabled) {
    g_colorEnabled = enabled;
}

bool isColorEnabled() {
    return g_colorEnabled;
}

std::string colorize(const std::string& text, const std::string& ansiCode) {
    if (!g_colorEnabled || ansiCode.empty()) {
        return text;
    }
    return ansiCode + text + Colors::RESET;
}

void clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void pause(const std::string& prompt) {
    std::cout << colorize("\n" + prompt, Colors::DIM);
    std::string line;
    std::getline(std::cin, line);
}

void printHeader(const std::string& title, int width, const std::string& color) {
    if (width < 10) width = 10;
    std::string topBorder    = "╔" + std::string(width - 2, '=') + "╗";
    std::string bottomBorder = "╚" + std::string(width - 2, '=') + "╝";

    int padding = static_cast<int>(width - 2 - title.length());
    int padLeft = (padding > 0) ? padding / 2 : 0;
    int padRight = (padding > 0) ? (padding - padLeft) : 0;

    std::cout << colorize(topBorder, color) << "\n";
    std::cout << colorize("║", color) 
              << std::string(padLeft, ' ')
              << colorize(title, Colors::BOLD + color)
              << std::string(padRight, ' ')
              << colorize("║", color) << "\n";
    std::cout << colorize(bottomBorder, color) << "\n";
}

void printDivider(char ch, int length, const std::string& color) {
    std::string div(length, ch);
    std::cout << colorize(div, color) << "\n";
}

void printBox(const std::vector<std::string>& lines, int width, const std::string& borderColor) {
    std::string hBorder;
    for (int i = 0; i < width - 2; ++i) {
        hBorder += "─";
    }
    std::string topBorder    = "┌" + hBorder + "┐";
    std::string bottomBorder = "└" + hBorder + "┘";

    std::cout << colorize(topBorder, borderColor) << "\n";
    for (const auto& line : lines) {
        int padding = static_cast<int>(width - 4 - line.length());
        if (padding < 0) padding = 0;
        std::cout << colorize("│ ", borderColor)
                  << line
                  << std::string(padding, ' ')
                  << colorize(" │", borderColor) << "\n";
    }
    std::cout << colorize(bottomBorder, borderColor) << "\n";
}

std::string formatProgressBar(int current, int max, int width, 
                             const std::string& filledColor, 
                             const std::string& emptyColor) {
    if (max <= 0) max = 1;
    if (current < 0) current = 0;
    if (current > max) current = max;

    float ratio = static_cast<float>(current) / static_cast<float>(max);
    int filledLength = static_cast<int>(ratio * width);
    int emptyLength = width - filledLength;

    // Automatic color choosing if not explicitly provided
    std::string color = filledColor;
    if (color.empty()) {
        if (ratio > 0.5f) {
            color = Colors::BRIGHT_GREEN;
        } else if (ratio > 0.25f) {
            color = Colors::BRIGHT_YELLOW;
        } else {
            color = Colors::BRIGHT_RED;
        }
    }

    std::string filledBar(filledLength, '#');
    std::string emptyBar(emptyLength, '-');

    std::string result = "[" + colorize(filledBar, color) + colorize(emptyBar, emptyColor) + "] "
                       + colorize(std::to_string(current), color) + "/" + std::to_string(max);
    return result;
}

void printProgressBar(const std::string& label, int current, int max, int width, 
                      const std::string& filledColor) {
    std::cout << std::left << std::setw(8) << label << " "
              << formatProgressBar(current, max, width, filledColor) << "\n";
}

int getIntInput(int minVal, int maxVal, const std::string& prompt) {
    int choice = 0;
    while (true) {
        std::cout << colorize(prompt, Colors::BRIGHT_CYAN);
        if (std::cin >> choice) {
            if (choice >= minVal && choice <= maxVal) {
                // Clear any trailing characters on the line
                std::string trailing;
                std::getline(std::cin, trailing);
                return choice;
            }
            std::cout << colorize("  [!] Lựa chọn phải từ " + std::to_string(minVal) + 
                                  " đến " + std::to_string(maxVal) + ". Vui lòng nhập lại.\n", 
                                  Colors::BRIGHT_RED);
        } else {
            std::cin.clear();
            std::string invalid;
            std::getline(std::cin, invalid);
            std::cout << colorize("  [!] Giá trị không hợp lệ. Vui lòng nhập số.\n", Colors::BRIGHT_RED);
        }
    }
}

std::string getStringInput(const std::string& prompt) {
    std::cout << colorize(prompt, Colors::BRIGHT_CYAN);
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void printSuccess(const std::string& message) {
    std::cout << colorize("  [✓] " + message, Colors::BRIGHT_GREEN) << "\n";
}

void printError(const std::string& message) {
    std::cout << colorize("  [✗] " + message, Colors::BRIGHT_RED) << "\n";
}

void printWarning(const std::string& message) {
    std::cout << colorize("  [!] " + message, Colors::BRIGHT_YELLOW) << "\n";
}

void printInfo(const std::string& message) {
    std::cout << colorize("  [i] " + message, Colors::CYAN) << "\n";
}

} // namespace ConsoleUI
