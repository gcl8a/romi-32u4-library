#pragma once

#include <Arduino.h>
#include <Romi32U4Motors.h>
    
class Chassis
{
protected:

public:
//    static int8_t readyToPID;

public:
    Chassis(void) {}
    void init(void);
    void loop(void);
    void idle(void);

    void updatePose(void);

    void setMotorEfforts(int leftSpeed, int rightSpeed);
};

extern Chassis chassis;
