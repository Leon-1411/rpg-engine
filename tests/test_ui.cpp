#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include <cassert>
#include <iostream>

int main() {
    // 1. Test ConsoleUI colorizing
    ConsoleUI::setColorEnabled(true);
    std::string colored = ConsoleUI::colorize("Test", ConsoleUI::Colors::RED);
    assert(colored.find("Test") != std::string::npos);
    assert(colored.find("\033[31m") != std::string::npos);

    ConsoleUI::setColorEnabled(false);
    std::string uncolored = ConsoleUI::colorize("Test", ConsoleUI::Colors::RED);
    assert(uncolored == "Test");
    ConsoleUI::setColorEnabled(true);

    // 2. Test progress bar formatting
    std::string pbar = ConsoleUI::formatProgressBar(50, 100, 20);
    assert(pbar.find("50/100") != std::string::npos);

    std::string pbarZero = ConsoleUI::formatProgressBar(0, 100, 20);
    assert(pbarZero.find("0/100") != std::string::npos);

    std::string pbarFull = ConsoleUI::formatProgressBar(100, 100, 20);
    assert(pbarFull.find("100/100") != std::string::npos);

    // 3. Test Art rendering without crash
    ASCIIArt::printTitleLogo();
    ASCIIArt::printWarriorArt();
    ASCIIArt::printBattleBanner();
    ASCIIArt::printVictoryBanner();

    std::cout << "[PASS] UI Console & ANSI unit tests successful!\n";
    return 0;
}
