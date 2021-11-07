#pragma once

#include <Arduino.h>

#define ECHO_RECD   0x02

class Rangefinder 
{
private:
    volatile uint8_t state = 0;

    uint8_t echoPin = -1;
    uint8_t trigPin = -1;

    uint32_t lastPing = 0;          // for keeping track of intervals
    uint32_t pingInterval = 10;    

    volatile uint32_t pulseStart = 0;
    volatile uint32_t pulseEnd = 0;

    float distance = 0;

public:
    Rangefinder(uint8_t echo, uint8_t trig);

    // void init(void);
    void init(void);

    //checks to see if it's time for a ping
    uint8_t checkPingTimer(void);

    //Checks to see if a pulse echo has been registered
    uint16_t checkEcho(void);

    float getDistance(void);

    //ISR for the echo pins
    void ISR_echo(void);
};

extern Rangefinder rangefinder;