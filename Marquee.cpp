#include "Marquee.h"
#include <algorithm>
#include <chrono>

namespace {
const char* defaultMarquee =
    "   _____  _____  ____  _____  ______  _______     __\n"
    "  / ____|/ ____|/ __ \\|  __ \\|  ____|/ ____\\ \\   / /\n"
    " | |    | (___ | |  | | |__) | |__  | (___  \\ \\_/ /\n"
    " | |     \\___ \\| |  | |  ___/|  __|  \\___ \\  \\   /\n"
    " | |____ ____) | |__| | |    | |____ ____) |  | |\n"
    "  \\_____|_____/ \\____/|_|    |______|_____/   |_|";
}

// default text, speed, and running state
Marquee::Marquee()
    : text(defaultMarquee), speed(125), running(false) {
}

Marquee::~Marquee() {
    stop();
}

void Marquee::start() {
    if (running) {
        return;
    }

    if (marqueeThread.joinable()) {
        marqueeThread.join();
    }

    running = true;
    marqueeThread = std::thread(&Marquee::run, this);
}

void Marquee::stop() {
    running = false;

    if (marqueeThread.joinable()) {
        marqueeThread.join();
    }
}

void Marquee::setText(const std::string& newText) {
    std::lock_guard<std::mutex> lock(textMutex);
    text = newText;
}

void Marquee::setSpeed(int newSpeed) {
    if (newSpeed > 0) {
        speed = newSpeed;
    }
}

bool Marquee::isRunning() const {
    return running.load();
}

void Marquee::run() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    while (running) {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

        if (!GetConsoleScreenBufferInfo(console, &consoleInfo)) {
            running = false;
            break;
        }

        int windowWidth =
            consoleInfo.srWindow.Right -
            consoleInfo.srWindow.Left + 1;

        int windowHeight =
            consoleInfo.srWindow.Bottom -
            consoleInfo.srWindow.Top + 1;

        // gets the current marquee text
        std::string currentText;
        {
            std::lock_guard<std::mutex> lock(textMutex);
            currentText = text;
        }

        if (currentText.empty()) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(speed)
            );
            continue;
        }

        // splits the text into separate lines
        std::vector<std::string> lines;
        size_t lineStart = 0;

        while (lineStart <= currentText.length()) {
            size_t lineEnd = currentText.find('\n', lineStart);

            if (lineEnd == std::string::npos) {
                lineEnd = currentText.length();
            }

            lines.push_back(
                currentText.substr(lineStart, lineEnd - lineStart)
            );

            if (lineEnd == currentText.length()) {
                break;
            }

            lineStart = lineEnd + 1;
        }

        // gets the width of the marquee text
        int textWidth = 0;
        for (const std::string& line : lines) {
            textWidth = std::max(
                textWidth,
                static_cast<int>(line.length())
            );
        }

        const int availableWidth =
            std::max(1, windowWidth - marqueeAreaLeft - 1);

        for (std::string& line : lines) {
            if (static_cast<int>(line.length()) > availableWidth) {
                line.resize(availableWidth);
            }
        }

        textWidth = std::min(textWidth, availableWidth);

        // sets the movement boundaries
        int minX = std::min(
            marqueeAreaLeft,
            std::max(0, windowWidth - 1)
        );

        int maxX = std::max(
            minX,
            windowWidth - textWidth - 1
        );

        int minY = 0;
        int maxY = std::max(
            minY,
            windowHeight - static_cast<int>(lines.size())
        );

        x = std::clamp(x, minX, maxX);
        y = std::clamp(y, minY, maxY);

        // reverses direction when the marquee reaches a boundary
        if (x >= maxX) {
            x = maxX;
            dx = -1;
        }
        else if (x <= minX) {
            x = minX;
            dx = 1;
        }

        if (y >= maxY) {
            y = maxY;
            dy = -1;
        }
        else if (y <= minY) {
            y = minY;
            dy = 1;
        }

        // restores the background from the previous position
        for (size_t index = 0;
             index < previousBackground.size();
             ++index) {
            if (previousBackground[index].empty()) {
                continue;
            }

            COORD oldPosition = {
                static_cast<SHORT>(previousX),
                static_cast<SHORT>(previousY + static_cast<int>(index))
            };

            DWORD charsWritten = 0;
            WriteConsoleOutputCharacterA(
                console,
                previousBackground[index].c_str(),
                static_cast<DWORD>(previousBackground[index].length()),
                oldPosition,
                &charsWritten
            );
        }

        // saves the background and draws the marquee
        std::vector<std::string> currentBackground(lines.size());

        for (size_t index = 0; index < lines.size(); ++index) {
            COORD position = {
                static_cast<SHORT>(x),
                static_cast<SHORT>(y + static_cast<int>(index))
            };

            if (!lines[index].empty()) {
                currentBackground[index].resize(lines[index].length(), ' ');

                DWORD charsRead = 0;
                ReadConsoleOutputCharacterA(
                    console,
                    &currentBackground[index][0],
                    static_cast<DWORD>(currentBackground[index].length()),
                    position,
                    &charsRead
                );
            }

            DWORD charsWritten = 0;
            WriteConsoleOutputCharacterA(
                console,
                lines[index].c_str(),
                static_cast<DWORD>(lines[index].length()),
                position,
                &charsWritten
            );
        }

        previousBackground = currentBackground;
        previousX = x;
        previousY = y;

        // controls how often the marquee updates
        std::this_thread::sleep_for(
            std::chrono::milliseconds(speed)
        );

        x += dx;
        y += dy;
    }

    // restores the last background when the animation stops
    for (size_t index = 0;
         index < previousBackground.size();
         ++index) {
        if (previousBackground[index].empty()) {
            continue;
        }

        COORD position = {
            static_cast<SHORT>(previousX),
            static_cast<SHORT>(previousY + static_cast<int>(index))
        };

        DWORD charsWritten = 0;
        WriteConsoleOutputCharacterA(
            console,
            previousBackground[index].c_str(),
            static_cast<DWORD>(previousBackground[index].length()),
            position,
            &charsWritten
        );
    }

    previousBackground.clear();
}
