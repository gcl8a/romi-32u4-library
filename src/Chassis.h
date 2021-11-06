#pragma once

#include <Arduino.h>
#include <Romi32U4Motors.h>

class Chassis
{
protected:
    Romi32U4Motors motors;
    //Romi32U4Encoders encoders;

public:
    Chassis(void) {}
    void idle(void);

    void setMotorEfforts(int leftSpeed, int rightSpeed);
};