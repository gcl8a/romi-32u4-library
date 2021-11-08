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

void leftISR()
{
    bool newLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool newLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ newLeftB;

    leftMotor.handleISR(newLeftA, newLeftB);
}

void rightISR()
{
    bool newRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    bool newRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ newRightB;

    rightMotor.handleISR(newRightA, newRightB);
}

void Romi32U4Motor::handleISR(bool newA, bool newB)
{
    count += (lastA ^ newB) - (newA ^ lastB);

    // if((lastLeftA ^ newLeftA) & (lastLeftB ^ newLeftB))
    // {
    //     errorLeft = true;
    // }

    lastA = newA;
    lastB = newB;

    if(ctrlMode == CTRL_POS)
    {
        if(count == targetPos)
        {
            setEffort(0);
            ctrlMode = CTRL_DIRECT;
        }
    }

}

int16_t Romi32U4Motor::getCount()
{
    cli();
    int16_t tempCount = count;
    sei();
    return tempCount;
}

int16_t Romi32U4Motor::resetCount(void)
{
    cli();
    int16_t tempCount = count;
    count = 0;
    sei();
    return tempCount;
}

void Romi32U4Motor::initEncoders(void)
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
    bool lastLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool lastLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ lastLeftB;

    leftMotor.handleISR(lastLeftA, lastLeftB);
    leftMotor.resetCount();

    bool lastRightB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool lastRightA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ lastRightB;

    rightMotor.handleISR(lastRightA, lastRightB);
    rightMotor.resetCount();

    Serial.println("/initEncoders()");
}

void Romi32U4Motor::updateSpeed(void) 
{
    cli();
    int16_t currCount = count;
    sei();
    speed = currCount - prevCount;
    prevCount = currCount;

    Serial.print(currCount);
    Serial.print('\t');
    Serial.print(targetPos);
    Serial.print('\t');
}

/*
 * ISR for timing. On overflow, it takes a 'snapshot' of the encoder counts and raises a flag to let
 * the main program it is time to execute the PID calculations.
 */
ISR(TIMER4_OVF_vect)
{
    leftMotor.updateSpeed();
    rightMotor.updateSpeed();
    Romi32U4Motor::readyToPID++;
}
