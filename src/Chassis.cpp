#include <Chassis.h>

Romi32U4Motors motors;

void Chassis::init(void) {motors.init();}

void Chassis::idle(void)
{
  //stop motors -- I use setEffort so that the wheels aren't locked
  motors.setEfforts(0, 0);

  ctrlMode = CRTL_DIRECT;
}

void Chassis::setMotorEfforts(int leftSpeed, int rightSpeed)
{
    motors.setEfforts(leftSpeed, rightSpeed);
}

void Chassis::loop(void)
{
    if(motors.readyToPID)
    {
        motors.readyToPID = 0;
        //Serial.println(millis());

        motors.updateMotors();

        updatePose();
    }
}

void Chassis::updatePose(void)
{
    int16_t deltaLeft = motors.speedLeft;   
    int16_t deltaRight = motors.speedRight;

    ////////!!!!!!convert to actual distances...

    currDist += ((deltaLeft + deltaRight) / 2.0) * cmPerEncoderTick;
    currAngle += ((deltaRight - deltaLeft) * cmPerEncoderTick / wheelTrack) * (180.0 / 3.14);

    if(ctrlMode == CTRL_DRIVE_FOR) 
    {
        if(currDist >= targetDist)
        {
            idle();
        }
    }

    if(ctrlMode == CTRL_TURN_FOR) 
    {
        if(currAngle >= targetAngle)
        {
            idle();
        }
    }
}

void Chassis::setTwist(float forwardSpeed, float turningSpeed)
{
    Serial.print("setting speed targets: \t");
    Serial.println(forwardSpeed);

    int16_t ticksPerIntervalFwd = (forwardSpeed * (ctrlIntervalMS / 1000.0)) / cmPerEncoderTick;
    int16_t ticksPerIntervalTurn = (wheelTrack * 3.14 / 180.0) * 
                        (turningSpeed * (ctrlIntervalMS / 1000.0)) / cmPerEncoderTick;

    Serial.print("\t");
    Serial.print(ticksPerIntervalFwd);
    Serial.print("\t");
    Serial.println(ticksPerIntervalTurn);

    motors.setTargetSpeeds(ticksPerIntervalFwd - ticksPerIntervalTurn, 
                            ticksPerIntervalFwd + ticksPerIntervalTurn);
}


void Chassis::driveFor(float forwardDistance, float forwardSpeed, float turningSpeed)
{
    targetDist = currDist + forwardDistance;
    ctrlMode = CTRL_DRIVE_FOR;
    
    setTwist(forwardSpeed, turningSpeed);
}

void Chassis::turnFor(float turnAngle, float turningSpeed, float forwardSpeed)
{
    targetAngle = currAngle + turnAngle;
    ctrlMode = CTRL_TURN_FOR;

    setTwist(forwardSpeed, turningSpeed);
}
