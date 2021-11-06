// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <Arduino.h>
#include <Romi32U4Motors.h>
#include <FastGPIO.h>
#include <avr/interrupt.h>

#include <pcint.h>

#include <Chassis.h>

#define LEFT_XOR   8
#define LEFT_B     IO_E2
#define RIGHT_XOR  7
#define RIGHT_B    23


void Romi32U4Motors::leftISR()
{
    bool newLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool newLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ newLeftB;

    countLeft += (lastLeftA ^ newLeftB) - (newLeftA ^ lastLeftB);

    // if((lastLeftA ^ newLeftA) & (lastLeftB ^ newLeftB))
    // {
    //     errorLeft = true;
    // }

    lastLeftA = newLeftA;
    lastLeftB = newLeftB;
}

void Romi32U4Motors::rightISR()
{
    bool newRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    bool newRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ newRightB;

    countRight += (lastRightA ^ newRightB) - (newRightA ^ lastRightB);

    // if((lastRightA ^ newRightA) & (lastRightB ^ newRightB))
    // {
    //     errorRight = true;
    // }

    lastRightA = newRightA;
    lastRightB = newRightB;
}


int16_t Romi32U4Motors::getCountsLeft()
{
    init();

    cli();
    int16_t counts = countLeft;
    sei();
    return counts;
}

int16_t Romi32U4Motors::getCountsRight()
{
    init();

    cli();
    int16_t counts = countRight;
    sei();
    return counts;
}

int16_t Romi32U4Motors::getCountsAndResetLeft()
{
    init();

    cli();
    int16_t counts = countLeft;
    countLeft = 0;
    sei();
    return counts;
}

int16_t Romi32U4Motors::getCountsAndResetRight()
{
    init();

    cli();
    int16_t counts = countRight;
    countRight = 0;
    sei();
    return counts;
}

// bool Romi32U4Motors::checkErrorLeft()
// {
//     init();

//     bool error = errorLeft;
//     errorLeft = 0;
//     return error;
// }

// bool Romi32U4Motors::checkErrorRight()
// {
//     init();

//     bool error = errorRight;
//     errorRight = 0;
//     return error;
// }

void leftISR(void) {motors.leftISR();}
void rightISR(void) {motors.rightISR();}

void Romi32U4Motors::initEncoders(void)
{    
    Serial.println("initEncoders()");
    // Set the pins as pulled-up inputs.
    FastGPIO::Pin<LEFT_XOR>::setInputPulledUp();
    FastGPIO::Pin<LEFT_B>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_XOR>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_B>::setInputPulledUp();

    //attach a PC interrupt
    attachPCInt(PCINT4, ::leftISR);

    // Enable interrupt on PE6 for the right encoder.  We use attachInterrupt
    // instead of defining ISR(INT6_vect) ourselves so that this class will be
    // compatible with other code that uses attachInterrupt.
    attachInterrupt(4, ::rightISR, CHANGE);

    // Initialize the variables.  It's good to do this after enabling the
    // interrupts in case the interrupts fired by accident as we were enabling
    // them.
    lastLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    lastLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ lastLeftB;
    countLeft = 0;
    //errorLeft = 0;

    lastRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    lastRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ lastRightB;
    countRight = 0;
    //errorRight = 0;

    Serial.println("/initEncoders()");
}

void Romi32U4Motors::motorISR(void) 
{
    speedLeft = countLeft - prevCountLeft;
    prevCountLeft = countLeft;

    speedRight = countRight - prevCountRight;
    prevCountRight = countRight;

    readyToPID = 1;
}
/*
 * ISR for timing. On overflow, it takes a 'snapshot' of the encoder counts and raises a flag to let
 * the main program it is time to execute the PID calculations.
 */
ISR(TIMER4_OVF_vect)
{
    motors.motorISR();
}
