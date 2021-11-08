#include <Chassis.h>

void Chassis::init(void) {Romi32U4Motor::init();}

void Chassis::idle(void)
{
  //stop motors -- I use setEffort so that the wheels aren't locked
  leftMotor.setEffort(0);
  rightMotor.setEffort(0);

//   ctrlMode = CTRL_DIRECT;
}

void Chassis::setMotorEfforts(int leftSpeed, int rightSpeed)
{
    leftMotor.setEffort(leftSpeed);
    rightMotor.setEffort(rightSpeed);
}

void Chassis::loop(void)
{
    if(Romi32U4Motor::readyToPID)
    {
        Romi32U4Motor::readyToPID = 0;
        //Serial.println(millis());

        leftMotor.update();
        rightMotor.update();

        updatePose();
        Serial.print('\n');
    }
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

    // if(ctrlMode == CTRL_DRIVE_FOR) 
    // {
    //     if((currDist >= targetDist) != (prevDist >= targetDist))
    //     {
    //         idle();
    //     }
    // }

    // if(ctrlMode == CTRL_TURN_FOR) 
    // {
    //     if((currAngle >= targetAngle) != (prevAngle >= targetAngle))
    //     {
    //         idle();
    //     }
    // }
}

void Chassis::setTwist(float forwardSpeed, float turningSpeed)
{
    Serial.print("setting speed targets: \t");

    int16_t ticksPerIntervalFwd = (forwardSpeed * (ctrlIntervalMS / 1000.0)) / cmPerEncoderTick;
    int16_t ticksPerIntervalTurn = (wheelTrack * 3.14 / 180.0) * 
                        (turningSpeed * (ctrlIntervalMS / 1000.0)) / cmPerEncoderTick;

    Serial.print(ticksPerIntervalFwd);
    Serial.print("\t");
    Serial.println(ticksPerIntervalTurn);

    leftMotor.setTargetSpeed(ticksPerIntervalFwd - ticksPerIntervalTurn);
    rightMotor.setTargetSpeed(ticksPerIntervalFwd + ticksPerIntervalTurn);
}

void Chassis::driveFor(float forwardDistance, float forwardSpeed, float turningSpeed)
{
    leftMotor.MoveFor(1440, 20);
    rightMotor.MoveFor(1440, 20);
}

void Chassis::turnFor(float turnAngle, float turningSpeed, float forwardSpeed)
{
    leftMotor.MoveFor(-1440, -20);
    rightMotor.MoveFor(1440, 20);
}

bool Chassis::checkMotionComplete(void)
{
    return leftMotor.checkComplete() && leftMotor.checkComplete();
    //return ctrlMode == CTRL_DIRECT;
}