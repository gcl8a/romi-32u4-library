#include <Chassis.h>

Romi32U4Motors motors;
//int8_t Chassis::readyToPID = 0;

void Chassis::init(void) {motors.init();}

void Chassis::idle(void)
{
  // //turn off LED
  // SetLED(LOW);

  //stop motors -- I use setEffort so that the wheels aren't locked
  motors.setEfforts(0, 0);

  //set state to idle
//  robotState = ROBOT_IDLE;
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
        Serial.println(millis());

        updatePose();
    }
}

void Chassis::updatePose(void)
{
    int16_t speedLeft = motors.speedLeft;   
}