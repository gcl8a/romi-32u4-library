#include <Chassis.h>

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