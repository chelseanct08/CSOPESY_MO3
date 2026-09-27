#include <iostream>
#include <string>
#include <windows.h>
#include "Marquee.h"

void setCursorPosition(int x, int y) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD position;
    position.X = static_cast<SHORT>(x);
    position.Y = static_cast<SHORT>(y);

    SetConsoleCursorPosition(console, position);
}

void clearCommandLine(int commandAreaTop, int windowWidth) {
    const int menuWidth = 40;
    int menuLeft = (windowWidth - menuWidth) / 2;

    setCursorPosition(menuLeft, commandAreaTop + 14);

    std::cout << std::string(menuWidth, ' ');

    setCursorPosition(menuLeft, commandAreaTop + 14);
}

void displayWelcome(int commandAreaTop, int windowWidth) {
    const int menuWidth = 40;
    int menuLeft = (windowWidth - menuWidth) / 2;

    setCursorPosition(menuLeft, commandAreaTop);

    std::cout << "========================================\n";

    setCursorPosition(menuLeft, commandAreaTop + 1);
    std::cout << "              OS EMULATOR\n";

    setCursorPosition(menuLeft, commandAreaTop + 2);
    std::cout << "========================================\n";

    setCursorPosition(menuLeft, commandAreaTop + 4);
    std::cout << " Type 'help' to display available commands.";

    setCursorPosition(menuLeft, commandAreaTop + 6);
    std::cout << "----------------------------------------";

    setCursorPosition(menuLeft, commandAreaTop + 7);
    std::cout << " Enter command: ";
}

void displayMenu(int commandAreaTop, int windowWidth) {
    const int menuWidth = 40;
    int menuLeft = (windowWidth - menuWidth) / 2;

    setCursorPosition(menuLeft, commandAreaTop);

    std::cout << "========================================\n";

    setCursorPosition(menuLeft, commandAreaTop + 1);
    std::cout << "              OS EMULATOR\n";

    setCursorPosition(menuLeft, commandAreaTop + 2);
    std::cout << "========================================\n";

    setCursorPosition(menuLeft, commandAreaTop + 4);
    std::cout << "                 COMMANDS\n";

    setCursorPosition(menuLeft, commandAreaTop + 5);
    std::cout << "----------------------------------------\n";

    setCursorPosition(menuLeft, commandAreaTop + 6);
    std::cout << " help          - Display commands\n";

    setCursorPosition(menuLeft, commandAreaTop + 7);
    std::cout << " start_marquee - Start marquee animation\n";

    setCursorPosition(menuLeft, commandAreaTop + 8);
    std::cout << " stop_marquee  - Stop marquee animation\n";

    setCursorPosition(menuLeft, commandAreaTop + 9);
    std::cout << " set_text      - Set marquee text\n";

    setCursorPosition(menuLeft, commandAreaTop + 10);
    std::cout << " set_speed     - Set marquee speed in ms\n";

    setCursorPosition(menuLeft, commandAreaTop + 11);
    std::cout << " exit          - Exit the console\n";

    setCursorPosition(menuLeft, commandAreaTop + 13);
    std::cout << "----------------------------------------";

    setCursorPosition(menuLeft, commandAreaTop + 14);
    std::cout << " Enter command: ";
}

int main() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(console, &consoleInfo);

    int windowWidth =
        consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;

    int windowHeight =
        consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;

    const int commandAreaHeight = 19;

    int commandAreaTop = windowHeight - commandAreaHeight;

    system("cls");

    Marquee marquee;
    std::string command;

    displayWelcome(commandAreaTop, windowWidth);

    while (true) {
        std::getline(std::cin, command);

        if (command == "help") {
            system("cls");

            displayMenu(commandAreaTop, windowWidth);
        }
        else if (command == "start_marquee") {
            marquee.start();

            clearCommandLine(commandAreaTop, windowWidth);

            std::cout << " Enter command: ";
        }
        else if (command == "stop_marquee") {
            marquee.stop();

            clearCommandLine(commandAreaTop, windowWidth);

            std::cout << " Enter command: ";
        }
        else if (command == "set_text") {
            std::string text;

            clearCommandLine(commandAreaTop, windowWidth);

            std::cout << " Enter marquee text: ";

            std::getline(std::cin, text);

            marquee.setText(text);

            clearCommandLine(commandAreaTop, windowWidth);

            std::cout << " Enter command: ";
        }
        else if (command == "set_speed") {
            int speed;

            clearCommandLine(commandAreaTop, windowWidth);

            std::cout << " Enter speed in milliseconds: ";

            std::cin >> speed;
            std::cin.ignore(1000, '\n');

            marquee.setSpeed(speed);

            clearCommandLine(commandAreaTop, windowWidth);

            std::cout << " Enter command: ";
        }
        else if (command == "exit") {
            marquee.stop();
            break;
        }
        else {
            clearCommandLine(commandAreaTop, windowWidth);

            std::cout << " Unknown command.";

            setCursorPosition(
                (windowWidth - 40) / 2,
                commandAreaTop + 15
            );

            std::cout << " Enter command: ";

            setCursorPosition(
                (windowWidth - 40) / 2,
                commandAreaTop + 14
            );
        }
    }

    return 0;
}