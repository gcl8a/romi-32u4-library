#include <Chassis.h>

/**
 * Call init() in your setup() routine. It sets up some internal timers so that the speed controllers
 * for the wheels will work properly.
 * 
 * The way it works, it starts a hardware timer on a 16 ms loop. Every time the timer "rolls over,"
 * an interrupt service routine (ISR) is called that updates the motor speeds and sets a flag to notify
 * the main program that it is time to calculate the control inputs.
 * 
 * When set up this way, pins 6, 12, and 13 cannot be used with analogWrite()
 * */
void Chassis::init(void) 
{
    Romi32U4Motor::init();

    // temporarily turn off interrupts while we set the time up
    noInterrupts();

    //sets up timer 4 for a 16 ms loop, which triggers the motor PID controller
    TCCR4A = 0x00; //disable some functionality
    TCCR4B = 0x0B; //sets the prescaler 
    TCCR4C = 0x00; //
    TCCR4D = 0x00; //normal mode

    OCR4C = 249;   //TOP goes in OCR4C

    TIMSK4 = 0x04; //enable overflow interrupt
    
    // re-enable interrupts
    interrupts();
}

/**
 * loop() must be called regularly to properly control the motor speed
 * */
void Chassis::loop(void)
{
    if(readyToPID)
    {
        readyToPID = 0;

        leftMotor.update();
        rightMotor.update();

        updatePose();
    }
}

/**
 * Stops the motors. It calls setMotorEfforts() so that the wheels won't lock. Use setSpeeds() if you want 
 * the wheels to 'lock' in place.
 * */
void Chassis::idle(void)
{
    setMotorEfforts(0, 0);
}

/**
 * Sets the motor _efforts_.
 * */
void Chassis::setMotorEfforts(int leftSpeed, int rightSpeed)
{
    leftMotor.setEffort(leftSpeed);
    rightMotor.setEffort(rightSpeed);
}

void Chassis::updatePose(void)
{
    int16_t deltaLeft = leftMotor.speed;   
    int16_t deltaRight = rightMotor.speed;

    ////////!!!!!!convert to actual distances...
    float prevDist = currDist;
    currDist += ((deltaLeft + deltaRight) / 2.0) * cmPerEncoderTick;

    float prevAngle = currAngle;
    currAngle += ((deltaRight - deltaLeft) * cmPerEncoderTick / wheelTrack) * (180.0 / 3.14);
}

void Chassis::setTwist(float forwardSpeed, float turningSpeed)
{
    Serial.print("setting speed targets: \t");

    int16_t ticksPerIntervalFwd = (forwardSpeed * (ctrlIntervalMS / 1000.0)) / cmPerEncoderTick;
    int16_t ticksPerIntervalTurn = (wheelTrack * 3.14 / 180.0) * 
                        (turningSpeed * (ctrlIntervalMS / 1000.0)) / cmPerEncoderTick;

    leftMotor.setTargetSpeed(ticksPerIntervalFwd - ticksPerIntervalTurn);
    rightMotor.setTargetSpeed(ticksPerIntervalFwd + ticksPerIntervalTurn);
}

//need to fill these in
void Chassis::driveFor(float forwardDistance, float forwardSpeed, float turningSpeed)
{
    leftMotor.moveFor(1440, 20);
    rightMotor.moveFor(1440, 20);
}

void Chassis::turnFor(float turnAngle, float turningSpeed, float forwardSpeed)
{
    leftMotor.moveFor(-1440, -20);
    rightMotor.moveFor(1440, 20);
}

bool Chassis::checkMotionComplete(void)
{
    return leftMotor.checkComplete() && rightMotor.checkComplete();
}

/**
 * ISR for timing. On overflow of Timer4, the ISR takes a 'snapshot' of the encoder counts 
 * and then raises a flag to let the main program know it is time to execute the PID calculations.
 * 
 * Do not edit this function -- adding length function calls will cause headaches.
 * */
ISR(TIMER4_OVF_vect)
{
    chassis.leftMotor.calcEncoderDelta();
    chassis.rightMotor.calcEncoderDelta();
    chassis.readyToPID++;
}
