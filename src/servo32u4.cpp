#include <servo32u4.h>

void Servo32U4::attach(void) //MUST USE PIN 5!!
{
    pinMode(5, OUTPUT); // set pin as OUTPUT

    cli();

    // clear then set the OCR4D bits (pin 6)
    TCCR3A = 0x82; //WGM
    TCCR3B = 0x1A; //WGM + CS = 8
    ICR3 = 39999; //20ms
    OCR3A = 3000; //default to 1500us

    sei();

    isAttached = true;
}

void Servo32U4::detach(void)
{
    cli();

    // clear the OCR4D bits
    TCCR3A &= 0xfd; //cancel OCR4D
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
