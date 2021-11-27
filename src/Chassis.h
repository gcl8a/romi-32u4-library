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
    const float cmPerEncoderTick;
    const float robotRadius;
    const uint16_t ctrlIntervalMS = 16;

public:
    Chassis(float wheelDiam, float ticksPerRevolution, float wheelTrack) 
        : cmPerEncoderTick(wheelDiam * M_PI / ticksPerRevolution), robotRadius(wheelTrack / 2.0)
        {}
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
    //void updatePose(void);
};

extern Chassis chassis;
