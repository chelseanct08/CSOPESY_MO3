#include <iostream>
#include <string>
#include <windows.h>
#include "Marquee.h"

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

void clearCommandLine(int commandAreaTop, int) {
    const int menuWidth = 42;
    int menuLeft = 2;

    setCursorPosition(
        menuLeft,
        commandAreaTop + 14
    );

    std::cout << std::string(
        menuWidth,
        ' '
    );

    setCursorPosition(
        menuLeft,
        commandAreaTop + 14
    );
}

void displayWelcome(int commandAreaTop, int) {
    int menuLeft = 2;

    setCursorPosition(menuLeft, commandAreaTop);
    std::cout << " Welcome to CSOPESY!";

    setCursorPosition(menuLeft, commandAreaTop + 2);
    std::cout << " Group developer:";

    setCursorPosition(menuLeft, commandAreaTop + 3);
    std::cout << "   Hernaez, Raeka Estrelle";

    setCursorPosition(menuLeft, commandAreaTop + 4);
    std::cout << "   Liwanag, Ram Miguel";

    setCursorPosition(menuLeft, commandAreaTop + 5);
    std::cout << "   Serrano, Paul Rhazzel";

    setCursorPosition(menuLeft, commandAreaTop + 6);
    std::cout << "   Tamayo, Chelsea Nichole";

    setCursorPosition(menuLeft, commandAreaTop + 8);
    std::cout << " Version date: 2026-09-27";
}

void displayMenu(int commandAreaTop, int) {
    int menuLeft = 2;

    setCursorPosition(menuLeft, commandAreaTop);
    std::cout << "========================================";

    setCursorPosition(menuLeft, commandAreaTop + 1);
    std::cout << "              OS EMULATOR";

    setCursorPosition(menuLeft, commandAreaTop + 2);
    std::cout << "========================================";

    setCursorPosition(menuLeft, commandAreaTop + 4);
    std::cout << "                 COMMANDS";

    setCursorPosition(menuLeft, commandAreaTop + 5);
    std::cout << "----------------------------------------";

    setCursorPosition(menuLeft, commandAreaTop + 6);
    std::cout << " help          - Display commands";

    setCursorPosition(menuLeft, commandAreaTop + 7);
    std::cout << " start_marquee - Start marquee animation";

    setCursorPosition(menuLeft, commandAreaTop + 8);
    std::cout << " stop_marquee  - Stop marquee animation";

    setCursorPosition(menuLeft, commandAreaTop + 9);
    std::cout << " set_text      - Set marquee text";

    setCursorPosition(menuLeft, commandAreaTop + 10);
    std::cout << " set_speed     - Set marquee speed in ms";

    setCursorPosition(menuLeft, commandAreaTop + 11);
    std::cout << " exit          - Exit the console";

    setCursorPosition(menuLeft, commandAreaTop + 13);
    std::cout << "----------------------------------------";
}

void displayPrompt(int commandAreaTop, int) {
    int menuLeft = 2;

    setCursorPosition(
        menuLeft,
        commandAreaTop + 14
    );

    std::cout << " Command> ";
}

void printPromptLine(int commandAreaTop, const std::string& message) {
    setCursorPosition(2, commandAreaTop + 14);
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

    const int commandAreaHeight = 19;

    int commandAreaTop =
        windowHeight - commandAreaHeight;

    system("cls");

    Marquee marquee;
    std::string command;
    enum ScreenMode {
        WelcomeScreen,
        MenuScreen
    };

    ScreenMode currentScreen = WelcomeScreen;

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
        std::getline(
            std::cin,
            command
        );

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
                windowHeight - commandAreaHeight;

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

            commandAreaTop =
                windowHeight - commandAreaHeight;

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
                windowWidth
            );

            printPromptLine(
                commandAreaTop,
                " Enter marquee text: "
            );

            std::getline(
                std::cin,
                text
            );

            marquee.setText(text);
            if (!marquee.isRunning()) {
                marquee.start();
            }

            clearCommandLine(
                commandAreaTop,
                windowWidth
            );

            printPromptLine(
                commandAreaTop,
                " Command> "
            );
        }
        else if (command == "set_speed") {
            int speed;

            clearCommandLine(
                commandAreaTop,
                windowWidth
            );

            printPromptLine(
                commandAreaTop,
                " Enter speed in milliseconds: "
            );

            std::cin >> speed;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');

                clearCommandLine(
                    commandAreaTop,
                    windowWidth
                );

                printPromptLine(
                    commandAreaTop,
                    " Invalid speed."
                );

                Sleep(1000);

                clearCommandLine(
                    commandAreaTop,
                    windowWidth
                );

                printPromptLine(
                    commandAreaTop,
                    " Command> "
                );
            }
            else if (speed <= 0) {
                std::cin.ignore(1000, '\n');

                clearCommandLine(
                    commandAreaTop,
                    windowWidth
                );

                printPromptLine(
                    commandAreaTop,
                    " Invalid speed."
                );

                Sleep(1000);

                clearCommandLine(
                    commandAreaTop,
                    windowWidth
                );

                printPromptLine(
                    commandAreaTop,
                    " Command> "
                );
            }
            else {
                std::cin.ignore(1000, '\n');

                marquee.setSpeed(speed);

                clearCommandLine(
                    commandAreaTop,
                    windowWidth
                );

                printPromptLine(
                    commandAreaTop,
                    " Command> "
                );
            }
        }
        else if (command == "exit") {
            marquee.stop();

            clearEntireConsole();

            getConsoleSize(
                windowWidth,
                windowHeight
            );

            setCursorPosition(
                2,
                windowHeight - 1
            );

            std::cout << " OS EMULATOR";
            break;
        }
        else {
            clearCommandLine(
                commandAreaTop,
                windowWidth
            );

            printPromptLine(
                commandAreaTop,
                " Unknown command."
            );

            Sleep(1000);

            clearCommandLine(
                commandAreaTop,
                windowWidth
            );

            printPromptLine(
                commandAreaTop,
                " Command> "
            );
        }
    }

    return 0;
}