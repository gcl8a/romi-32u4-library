/*
 * A minimal class to control a servo on pin 6.
 * 
 * It uses output compare on Timer4 to control the pulse. The 10-bit Timer4 is set up in 
 * Chassis::init() with a pre-scaler of 1024, TOP of 249 + 1 => 16 ms
 * 
 * OCR4D controls the pulse on pin 6 -- THE SERVO MUST BE ON PIN 6! 
 * 
 * Defaults to a range of 1000 - 2000 us, but can be customized.
 */

#pragma once

#include <Arduino.h>

class Servo32U4
{
private:
    uint16_t usMin = 1000;
    uint16_t usMax = 2000;

    uint8_t feedbackPin = -1;
    static bool isAttached;

public:
    static void init(void);
    static void attach(void); //MUST USE PIN 6
    void detach(void);
    void write(uint16_t microseconds);
    uint16_t setMinMaxUS(uint16_t min, uint16_t max);
};
