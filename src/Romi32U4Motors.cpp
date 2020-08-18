// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <Romi32U4Motors.h>
#include <FastGPIO.h>
#include <avr/io.h>

#define PWM_L 10
#define PWM_R 9
#define DIR_L 16
#define DIR_R 15

bool Romi32U4Motors::flipLeft = false;
bool Romi32U4Motors::flipRight = false;

int16_t Romi32U4Motors::maxEffort = 300;

// initialize timer1 to generate the proper PWM outputs to the motor drivers
void Romi32U4Motors::init2()
{
    FastGPIO::Pin<PWM_L>::setOutputLow();
    FastGPIO::Pin<PWM_R>::setOutputLow();
    FastGPIO::Pin<DIR_L>::setOutputLow();
    FastGPIO::Pin<DIR_R>::setOutputLow();

    // Timer 1 configuration
    // prescaler: clockI/O / 1
    // outputs enabled
    // phase-correct PWM
    // top of 400
    //
    // PWM frequency calculation
    // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
    TCCR1A = 0b10101000; //gcl: added OCR1C for adding a third motor
    TCCR1B = 0b00010001;
    ICR1 = 400;
    OCR1A = 0;
    OCR1B = 0;

    /*
    * The following lines will enable a compare match interrupt on Timer0. Timer0 is used by Arduino
    * for managing millis(), so it runs at ~1ms (actually 1024 us...long story). We don't want
    * to break anything Arduino, so we set up OCR0A to raise an interrupt. In order to stagger
    * the millis() machinery with our ISR, we set OCR0A to 128 -- any value other than 0 should
    * work.
    */
    TIMSK0 |= (1 << OCIE0A);    //enable OCR0 interrupt
    OCR0A = 128;                //stagger interrupt w.r.t. millis machinery
}

void Romi32U4Motors::flipLeftMotor(bool flip)
{
    flipLeft = flip;
}

void Romi32U4Motors::flipRightMotor(bool flip)
{
    flipRight = flip;
}

void Romi32U4Motors::setLeftEffort(int16_t effort)
{
    init();

    bool reverse = 0;

    if (effort < 0)
    {
        effort = -effort; // Make speed a positive quantity.
        reverse = 1;    // Preserve the direction.
    }
    if (effort > maxEffort)
    {
        effort = maxEffort;
    }

    OCR1B = effort;

    FastGPIO::Pin<DIR_L>::setOutput(reverse ^ flipLeft);
}

void Romi32U4Motors::setRightEffort(int16_t effort)
{
    init();

    bool reverse = 0;

    if (effort < 0)
    {
        effort = -effort;  // Make speed a positive quantity.
        reverse = 1;     // Preserve the direction.
    }
    if (effort > maxEffort)
    {
        effort = maxEffort;
    }

    OCR1A = effort;

    FastGPIO::Pin<DIR_R>::setOutput(reverse ^ flipRight);
}

void Romi32U4Motors::setEfforts(int16_t leftEffort, int16_t rightEffort)
{
    setLeftEffort(leftEffort);
    setRightEffort(rightEffort);
}

void Romi32U4Motors::allowTurbo(bool turbo)
{
    maxEffort = turbo ? 400 : 300;
}
