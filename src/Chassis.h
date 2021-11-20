#pragma once

#include <Arduino.h>
#include <Romi32U4Motors.h>

class Chassis
{
public:
    volatile uint8_t readyToPID = 0;

    LeftMotor leftMotor;
    RightMotor rightMotor;

protected:
    const float cmPerEncoderTick = 7.0 * 3.14 / 1440.0;
    const float robotRadius = 14.7 / 2.0;
    const uint16_t ctrlIntervalMS = 16;

    //used for driving/turning set amounts
    // float currDist = 0;
    // float currAngle = 0;

    // float targetDist = 0;
    // float targetAngle = 0;

public:
    Chassis(void) {}
    void init(void);
    void setMotorPIDcoeffs(float kp, float ki);

    void loop(void);
    void idle(void);

    void setMotorEfforts(int leftEffort, int rightEffort);
    void setTwist(float forwardSpeed, float turningSpeed);

    void driveFor(float forwardDistance, float forwardSpeed);
    void turnFor(float turnAngle, float turningSpeed);

    bool checkMotionComplete(void);

    inline void updateEncoderDeltas();
protected:
    void updatePose(void);
};

extern Chassis chassis;
