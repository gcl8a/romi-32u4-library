#include <Chassis.h>

/**
 * Call init() in your setup() routine. It sets up some internal timers so that the speed controllers
 * for the wheels will work properly.
 * 
 * Here's how it works: Motor::init() starts a hardware timer on a 16 ms loop. Every time the timer 
 * "rolls over," an interrupt service routine (ISR) is called that updates the motor speeds and 
 * sets a flag to notify Chassis that it is time to calculate the control inputs.
 * 
 * When set up this way, pins 6, 12, and 13 cannot be used with analogWrite()
 * */
void Chassis::init(void) 
{
    Romi32U4Motor::init();

    //these can be undone for the student to adjust
    leftMotor.setPIDCoeffients(5, 0.5);
    rightMotor.setPIDCoeffients(5, 0.5);

    // temporarily turn off interrupts while we set the time up
    noInterrupts();

    // sets up timer 4 for a 16 ms loop, which triggers the motor PID controller
    // dt = 1024 (prescaler) * (249 + 1) / 16E6 (clock speed) = 16 ms
    TCCR4A = 0x00; //disable some functionality
    TCCR4B = 0x0B; //sets the prescaler to 1024
    TCCR4C = 0x00; //disable outputs (overridden for the servo class)
    TCCR4D = 0x00; //fast PWM mode (for servo)

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

        // leftMotor.update();
        // rightMotor.update();

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

    float prevDist = currDist;
    currDist += ((deltaLeft + deltaRight) / 2.0) * cmPerEncoderTick;

    float prevAngle = currAngle;
    currAngle += ((deltaRight - deltaLeft) * cmPerEncoderTick / robotRadius) * (180.0 / 3.14);
}

void Chassis::setTwist(float forwardSpeed, float turningSpeed)
{
    Serial.print("setting speed targets: \t");

    int16_t ticksPerIntervalFwd = (forwardSpeed * (ctrlIntervalMS / 1000.0)) / cmPerEncoderTick;
    int16_t ticksPerIntervalTurn = (robotRadius * 3.14 / 180.0) * 
                        (turningSpeed * (ctrlIntervalMS / 1000.0)) / cmPerEncoderTick;

    leftMotor.setTargetSpeed(ticksPerIntervalFwd - ticksPerIntervalTurn);
    rightMotor.setTargetSpeed(ticksPerIntervalFwd + ticksPerIntervalTurn);
}

//need to fill these in
void Chassis::driveFor(float forwardDistance, float forwardSpeed, float turningSpeed)
{
    setTwist(forwardSpeed, turningSpeed); //sets the speeds
    int16_t delta = forwardDistance / cmPerEncoderTick;
    leftMotor.moveFor(delta);
    rightMotor.moveFor(delta);
}

void Chassis::turnFor(float turnAngle, float turningSpeed, float forwardSpeed)
{
    setTwist(forwardSpeed, turningSpeed); //sets the speeds
    int16_t delta = turnAngle * (robotRadius * 3.14 / 180.0) / cmPerEncoderTick;
    leftMotor.moveFor(-delta);
    rightMotor.moveFor(delta);
}

bool Chassis::checkMotionComplete(void)
{
    bool complete = leftMotor.checkComplete() && rightMotor.checkComplete();
    return complete;
}

/**
 * ISR for timing. On overflow of Timer4, the ISR takes a 'snapshot' of the encoder counts 
 * and then raises a flag to let the main program know it is time to execute the PID calculations.
 * 
 * Do not edit this function -- adding length function calls will cause headaches.
 * */
ISR(TIMER4_OVF_vect)
{
    chassis.updateEncoderDeltas();
}

void Chassis::updateEncoderDeltas(void)
{
    leftMotor.calcEncoderDelta();
    rightMotor.calcEncoderDelta();

    leftMotor.update();
    rightMotor.update();

    readyToPID++;
}
    