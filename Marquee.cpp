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
    GetConsoleScreenBufferInfo(console, &consoleInfo);

    int windowWidth =
        consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;

    int windowHeight =
        consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;

    // Smaller area for the marquee
    const int marqueeHeight = 8;

    int marqueeTop = 1;
    int marqueeBottom = marqueeTop + marqueeHeight - 1;

    int x = 0;
    int y = 1;

    int dx = 1;
    int dy = 1;

    while (running) {
        std::string currentText;

        {
            std::lock_guard<std::mutex> lock(textMutex);
            currentText = text;
        }

        int textWidth = static_cast<int>(currentText.length());

        if (textWidth >= windowWidth) {
            textWidth = windowWidth - 1;
        }

        int maxX = windowWidth - textWidth - 1;

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

        COORD position;
        position.X = static_cast<SHORT>(x);
        position.Y = static_cast<SHORT>(y);

        SetConsoleCursorPosition(console, position);

        std::cout << currentText << std::flush;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(speed)
        );

        SetConsoleCursorPosition(console, position);

        std::cout << std::string(textWidth, ' ') << std::flush;

        x += dx;
        y += dy;
    }
}