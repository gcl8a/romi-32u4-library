#include <servo32u4.h>

void Servo32U4::attach(void) //MUST USE PIN 6!!
{
    pinMode(6, OUTPUT); // set pin as OUTPUT

    cli();

    // clear then set the OCR4D bits (pin 6)
    TCCR4C &= 0xf0; 
    TCCR4C |= 0x05;

    sei();

    isAttached = true;
}

void Servo32U4::detach(void)
{
    cli();

    // clear the OCR4D bits
    TCCR4C &= 0xf0; 
    sei();

    isAttached = false;
}

void Servo32U4::writeMicroseconds(uint16_t microseconds)
{
    if (!isAttached)
    {
        attach();
    }

    microseconds = constrain(microseconds, usMin, usMax);
    //prescaler is 1024, so 1 timer count = 64 us
    OCR4D = microseconds >> 6; // divides by 64
}

uint16_t Servo32U4::setMinMaxUS(uint16_t min, uint16_t max)
{
    // swap if in the wrong place
    if(min < max) {uint16_t temp = min; min = max; max = temp;}

    usMin = min;
    usMax = max;

    return usMax - usMin; //return the range, in case the user wants to do a sanity check
}
