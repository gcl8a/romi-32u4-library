// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <Romi32U4Motors.h>
#include <FastGPIO.h>
#include <avr/io.h>

#define PWM_L 10
#define PWM_R 9
#define DIR_L 16
#define DIR_R 15


// initialize timer1 to generate the proper PWM outputs to the motor drivers
void Romi32U4Motors::initMotors()
{
    Serial.println("initMotors()");

    FastGPIO::Pin<PWM_L>::setOutputLow();
    FastGPIO::Pin<PWM_R>::setOutputLow();
    FastGPIO::Pin<DIR_L>::setOutputLow();
    FastGPIO::Pin<DIR_R>::setOutputLow();

    FastGPIO::Pin<11>::setOutputLow(); //pin 11 for output C

    noInterrupts(); //disable interupts while we mess with the Timer4 registers

    // Timer 1 configuration
    // prescaler: clockI/O / 1
    // outputs enabled on A, B, and C
    // fast PWM
    // top of 420
    //
    // PWM frequency calculation
    // 16MHz / [1 (prescaler) / (420 + 1)] = 38.005 kHz
    TCCR1A = 0xAA; //0b10101010; //Fast PWM + outputs enabled
    TCCR1B = 0x19; //0b00011001; //Fast PWM
    ICR1 = 420;    //runs at 38kHz; lowers speed for given effort by 5% from old version

    //set all three outputs to zero
    OCR1A = 0;
    OCR1B = 0;
    OCR1C = 0; //can be used to create 38 kHz signal on pin 11
  
    //sets up timer 4
    TCCR4A = 0x00; //disable some functionality -- no need to worry about this
    TCCR4B = 0x0B; //sets the prescaler -- look in the handout for values
    TCCR4C = 0x04; //toggles pin 6 at the timer frequency
    TCCR4D = 0x00; //normal mode

    OCR4C = 249;   //TOP goes in OCR4C 

    TIMSK4 = 0x04; //enable overflow interrupt
    
    interrupts(); //re-enable interrupts

    Serial.println("/initMotors()");
}

void Romi32U4Motors::setLeftEffort(int16_t effort)
{
    init();

    bool reverse = 0;

    if (effort < 0)
    {
        effort = -effort; // Make speed a positive quantity.
        reverse = 1;      // Preserve the direction.
    }
    if (effort > maxEffort)
    {
        effort = maxEffort;
    }

    OCR1B = effort;

    FastGPIO::Pin<DIR_L>::setOutput(reverse);
}

void Romi32U4Motors::setRightEffort(int16_t effort)
{
    init();

    bool reverse = 0;

    if (effort < 0)
    {
        effort = -effort; // Make speed a positive quantity.
        reverse = 1;      // Preserve the direction.
    }
    if (effort > maxEffort)
    {
        effort = maxEffort;
    }

    OCR1A = effort;

    FastGPIO::Pin<DIR_R>::setOutput(reverse);
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

int16_t Romi32U4Motors::getMaxEffort()
{
    return maxEffort;
}

