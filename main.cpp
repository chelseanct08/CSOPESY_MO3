#include <iostream>
#include <string>
#include <windows.h>
#include <cstdlib>
#include "Marquee.h"

void getConsoleSize(int& windowWidth, int& windowHeight) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    if (GetConsoleScreenBufferInfo(console, &consoleInfo)) {
        windowWidth = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
        windowHeight = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
    }
    else {
        windowWidth = 80;
        windowHeight = 25;
    }
}

void clearCommandLine(int promptRow, int width = 80) {
    int left = 2;
    setCursorPosition(left, promptRow);
    std::cout << std::string(width, ' ');
    setCursorPosition(left, promptRow);
}

void printPromptLine(int promptRow, const std::string& message) {
    setCursorPosition(2, promptRow);
    std::cout << message;
}

void displayHeader(int startRow) {
    int left = 1;

    // CSOPESY Title without border lines
    setCursorPosition(left, startRow);
    std::cout << R"(
   _____  _____  ____  _____  ______  _______     __
  / ____|/ ____|/ __ \|  __ \|  ____|/ ____\ \   / /
 | |    | (___ | |  | | |__) | |__  | (___  \ \_/ / 
 | |     \___ \| |  | |  ___/|  __|  \___ \  \   /  
 | |____ ____) | |__| | |    | |____ ____) |  | |   
  \_____|_____/ \____/|_|    |______|_____/   |_|           

                                                                                                                                                             
  )" << '\n';

    setCursorPosition(left, startRow + 8);
    std::cout << "Welcome to CSOPESY!";

    setCursorPosition(left, startRow + 9);
    std::cout << "Group developer:";
    setCursorPosition(left, startRow + 10);
    std::cout << "  Hernaez, Raeka";
    setCursorPosition(left, startRow + 11);
    std::cout << "  Liwanag, Ram Miguel";
    setCursorPosition(left, startRow + 12);
    std::cout << "  Serrano, Paul";
    setCursorPosition(left, startRow + 13);
    std::cout << "  Tamayo, Chelsea";

    setCursorPosition(left, startRow + 15);
    std::cout << "Version date: Sept 27, 2026";

    setCursorPosition(left, startRow + 17);
    std::cout << "Type 'help' to display available commands.";
}

void displayHelpCommands(int startRow) {
    int left = 2;

    setCursorPosition(left, startRow);
    std::cout << "---------------------- COMMANDS ----------------------";
    setCursorPosition(left, startRow + 1);
    std::cout << "help          - Display commands";
    setCursorPosition(left, startRow + 2);
    std::cout << "start_marquee - Start marquee animation";
    setCursorPosition(left, startRow + 3);
    std::cout << "stop_marquee  - Stop marquee animation";
    setCursorPosition(left, startRow + 4);
    std::cout << "set_text      - Set marquee text";
    setCursorPosition(left, startRow + 5);
    std::cout << "set_speed     - Set speed in ms";
    setCursorPosition(left, startRow + 6);
    std::cout << "clear         - Clear the screen";
    setCursorPosition(left, startRow + 7);
    std::cout << "exit          - Exit the console";
    setCursorPosition(left, startRow + 8);
    std::cout << "------------------------------------------------------";
}

int main() {
    // No window size restrictions: you can resize, expand, or maximize freely
    system("cls");

    int windowWidth = 0;
    int windowHeight = 0;
    getConsoleSize(windowWidth, windowHeight);

    int previousWidth = windowWidth;
    int previousHeight = windowHeight;

    Marquee marquee;
    std::string command;
    bool showHelpMenu = false;

    const int headerStartRow = 1;
    const int helpMenuStartRow = 19;

    auto getPromptRow = [&]() {
        return showHelpMenu ? (helpMenuStartRow + 10) : 19;
        };

    auto renderPrompt = [&]() {
        int row = getPromptRow();
        clearCommandLine(row, windowWidth > 4 ? windowWidth - 4 : 40);
        setCursorPosition(2, row);
        std::cout << "Command> ";
        };

    auto redrawScreen = [&]() {
        system("cls");
        displayHeader(headerStartRow);

        if (showHelpMenu) {
            displayHelpCommands(helpMenuStartRow);
        }

        renderPrompt();
        };

    redrawScreen();

    while (true) {
        int promptRow = getPromptRow();
        setCursorPosition(11, promptRow);

        if (!std::getline(std::cin, command)) {
            break;
        }

        getConsoleSize(windowWidth, windowHeight);
        bool resized = (windowWidth != previousWidth || windowHeight != previousHeight);

        if (resized) {
            previousWidth = windowWidth;
            previousHeight = windowHeight;

            const bool marqueeWasRunning = marquee.isRunning();
            if (marqueeWasRunning) {
                marquee.stop();
            }

            redrawScreen();

            if (marqueeWasRunning) {
                marquee.start();
            }
        }

        promptRow = getPromptRow();

        if (command == "exit") {
            if (marquee.isRunning()) {
                marquee.stop();
            }
            system("cls");
            std::cout << "CSOPESY Emulator Terminated.\n";
            return 0;
        }
        else if (command == "help") {
            const bool marqueeWasRunning = marquee.isRunning();
            if (marqueeWasRunning) {
                marquee.stop();
            }

            showHelpMenu = true;
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
        }
        else if (command == "stop_marquee") {
            marquee.stop();
            redrawScreen();
        }
        else if (command == "set_text") {
            clearCommandLine(promptRow);
            printPromptLine(promptRow, "Enter marquee text: ");

            std::string text;
            std::getline(std::cin, text);
            marquee.setText(text);

            renderPrompt();
        }
        else if (command == "set_speed") {
            clearCommandLine(promptRow);
            printPromptLine(promptRow, "Enter speed in ms: ");

            int speed = 0;
            std::cin >> speed;

            if (std::cin.fail() || speed <= 0) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');

                clearCommandLine(promptRow);
                printPromptLine(promptRow, "Invalid speed.");
                Sleep(800);
            }
            else {
                std::cin.ignore(1000, '\n');
                marquee.setSpeed(speed);
            }

            renderPrompt();
        }
        else if (command == "clear") {
            const bool marqueeWasRunning = marquee.isRunning();
            if (marqueeWasRunning) {
                marquee.stop();
            }

            showHelpMenu = false;
            redrawScreen();

            if (marqueeWasRunning) {
                marquee.start();
            }
        }
        else if (command == "initialize" || command == "screen" ||
            command == "scheduler-start" || command == "scheduler-stop" ||
            command == "report-util") {
            clearCommandLine(promptRow);
            printPromptLine(promptRow, command + " recognized.");
            Sleep(800);
            renderPrompt();
        }
        else {
            clearCommandLine(promptRow);
            printPromptLine(promptRow, "Unknown command.");
            Sleep(800);
            renderPrompt();
        }
    }

    if (marquee.isRunning()) {
        marquee.stop();
    }
    return 0;
}