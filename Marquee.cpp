#include "Marquee.h"
#include <chrono>

Marquee::Marquee()
    : text("Hello World"), speed(100), running(false) {
    x = 2;
    y = 1;
    dx = 1;
    dy = 1;
    previousText.clear();
    previousX = 2;
    previousY = 1;
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
    return running.load();
}

void Marquee::run() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    const int marqueeTop = 1;
    const int menuRight = 43;

    while (running) {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

        if (!GetConsoleScreenBufferInfo(
                console,
                &consoleInfo)) {

            running = false;
            break;
        }

        int windowWidth =
            consoleInfo.srWindow.Right -
            consoleInfo.srWindow.Left + 1;

        int windowHeight =
            consoleInfo.srWindow.Bottom -
            consoleInfo.srWindow.Top + 1;

        int marqueeBottom = windowHeight - 1;

        const int safeRight = windowWidth - 1;
        if (x < menuRight) {
            x = menuRight;
            dx = 1;
        }
        if (x > safeRight) {
            x = safeRight;
            dx = -1;
        }

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

        int textWidth =
            static_cast<int>(currentText.length());

        /*
            If the text is wider than the console,
            shorten it so it remains visible.
        */
        if (textWidth >= windowWidth) {
            textWidth = windowWidth - 1;

            if (textWidth > 0) {
                currentText =
                    currentText.substr(0, textWidth);
            }
        }

        int minX = menuRight;
        int maxX = windowWidth - textWidth - 2;

        int minY = marqueeTop;
        int maxY = marqueeBottom;

        if (maxX < minX) {
            maxX = minX;
        }

        if (x > maxX) {
            x = maxX;
        }

        if (x < minX) {
            x = minX;
        }

        if (y > maxY) {
            y = maxY;
        }

        if (y < minY) {
            y = minY;
        }

        if (x >= maxX) {
            x = maxX;
            dx = -1;
        }

        if (x <= minX) {
            x = minX;
            dx = 1;
        }

        if (y >= maxY) {
            y = maxY;
            dy = -1;
        }

        if (y <= minY) {
            y = minY;
            dy = 1;
        }

        if (!previousText.empty()) {
            COORD oldPosition;

            oldPosition.X =
                static_cast<SHORT>(previousX);

            oldPosition.Y =
                static_cast<SHORT>(previousY);

            DWORD charsWritten = 0;

            WriteConsoleOutputCharacterA(
                console,
                std::string(
                    previousText.length(),
                    ' '
                ).c_str(),
                static_cast<DWORD>(
                    previousText.length()
                ),
                oldPosition,
                &charsWritten
            );
        }

        COORD position;

        position.X = static_cast<SHORT>(x);
        position.Y = static_cast<SHORT>(y);

        DWORD charsWritten = 0;

        WriteConsoleOutputCharacterA(
            console,
            currentText.c_str(),
            static_cast<DWORD>(
                currentText.length()
            ),
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

    if (!previousText.empty()) {
        COORD position;

        position.X =
            static_cast<SHORT>(previousX);

        position.Y =
            static_cast<SHORT>(previousY);

        DWORD charsWritten = 0;

        WriteConsoleOutputCharacterA(
            console,
            std::string(
                previousText.length(),
                ' '
            ).c_str(),
            static_cast<DWORD>(
                previousText.length()
            ),
            position,
            &charsWritten
        );
    }
}