/*
 * A minimal class to control a servo on pin 6.
 * 
 * It uses output compare on Timer4 to control the pulse. The 10-bit Timer4 is set up (in Init()) 
 * with a pre-scaler of 256, so 16 us resolution for the servo position (which is pretty fine, actually).
 * 
 * Upon request, I could figure out how to have 11-bit accuracy, or 8 us.
 * 
 * OCR4D controls the pulse on pin 6 -- THE SERVO MUST BE ON PIN 6! 
 * 
 * OCR4D takes a 10-bit value, but two MSBs are written using a common register (TC4H). 
 * But since a value of 256 corresponds to 4096 us, the high bits will always be 00.
 * 
 * Defaults to a range of 1000 - 2000 us, but can be customized.
 */ 

#include <Arduino.h>
#include <servo32u4.h>

void Servo32U4::Init(void)
{
    cli();
    //set up timer 4
    //Timer4 is set for 16ms rollover (which can also be used for PID timing)
    
    TCCR4A = 0x00; //disable A/B compares
    TCCR4B = 0x09; //sets the prescaler -- see datasheet for values
    //TCCR4C = 0x05; //moved to Attach: enable PWM; clear OC4D on compare match (pin 6 for the servo)
    TCCR4D = 0x00; //fast PWM mode

    //16ms rollover
    uint16_t ocr = 999; //TOP = 999; rollover in 1000

    //for 10-bit resolution, put the two high bytes in TC4H and the low in OCR4C
    TC4H = (ocr >> 8) & 0x03;
    OCR4C = ocr;  

    sei();

    pinMode(6, OUTPUT); //don't forget to enable the pin as OUTPUT

    //this can be used for timing the PID loop -- but it belongs in the motor class
    //TIMSK4 = 0x04;
    //sbi(TIMSK4, TOIE4); //enable OVF interrupt
}

void Servo32U4::Attach(void) //MUST USE PIN 6
{
    cli();

    //check to make sure Timer4 is set up correctly
    //TODO: assert() ??? I can't seem to get it to output an error; everything just kind of dies
    if(TCCR4B != 0x09) Serial.println("Warning! Incorrect timer prescaler: TCCR4B");

    uint16_t ocr = OCR4C;
    ocr += TC4H << 8;
    if(ocr != 999) Serial.println("Warning! Incorrect timer TOP: TCCR4B");

    //technically, we should be sure not to clobber the high bits, but that won't happen
    //in this code
    TCCR4C = 0x05; //enable PWM; clear OC4D on compare match (pin 6 for the servo)

    sei();
}

void Servo32U4::Detach(void)
{
    cli();
    TCCR4C = 0x00; //disables the pulse
    sei();
}

void Servo32U4::Write(uint16_t microseconds)
{
    microseconds = constrain(microseconds, usMin, usMax);
    //prescaler is 256, so 1 timer count = 16 us

    //the two MSB have to be written first; they will always be 0, since microseconds < 2500 or so
    TC4H = 0;

    //then the low byte
    OCR4D = (microseconds >> 4); //divides by 16
}

uint16_t Servo32U4::SetMinMaxUS(uint16_t min, uint16_t max)
{
    usMin = min;
    usMax = (max > min) ? max : min; //in case they're mixed up, just constrain to min

    return usMax - usMin; //return the range, in case the user wants to do a sanity check
}
