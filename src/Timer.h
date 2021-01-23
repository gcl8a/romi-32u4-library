#pragma once

class Timer
{
public:
    Timer(long interval);
    bool isExpired();
    void reset();

private:
    long expiredTime;
    long timeInterval;
};
