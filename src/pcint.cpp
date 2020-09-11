#include <pcint.h>

typedef void(*PCISR)(); //function pointer
PCISR pcISR[] = {0, 0, 0, 0, 0, 0, 0, 0}; //initialize array to null

void attachPCInt(uint8_t pcInt, void (*pcisr)(void))
{
    cli();
    PCICR = (1 << PCIE0);   //enable PC interrupts

    PCMSK0 |= (1 << pcInt); //enable PCInt on the specific pin
    pcISR[pcInt] = pcisr;   //register the ISR

    PCIFR = (1 << PCIF0);   // Clear its interrupt flag by writing a 1.
    sei();
}

ISR(PCINT0_vect)
{
    static volatile uint8_t lastB = PINB;

    volatile uint8_t pinsB = PINB;
    volatile uint8_t deltaB = pinsB ^ lastB;                //pins that have changed

    volatile uint8_t maskedDeltaB = deltaB & PCMSK0;        //pins that have changed AND are set for interrupts

    if((maskedDeltaB & (1 << PCINT0))) {pcISR[PCINT0]();}   //each of these checks if ind. ISRs should run
    if((maskedDeltaB & (1 << PCINT1))) {pcISR[PCINT1]();}
    if((maskedDeltaB & (1 << PCINT2))) {pcISR[PCINT2]();}
    if((maskedDeltaB & (1 << PCINT3))) {pcISR[PCINT3]();}
    if((maskedDeltaB & (1 << PCINT4))) {pcISR[PCINT4]();}
    if((maskedDeltaB & (1 << PCINT5))) {pcISR[PCINT5]();}
    if((maskedDeltaB & (1 << PCINT6))) {pcISR[PCINT6]();}
    if((maskedDeltaB & (1 << PCINT7))) {pcISR[PCINT7]();}

    lastB = pinsB;                                          //track last pin states
}
