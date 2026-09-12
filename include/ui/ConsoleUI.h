#pragma once

#include <string>
#include <vector>

namespace ConsoleUI {

namespace Colors {
    // Formatting
    constexpr const char* RESET         = "\033[0m";
    constexpr const char* BOLD          = "\033[1m";
    constexpr const char* DIM           = "\033[2m";
    constexpr const char* ITALIC        = "\033[3m";
    constexpr const char* UNDERLINE     = "\033[4m";

    // Standard Foreground Colors
    constexpr const char* BLACK         = "\033[30m";
    constexpr const char* RED           = "\033[31m";
    constexpr const char* GREEN         = "\033[32m";
    constexpr const char* YELLOW        = "\033[33m";
    constexpr const char* BLUE          = "\033[34m";
    constexpr const char* MAGENTA       = "\033[35m";
    constexpr const char* CYAN          = "\033[36m";
    constexpr const char* WHITE         = "\033[37m";

    // High Intensity / Bright Foreground Colors
    constexpr const char* BRIGHT_BLACK   = "\033[90m";
    constexpr const char* BRIGHT_RED     = "\033[91m";
    constexpr const char* BRIGHT_GREEN   = "\033[92m";
    constexpr const char* BRIGHT_YELLOW  = "\033[93m";
    constexpr const char* BRIGHT_BLUE    = "\033[94m";
    constexpr const char* BRIGHT_MAGENTA = "\033[95m";
    constexpr const char* BRIGHT_CYAN    = "\033[96m";
    constexpr const char* BRIGHT_WHITE   = "\033[97m";

    // Background Colors
    constexpr const char* BG_BLACK       = "\033[40m";
    constexpr const char* BG_RED         = "\033[41m";
    constexpr const char* BG_GREEN       = "\033[42m";
    constexpr const char* BG_YELLOW      = "\033[43m";
    constexpr const char* BG_BLUE        = "\033[44m";
    constexpr const char* BG_MAGENTA     = "\033[45m";
    constexpr const char* BG_CYAN        = "\033[46m";
    constexpr const char* BG_WHITE       = "\033[47m";
} // namespace Colors

// Console initialization & settings
void initConsole();
void setColorEnabled(bool enabled);
bool isColorEnabled();

// Text coloring helper
std::string colorize(const std::string& text, const std::string& ansiCode);

// Screen manipulation
void clearScreen();
void pause(const std::string& prompt = "Nhấn Enter để tiếp tục...");

// Visual layout & formatting helpers
void printHeader(const std::string& title, int width = 60, const std::string& color = Colors::BRIGHT_YELLOW);
void printDivider(char ch = '=', int length = 60, const std::string& color = Colors::CYAN);
void printBox(const std::vector<std::string>& lines, int width = 60, const std::string& borderColor = Colors::BLUE);

// Progress bars (HP, MP, EXP)
std::string formatProgressBar(int current, int max, int width = 20, 
                             const std::string& filledColor = "", 
                             const std::string& emptyColor = Colors::DIM);
void printProgressBar(const std::string& label, int current, int max, int width = 20, 
                      const std::string& filledColor = "");

// Input validation utilities
bool isValidInteger(const std::string& str, long long& outVal);
int getIntInput(int minVal, int maxVal, const std::string& prompt = "Chọn: ", std::istream& in = std::cin);
std::string getStringInput(const std::string& prompt = "> ", std::istream& in = std::cin);

// Status notification helpers
void printSuccess(const std::string& message);
void printError(const std::string& message);
void printWarning(const std::string& message);
void printInfo(const std::string& message);

} // namespace ConsoleUI
