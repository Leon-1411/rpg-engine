#include "ui/ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <cstdlib>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <sys/ioctl.h>
#include <unistd.h>
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

void pause(const std::string& prompt, std::istream& in) {
    if (in.eof()) return;
    std::cout << colorize("\n" + prompt, Colors::DIM);
    std::string line;
    std::getline(in, line);
}

int getTerminalWidth() {
    int width = 100; // default standard width
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
            int consoleCols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            if (consoleCols > 0) {
                width = consoleCols;
            }
        }
    }
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        width = ws.ws_col;
    } else {
        const char* colEnv = std::getenv("COLUMNS");
        if (colEnv) {
            try {
                int cols = std::stoi(colEnv);
                if (cols > 0) width = cols;
            } catch (...) {}
        }
    }
#endif
    // Clamp to [60, 110] for optimal reading experience across all displays
    if (width < 60) width = 60;
    if (width > 110) width = 110;
    return width;
}

size_t getDisplayWidth(const std::string& str) {
    size_t width = 0;
    size_t i = 0;
    while (i < str.length()) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        // Handle ANSI escape sequences like \033[31m or \033[0m
        if (c == '\033' && i + 1 < str.length() && str[i + 1] == '[') {
            i += 2;
            while (i < str.length() && str[i] != 'm') {
                i++;
            }
            if (i < str.length() && str[i] == 'm') {
                i++;
            }
            continue;
        }

        // UTF-8 decoding
        if ((c & 0x80) == 0) {
            // Standard 1-byte ASCII
            width += 1;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte UTF-8 character (Vietnamese accents)
            width += 1;
            i += (i + 2 <= str.length()) ? 2 : 1;
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte UTF-8 character (Vietnamese compound vowels & symbols)
            width += 1;
            i += (i + 3 <= str.length()) ? 3 : 1;
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte UTF-8 character (Emoji/wide chars)
            width += 2;
            i += (i + 4 <= str.length()) ? 4 : 1;
        } else {
            // Continuation or invalid byte
            i += 1;
        }
    }
    return width;
}

std::vector<std::string> wrapText(const std::string& text, size_t maxWidth) {
    std::vector<std::string> result;
    if (maxWidth < 1) maxWidth = 1;
    if (text.empty()) {
        result.push_back("");
        return result;
    }

    size_t startPos = 0;
    while (startPos <= text.length()) {
        size_t newlinePos = text.find('\n', startPos);
        std::string paragraph;
        if (newlinePos == std::string::npos) {
            paragraph = text.substr(startPos);
            startPos = text.length() + 1;
        } else {
            paragraph = text.substr(startPos, newlinePos - startPos);
            startPos = newlinePos + 1;
        }

        // Tokenize paragraph into words
        std::vector<std::string> words;
        size_t i = 0;
        while (i < paragraph.length()) {
            while (i < paragraph.length() && (paragraph[i] == ' ' || paragraph[i] == '\t' || paragraph[i] == '\r')) {
                i++;
            }
            if (i >= paragraph.length()) break;
            size_t wordStart = i;
            while (i < paragraph.length() && paragraph[i] != ' ' && paragraph[i] != '\t' && paragraph[i] != '\r') {
                i++;
            }
            words.push_back(paragraph.substr(wordStart, i - wordStart));
        }

        if (words.empty()) {
            result.push_back("");
            continue;
        }

        std::string currentLine = "";
        size_t currentWidth = 0;

        for (const auto& word : words) {
            size_t wordWidth = getDisplayWidth(word);

            if (currentLine.empty()) {
                if (wordWidth <= maxWidth) {
                    currentLine = word;
                    currentWidth = wordWidth;
                } else {
                    // Single word exceeds maxWidth, break character-by-character
                    size_t charIdx = 0;
                    std::string part = "";
                    size_t partWidth = 0;
                    while (charIdx < word.length()) {
                        size_t charBytes = 1;
                        unsigned char uc = static_cast<unsigned char>(word[charIdx]);
                        if ((uc & 0x80) == 0) charBytes = 1;
                        else if ((uc & 0xE0) == 0xC0) charBytes = 2;
                        else if ((uc & 0xF0) == 0xE0) charBytes = 3;
                        else if ((uc & 0xF8) == 0xF0) charBytes = 4;

                        if (charIdx + charBytes > word.length()) charBytes = word.length() - charIdx;
                        std::string ch = word.substr(charIdx, charBytes);
                        size_t cw = getDisplayWidth(ch);

                        if (partWidth + cw > maxWidth && !part.empty()) {
                            result.push_back(part);
                            part = ch;
                            partWidth = cw;
                        } else {
                            part += ch;
                            partWidth += cw;
                        }
                        charIdx += charBytes;
                    }
                    if (!part.empty()) {
                        currentLine = part;
                        currentWidth = partWidth;
                    }
                }
            } else {
                if (currentWidth + 1 + wordWidth <= maxWidth) {
                    currentLine += " " + word;
                    currentWidth += 1 + wordWidth;
                } else {
                    result.push_back(currentLine);
                    if (wordWidth <= maxWidth) {
                        currentLine = word;
                        currentWidth = wordWidth;
                    } else {
                        // Word exceeds maxWidth
                        size_t charIdx = 0;
                        std::string part = "";
                        size_t partWidth = 0;
                        while (charIdx < word.length()) {
                            size_t charBytes = 1;
                            unsigned char uc = static_cast<unsigned char>(word[charIdx]);
                            if ((uc & 0x80) == 0) charBytes = 1;
                            else if ((uc & 0xE0) == 0xC0) charBytes = 2;
                            else if ((uc & 0xF0) == 0xE0) charBytes = 3;
                            else if ((uc & 0xF8) == 0xF0) charBytes = 4;

                            if (charIdx + charBytes > word.length()) charBytes = word.length() - charIdx;
                            std::string ch = word.substr(charIdx, charBytes);
                            size_t cw = getDisplayWidth(ch);

                            if (partWidth + cw > maxWidth && !part.empty()) {
                                result.push_back(part);
                                part = ch;
                                partWidth = cw;
                            } else {
                                part += ch;
                                partWidth += cw;
                            }
                            charIdx += charBytes;
                        }
                        currentLine = part;
                        currentWidth = partWidth;
                    }
                }
            }
        }

        if (!currentLine.empty()) {
            result.push_back(currentLine);
        }
    }

    return result;
}

void printHeader(const std::string& title, int width, const std::string& color) {
    if (width <= 0) {
        width = getTerminalWidth();
    }
    size_t titleWidth = getDisplayWidth(title);
    if (width < static_cast<int>(titleWidth + 4)) {
        width = static_cast<int>(titleWidth + 4);
    }
    if (width < 10) width = 10;

    std::string hBorder;
    for (int i = 0; i < width - 2; ++i) {
        hBorder += "═";
    }
    std::string topBorder    = "╔" + hBorder + "╗";
    std::string bottomBorder = "╚" + hBorder + "╝";

    int padding = static_cast<int>(width - 2 - titleWidth);
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
    if (length <= 0) {
        length = getTerminalWidth();
    }
    std::string div(length, ch);
    std::cout << colorize(div, color) << "\n";
}

void printBox(const std::vector<std::string>& lines, int width, const std::string& borderColor) {
    if (width <= 0) {
        width = getTerminalWidth();
    }
    if (width < 20) width = 20;

    int contentWidth = width - 4;
    if (contentWidth < 1) contentWidth = 1;

    std::vector<std::string> wrappedLines;
    for (const auto& line : lines) {
        std::vector<std::string> parts = wrapText(line, contentWidth);
        for (const auto& p : parts) {
            wrappedLines.push_back(p);
        }
    }

    std::string hBorder;
    for (int i = 0; i < width - 2; ++i) {
        hBorder += "─";
    }
    std::string topBorder    = "┌" + hBorder + "┐";
    std::string bottomBorder = "└" + hBorder + "┘";

    std::cout << colorize(topBorder, borderColor) << "\n";
    for (const auto& line : wrappedLines) {
        size_t lineWidth = getDisplayWidth(line);
        int padding = contentWidth - static_cast<int>(lineWidth);
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

    std::string countText = std::to_string(current) + "/" + std::to_string(max);
    std::string result = "[" + colorize(filledBar, color) + colorize(emptyBar, emptyColor) + "] "
                       + colorize(countText, color);
    return result;
}

void printProgressBar(const std::string& label, int current, int max, int width, 
                      const std::string& filledColor) {
    std::cout << std::left << std::setw(8) << label << " "
              << formatProgressBar(current, max, width, filledColor) << "\n";
}

bool isValidInteger(const std::string& str, long long& outVal) {
    if (str.empty()) return false;
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return false;
    size_t end = str.find_last_not_of(" \t\r\n");
    std::string trimmed = str.substr(start, end - start + 1);

    size_t i = 0;
    if (trimmed[0] == '+' || trimmed[0] == '-') {
        if (trimmed.length() == 1) return false;
        i = 1;
    }
    for (; i < trimmed.length(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(trimmed[i]))) {
            return false;
        }
    }
    try {
        outVal = std::stoll(trimmed);
        return true;
    } catch (...) {
        return false;
    }
}

int getIntInput(int minVal, int maxVal, const std::string& prompt, std::istream& in) {
    while (true) {
        std::cout << colorize(prompt, Colors::BRIGHT_CYAN);
        std::string line;
        if (!std::getline(in, line)) {
            return minVal;
        }

        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            std::cout << colorize("  [!] Bạn chưa nhập gì. Vui lòng nhập một số từ " + 
                                  std::to_string(minVal) + " đến " + std::to_string(maxVal) + ".\n", 
                                  Colors::BRIGHT_YELLOW);
            continue;
        }
        size_t end = line.find_last_not_of(" \t\r\n");
        std::string trimmed = line.substr(start, end - start + 1);

        bool hasNonDigit = false;
        size_t i = 0;
        if (trimmed[0] == '+' || trimmed[0] == '-') {
            if (trimmed.length() == 1) hasNonDigit = true;
            i = 1;
        }
        for (; i < trimmed.length(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(trimmed[i]))) {
                hasNonDigit = true;
                break;
            }
        }

        if (hasNonDigit) {
            std::cout << colorize("  [!] Ký tự không hợp lệ! Bạn đã nhập chữ hoặc ký hiệu thay vì số: \"" + 
                                  trimmed + "\". Vui lòng chỉ nhập số nguyên!\n", 
                                  Colors::BRIGHT_RED);
            continue;
        }

        try {
            long long val = std::stoll(trimmed);
            if (val < minVal || val > maxVal) {
                std::cout << colorize("  [!] Số đã nhập (" + std::to_string(val) + 
                                      ") nằm ngoài phạm vi [" + std::to_string(minVal) + 
                                      " - " + std::to_string(maxVal) + "]. Vui lòng nhập lại.\n", 
                                      Colors::BRIGHT_YELLOW);
                continue;
            }
            return static_cast<int>(val);
        } catch (const std::out_of_range&) {
            std::cout << colorize("  [!] Số vượt giới hạn xử lý! Vui lòng nhập lại trong khoảng [" + 
                                  std::to_string(minVal) + " - " + std::to_string(maxVal) + "].\n", 
                                  Colors::BRIGHT_RED);
            continue;
        } catch (...) {
            std::cout << colorize("  [!] Lỗi chuyển đổi số. Vui lòng nhập lại.\n", Colors::BRIGHT_RED);
            continue;
        }
    }
}

std::string getStringInput(const std::string& prompt, std::istream& in) {
    std::cout << colorize(prompt, Colors::BRIGHT_CYAN);
    std::string input;
    if (!std::getline(in, input)) {
        return "";
    }
    if (!input.empty() && input.back() == '\r') {
        input.pop_back();
    }
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

void printWIPWarning(const std::string& featureName) {
    std::string msg = "⚠️ [THÔNG BÁO] Tính năng đang được phát triển (Feature under development)";
    if (!featureName.empty()) {
        msg += ": " + featureName;
    }
    std::cout << "\n" << colorize(msg, Colors::BRIGHT_YELLOW) << "\n";
    pause("Nhấn Enter để quay lại...");
}

} // namespace ConsoleUI
