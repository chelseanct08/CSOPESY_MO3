#include <iostream>
#include <string>
#include <charconv>
#include <system_error>
#include <windows.h>
#include "Marquee.h"

namespace {
constexpr int inputLeft = 2;
constexpr int inputRight = marqueeBorderColumn - 1;

void clearCommandLine(int promptRow, int windowHeight) {
    const int inputWidth = inputRight - inputLeft + 1;

    for (int row = promptRow; row < windowHeight; ++row) {
        setCursorPosition(inputLeft, row);
        std::cout << std::string(inputWidth, ' ');
    }

    setCursorPosition(inputLeft, promptRow);
}

std::string readInputLine(int promptRow, const std::string& prompt) {
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD originalMode = 0;
    GetConsoleMode(input, &originalMode);

    SetConsoleMode(
        input,
        originalMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)
    );

    setCursorPosition(inputLeft, promptRow);
    std::cout << prompt;

    const int promptLength = static_cast<int>(prompt.length());
    int cursorColumn = inputLeft + promptLength;
    int cursorRow = promptRow;
    std::string value;
    INPUT_RECORD record;
    DWORD recordsRead = 0;

    while (true) {
        ReadConsoleInput(input, &record, 1, &recordsRead);

        if (record.EventType != KEY_EVENT ||
            !record.Event.KeyEvent.bKeyDown) {
            continue;
        }

        const KEY_EVENT_RECORD& key = record.Event.KeyEvent;
        if (key.wVirtualKeyCode == VK_RETURN) {
            break;
        }

        if (key.wVirtualKeyCode == VK_BACK) {
            if (value.empty()) {
                continue;
            }

            value.pop_back();
            if (cursorColumn == inputLeft) {
                --cursorRow;
                cursorColumn = inputRight + 1;
            }
            --cursorColumn;
            setCursorPosition(cursorColumn, cursorRow);
            std::cout << ' ';
            setCursorPosition(cursorColumn, cursorRow);
            continue;
        }

        const char character = key.uChar.AsciiChar;
        if (character < 32 || character > 126) {
            continue;
        }

        if (cursorColumn > inputRight) {
            ++cursorRow;
            cursorColumn = inputLeft;
            setCursorPosition(cursorColumn, cursorRow);
        }

        value += character;
        std::cout << character;
        ++cursorColumn;
    }

    SetConsoleMode(input, originalMode);
    return value;
}

bool parsePositiveSpeed(const std::string& input, int& speed) {
    const char* begin = input.data();
    const char* end = begin + input.size();
    const auto result = std::from_chars(begin, end, speed);

    return result.ec == std::errc() && result.ptr == end && speed > 0;
}
}

void getConsoleSize(int& windowWidth, int& windowHeight) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

    GetConsoleScreenBufferInfo(
        console,
        &consoleInfo
    );

    windowWidth =
        consoleInfo.srWindow.Right -
        consoleInfo.srWindow.Left + 1;

    windowHeight =
        consoleInfo.srWindow.Bottom -
        consoleInfo.srWindow.Top + 1;
}

void clearEntireConsole() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    if (!GetConsoleScreenBufferInfo(console, &consoleInfo)) {
        return;
    }

    COORD home = { 0, 0 };
    DWORD cellCount =
        static_cast<DWORD>(consoleInfo.dwSize.X) *
        static_cast<DWORD>(consoleInfo.dwSize.Y);

    DWORD written = 0;
    FillConsoleOutputCharacterA(
        console,
        ' ',
        cellCount,
        home,
        &written
    );

    FillConsoleOutputAttribute(
        console,
        consoleInfo.wAttributes,
        cellCount,
        home,
        &written
    );

    SetConsoleCursorPosition(console, home);
}

void displayWelcome(int, int) {
    int menuLeft = 2;
    const int welcomeTop = 0;
    setCursorPosition(menuLeft, welcomeTop);
    std::cout << " Welcome to CSOPESY!";
    setCursorPosition(menuLeft, welcomeTop + 1);
    std::cout << " Group developer:";
    setCursorPosition(menuLeft, welcomeTop + 2);
    std::cout << "   Hernaez, Raeka Estrelle";
    setCursorPosition(menuLeft, welcomeTop + 3);
    std::cout << "   Liwanag, Ram Miguel";
    setCursorPosition(menuLeft, welcomeTop + 4);
    std::cout << "   Serrano, Paul Rhazzel";
    setCursorPosition(menuLeft, welcomeTop + 5);
    std::cout << "   Tamayo, Chelsea Nichole";
    setCursorPosition(menuLeft, welcomeTop + 6);
    std::cout << " Version date: 2026-09-27";
    setCursorPosition(menuLeft, welcomeTop + 7);
    std::cout << " enter \"help\" to start and display commands";
}

void displayMenu(int, int) {
    int menuLeft = 2;
    const int menuTop = 0;

    setCursorPosition(menuLeft, menuTop);
    std::cout << "========================================";
    setCursorPosition(menuLeft, menuTop + 1);
    std::cout << "           GRAPHICAL MARQUEE";
    setCursorPosition(menuLeft, menuTop + 2);
    std::cout << "========================================";
    setCursorPosition(menuLeft, menuTop + 3);
    std::cout << "               COMMANDS";
    setCursorPosition(menuLeft, menuTop + 4);
    std::cout << "----------------------------------------";
    setCursorPosition(menuLeft, menuTop + 5);
    std::cout << " help          - Display commands";
    setCursorPosition(menuLeft, menuTop + 6);
    std::cout << " start_marquee - Start marquee animation";
    setCursorPosition(menuLeft, menuTop + 7);
    std::cout << " stop_marquee  - Stop marquee animation";
    setCursorPosition(menuLeft, menuTop + 8);
    std::cout << " set_text      - Set marquee text";
    setCursorPosition(menuLeft, menuTop + 9);
    std::cout << " set_speed     - Set marquee speed in ms";
    setCursorPosition(menuLeft, menuTop + 10);
    std::cout << " exit          - Exit the console";
    setCursorPosition(menuLeft, menuTop + 11);
    std::cout << "----------------------------------------";
}

void displayPrompt(int promptRow, int) {
    int menuLeft = 2;

    setCursorPosition(
        menuLeft,
        promptRow
    );

    std::cout << " Enter command: ";
}

void printPromptLine(int promptRow, const std::string& message) {
    setCursorPosition(2, promptRow);
    std::cout << message;
}

int main() {
    int windowWidth;
    int windowHeight;

    getConsoleSize(
        windowWidth,
        windowHeight
    );

    int previousWidth = windowWidth;
    int previousHeight = windowHeight;

    enum ScreenMode {
        WelcomeScreen,
        MenuScreen
    };

    Marquee marquee;
    std::string command;
    ScreenMode currentScreen = WelcomeScreen;
    int commandAreaTop = 8;

    system("cls");

    auto redrawScreen = [&]() {
        system("cls");

        if (currentScreen == MenuScreen) {
            displayMenu(
                commandAreaTop,
                windowWidth
            );
        }
        else {
            displayWelcome(
                commandAreaTop,
                windowWidth
            );
        }

        displayPrompt(
            commandAreaTop,
            windowWidth
        );
    };

    displayWelcome(
        commandAreaTop,
        windowWidth
    );

    displayPrompt(
        commandAreaTop,
        windowWidth
    );

    while (true) {
        clearCommandLine(
            commandAreaTop,
            windowHeight
        );
        command = readInputLine(commandAreaTop, " Enter command: ");

        getConsoleSize(
            windowWidth,
            windowHeight
        );

        bool resized =
            windowWidth != previousWidth ||
            windowHeight != previousHeight;

        if (resized) {
            previousWidth = windowWidth;
            previousHeight = windowHeight;

            const bool marqueeWasRunning = marquee.isRunning();
            if (marqueeWasRunning) {
                marquee.stop();
            }

            commandAreaTop =
                currentScreen == MenuScreen ? 13 : 8;

            redrawScreen();

            if (marqueeWasRunning) {
                marquee.start();
            }
        }

        if (command == "help") {
            bool marqueeWasRunning = marquee.isRunning();
            if (marqueeWasRunning) {
                marquee.stop();
            }

            getConsoleSize(
                windowWidth,
                windowHeight
            );

            previousWidth = windowWidth;
            previousHeight = windowHeight;

            commandAreaTop = 13;

            currentScreen = MenuScreen;
            redrawScreen();

            if (marqueeWasRunning) {
                marquee.start();
            }
        }
        else if (command == "start_marquee") {
            if (marquee.isRunning()) {
                marquee.stop();
            }

            redrawScreen();
            marquee.start();

            displayPrompt(
                commandAreaTop,
                windowWidth
            );
        }
        else if (command == "stop_marquee") {
            marquee.stop();
            redrawScreen();
        }
        else if (command == "set_text") {
            std::string text;

            clearCommandLine(
                commandAreaTop,
                windowHeight
            );

            text = readInputLine(
                commandAreaTop,
                " Enter marquee text: "
            );

            marquee.setText(text);

            clearCommandLine(
                commandAreaTop,
                windowHeight
            );

            printPromptLine(
                commandAreaTop,
                " Enter command: "
            );
        }
        else if (command == "set_speed") {
            int speed;

            clearCommandLine(
                commandAreaTop,
                windowHeight
            );

            std::string speedInput = readInputLine(
                commandAreaTop,
                " Enter speed in milliseconds: "
            );

            if (!parsePositiveSpeed(speedInput, speed)) {

                clearCommandLine(
                    commandAreaTop,
                    windowHeight
                );

                printPromptLine(
                    commandAreaTop,
                    " Invalid speed."
                );

                Sleep(1000);

                clearCommandLine(
                    commandAreaTop,
                    windowHeight
                );

                printPromptLine(
                    commandAreaTop,
                    " Enter command: "
                );
            }
            else {
                marquee.setSpeed(speed);

                clearCommandLine(
                    commandAreaTop,
                    windowHeight
                );

                printPromptLine(
                    commandAreaTop,
                    " Enter command: "
                );
            }
        }
        else if (command == "exit") {
            marquee.stop();
            clearEntireConsole();
            break;
        }
        else {
            clearCommandLine(
                commandAreaTop,
                windowHeight
            );

            printPromptLine(
                commandAreaTop,
                " Unknown command."
            );

            Sleep(1000);

            clearCommandLine(
                commandAreaTop,
                windowHeight
            );

            printPromptLine(
                commandAreaTop,
                " Enter command: "
            );
        }
    }

    return 0;
}