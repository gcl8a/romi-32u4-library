// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <Romi32U4Motors.h>
#include <FastGPIO.h>
#include <avr/io.h>

// define the motor pins here
#define PWM_L 10
#define PWM_R 9
#define DIR_L 16
#define DIR_R 15

//NEED TO UNDO PIN 11?

// initialize timer1 to generate the proper PWM outputs to the motor drivers
void Romi32U4Motor::initMotors()
{
    Serial.println("initMotors()");

    FastGPIO::Pin<PWM_L>::setOutputLow();
    FastGPIO::Pin<PWM_R>::setOutputLow();
    FastGPIO::Pin<DIR_L>::setOutputLow();
    FastGPIO::Pin<DIR_R>::setOutputLow();

    FastGPIO::Pin<11>::setOutputLow(); //pin 11 for output C

    noInterrupts(); //disable interupts while we set Timer4 registers

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
    
    interrupts(); //re-enable interrupts

    Serial.println("/initMotors()");
}

void LeftMotor::setEffort(int16_t effort)
{
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

void RightMotor::setEffort(int16_t effort)
{
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

void Romi32U4Motor::allowTurbo(bool turbo)
{
    maxEffort = turbo ? 400 : 300;
}

void Romi32U4Motor::update(void)
{
    if(ctrlMode == CTRL_SPEED || ctrlMode == CTRL_POS)
    {
        int16_t effort = pidCtrl.calcEffort(targetSpeed - speed);
        setEffort(effort);
    }
}

void Romi32U4Motor::setTargetSpeed(int16_t target)
{
    targetSpeed = target;

    if(ctrlMode != CTRL_SPEED)
    {
        //when new target speeds are set, reset the error integral
        pidCtrl.resetSum();
    }

    ctrlMode = CTRL_SPEED;
}

void Romi32U4Motor::moveFor(int16_t amount, int16_t speed)
{
    setTargetSpeed(speed);
    cli();
    int16_t currPos = count;
    sei();

    targetPos = currPos + amount;
    ctrlMode = CTRL_POS;
}
