#pragma once

#include <Arduino.h>

class Rangefinder 
{
protected:
    volatile uint8_t state = 0;

    uint8_t echoPin = -1;
    uint8_t trigPin = -1;

    // for keeping track of ping intervals
    uint32_t lastPing = 0;          
    uint32_t pingInterval = 10;    

    // for keeping track of echo duration
    volatile uint32_t pulseStart = 0;
    volatile uint32_t pulseEnd = 0;

    // holds the last recorded distance
    float distance = 0;

public:
    Rangefinder(uint8_t echo, uint8_t trig);

    // must call init() to set up pins and interrupts
    void init(void);

    // checks to see if it's time to emit a ping
    uint8_t checkPingTimer(void);

    // checks to see if an echo is complete
    uint16_t checkEcho(void);

    // returns the last recorded distance
    float getDistance(void);

    // ISR for the echo pin
    void ISR_echo(void);
};

// ISR for the echo
void ISR_Rangefinder(void);

// we declare as extern so we can refer to it in the ISR
extern Rangefinder rangefinder;
