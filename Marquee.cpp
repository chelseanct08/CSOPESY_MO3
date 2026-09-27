#include "Marquee.h"
#include <iostream>
#include <windows.h>
#include <chrono>

Marquee::Marquee()
    : text("Hello World"), speed(100), running(false) {
}

Marquee::~Marquee() {
    stop();
}

void Marquee::start() {
    if (running) {
        return;
    }

    running = true;
    marqueeThread = std::thread(&Marquee::run, this);
}

void Marquee::stop() {
    if (!running) {
        return;
    }

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
    return running;
}

void Marquee::run() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

    if (!GetConsoleScreenBufferInfo(console, &consoleInfo)) {
        running = false;
        return;
    }

    int windowWidth =
        consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;

    const int marqueeTop = 1;
    const int marqueeHeight = 8;
    const int marqueeBottom = marqueeTop + marqueeHeight - 1;

    int x = 0;
    int y = 1;

    int dx = 1;
    int dy = 1;

    std::string previousText;
    int previousX = 0;
    int previousY = 1;

    while (running) {
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

        int textWidth = static_cast<int>(currentText.length());

        if (textWidth >= windowWidth) {
            textWidth = windowWidth - 1;
            currentText = currentText.substr(0, textWidth);
        }

        int maxX = windowWidth - textWidth;

        if (maxX < 0) {
            maxX = 0;
        }

        if (x >= maxX) {
            x = maxX;
            dx = -1;
        }

        if (x <= 0) {
            x = 0;
            dx = 1;
        }

        if (y >= marqueeBottom) {
            y = marqueeBottom;
            dy = -1;
        }

        if (y <= marqueeTop) {
            y = marqueeTop;
            dy = 1;
        }

        // Erase the previous text
        if (!previousText.empty()) {
            COORD oldPosition;
            oldPosition.X = static_cast<SHORT>(previousX);
            oldPosition.Y = static_cast<SHORT>(previousY);

            DWORD charsWritten = 0;

            WriteConsoleOutputCharacterA(
                console,
                std::string(previousText.length(), ' ').c_str(),
                static_cast<DWORD>(previousText.length()),
                oldPosition,
                &charsWritten
            );
        }

        // Draw the current text
        COORD position;
        position.X = static_cast<SHORT>(x);
        position.Y = static_cast<SHORT>(y);

        DWORD charsWritten = 0;

        WriteConsoleOutputCharacterA(
            console,
            currentText.c_str(),
            static_cast<DWORD>(currentText.length()),
            position,
            &charsWritten
        );

        previousText = currentText;
        previousX = x;
        previousY = y;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(speed)
        );

        x += dx;
        y += dy;
    }

    // Clear the last marquee text
    if (!previousText.empty()) {
        COORD position;
        position.X = static_cast<SHORT>(previousX);
        position.Y = static_cast<SHORT>(previousY);

        DWORD charsWritten = 0;

        WriteConsoleOutputCharacterA(
            console,
            std::string(previousText.length(), ' ').c_str(),
            static_cast<DWORD>(previousText.length()),
            position,
            &charsWritten
        );
    }
}