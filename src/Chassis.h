#pragma once

#include <Arduino.h>
#include <Romi32U4Motors.h>
    
class Chassis
{
protected:
    float cmPerEncoderTick = 7.0 * 3.14 / 1440.0;
    float wheelTrack = 14.7;
    uint16_t ctrlIntervalMS = 16;

public:
//    static int8_t readyToPID;

    enum CTRL_MODE {CRTL_DIRECT, CRTL_SPEED, CTRL_DRIVE_FOR, CTRL_TURN_FOR};
    CTRL_MODE ctrlMode = CRTL_SPEED;

    //used for driving/turning set amounts
    float currDist = 0;
    float currAngle = 0;

    float targetDist = 0;
    float targetAngle = 0;

public:
    Chassis(void) {}
    void init(void);
    void loop(void);
    void idle(void);

    void updatePose(void);

    void setMotorEfforts(int leftSpeed, int rightSpeed);
    void setTwist(float forwardSpeed, float turningSpeed);

    void driveFor(float forwardDistance, float forwardSpeed, float turningSpeed = 0);
    void turnFor(float turnAngle, float turningSpeed, float forwardSpeed = 0);
};

extern Chassis chassis;
