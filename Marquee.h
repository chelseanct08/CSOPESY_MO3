#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

#define NOMINMAX
#include <windows.h>

inline void setCursorPosition(int x, int y) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD position;
    position.X = static_cast<SHORT>(x);
    position.Y = static_cast<SHORT>(y);

    SetConsoleCursorPosition(console, position);
}

class Marquee {
private:
    std::string text;
    std::atomic<int> speed;

    std::atomic<bool> running;
    std::thread marqueeThread;
    std::mutex textMutex;

    int x = 2;
    int y = 1;
    int dx = 1;
    int dy = 1;
    std::vector<std::string> previousLines;
    std::vector<std::string> previousBackground;
    int previousX = 2;
    int previousY = 1;

    void run();

public:
    Marquee();
    ~Marquee();

    void start();
    void stop();

    void setText(const std::string& newText);
    void setSpeed(int newSpeed);
    bool isRunning() const;
};