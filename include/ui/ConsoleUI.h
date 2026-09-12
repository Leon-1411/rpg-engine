#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace ConsoleUI {

namespace Colors {
    // Formatting
    inline const std::string RESET         = "\033[0m";
    inline const std::string BOLD          = "\033[1m";
    inline const std::string DIM           = "\033[2m";
    inline const std::string ITALIC        = "\033[3m";
    inline const std::string UNDERLINE     = "\033[4m";

    // Standard Foreground Colors
    inline const std::string BLACK         = "\033[30m";
    inline const std::string RED           = "\033[31m";
    inline const std::string GREEN         = "\033[32m";
    inline const std::string YELLOW        = "\033[33m";
    inline const std::string BLUE          = "\033[34m";
    inline const std::string MAGENTA       = "\033[35m";
    inline const std::string CYAN          = "\033[36m";
    inline const std::string WHITE         = "\033[37m";

    // High Intensity / Bright Foreground Colors
    inline const std::string BRIGHT_BLACK   = "\033[90m";
    inline const std::string BRIGHT_RED     = "\033[91m";
    inline const std::string BRIGHT_GREEN   = "\033[92m";
    inline const std::string BRIGHT_YELLOW  = "\033[93m";
    inline const std::string BRIGHT_BLUE    = "\033[94m";
    inline const std::string BRIGHT_MAGENTA = "\033[95m";
    inline const std::string BRIGHT_CYAN    = "\033[96m";
    inline const std::string BRIGHT_WHITE   = "\033[97m";

    // Background Colors
    inline const std::string BG_BLACK       = "\033[40m";
    inline const std::string BG_RED         = "\033[41m";
    inline const std::string BG_GREEN       = "\033[42m";
    inline const std::string BG_YELLOW      = "\033[43m";
    inline const std::string BG_BLUE        = "\033[44m";
    inline const std::string BG_MAGENTA     = "\033[45m";
    inline const std::string BG_CYAN        = "\033[46m";
    inline const std::string BG_WHITE       = "\033[47m";
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
