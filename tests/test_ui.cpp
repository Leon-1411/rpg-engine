#include "ui/ConsoleUI.h"
#include "ui/ASCIIArt.h"
#include <cassert>
#include <iostream>
#include <sstream>

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

    // 4. Test isValidInteger validation helper
    long long parsedVal = 0;
    assert(ConsoleUI::isValidInteger("42", parsedVal) == true && parsedVal == 42);
    assert(ConsoleUI::isValidInteger("  -15  ", parsedVal) == true && parsedVal == -15);
    assert(ConsoleUI::isValidInteger("+7", parsedVal) == true && parsedVal == 7);
    assert(ConsoleUI::isValidInteger("abc", parsedVal) == false);
    assert(ConsoleUI::isValidInteger("1a", parsedVal) == false);
    assert(ConsoleUI::isValidInteger("a1", parsedVal) == false);
    assert(ConsoleUI::isValidInteger("", parsedVal) == false);
    assert(ConsoleUI::isValidInteger("   ", parsedVal) == false);
    assert(ConsoleUI::isValidInteger("3.14", parsedVal) == false);
    assert(ConsoleUI::isValidInteger("!@#", parsedVal) == false);

    // 5. Test getIntInput handling invalid letters (nhập chữ vào ô số)
    {
        std::istringstream stream("abc\nxyz\n1a\n3\n");
        int res = ConsoleUI::getIntInput(1, 5, "Test prompt: ", stream);
        assert(res == 3);
    }

    // 6. Test getIntInput handling empty lines (Enter)
    {
        std::istringstream stream("\n  \n\t\n4\n");
        int res = ConsoleUI::getIntInput(1, 5, "Test prompt: ", stream);
        assert(res == 4);
    }

    // 7. Test getIntInput handling out-of-range numbers
    {
        std::istringstream stream("999\n-5\n2\n");
        int res = ConsoleUI::getIntInput(1, 5, "Test prompt: ", stream);
        assert(res == 2);
    }

    // 8. Test getIntInput handling trimmed whitespace
    {
        std::istringstream stream("   5   \n");
        int res = ConsoleUI::getIntInput(1, 5, "Test prompt: ", stream);
        assert(res == 5);
    }

    // 9. Test getIntInput handling EOF safely without infinite loop
    {
        std::istringstream stream("");
        int res = ConsoleUI::getIntInput(1, 5, "Test prompt: ", stream);
        assert(res == 1);
    }

    std::cout << "[PASS] UI Console, ANSI & Input Validation unit tests successful!\n";
    return 0;
}
