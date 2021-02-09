#pragma once

class Timer
{
public:
    Timer(long interval);
    bool isExpired();
    void reset();
    void reset(long newInterval);

private:
    long expiredTime;
    long timeInterval;
};
