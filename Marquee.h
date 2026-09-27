#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>

class Marquee {
private:
    std::string text;
    int speed;

    std::atomic<bool> running;
    std::thread marqueeThread;
    std::mutex textMutex;

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