#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
<<<<<<< Updated upstream
=======
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif
>>>>>>> Stashed changes
#include <windows.h>

inline constexpr int marqueeBorderColumn = 45;
inline constexpr int marqueeAreaLeft = marqueeBorderColumn + 2;

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

    int x = marqueeAreaLeft;
    int y = 1;
    int dx = 1;
    int dy = 1;
<<<<<<< Updated upstream
    std::string previousText;
    int previousX = 2;
=======
    std::vector<std::string> previousBackground;
    int previousX = marqueeAreaLeft;
>>>>>>> Stashed changes
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