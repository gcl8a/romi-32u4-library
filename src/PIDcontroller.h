#pragma once

#include <Arduino.h>

class PIDController
{
protected:
    float Kp, Ki, Kd;
    float currError = 0;
    float prevError = 0;
    
    float sumError = 0;
    float errorBound = 0;

    float deltaT = 0; //not used for now; could be useful

    float currEffort = 0;

public:
    PIDController(float p, float i = 0, float d = 0, float bound = 0) : Kp(p), Ki(i), Kd(d), errorBound(bound) {}
    float calcEffort(float current);
    float setKp(float k) {return Kp = k;}
    float setKi(float k) {sumError = 0; return Ki = k;}
    float setKd(float k) {return Kd = k;}
    float setCap(float cap) {return errorBound = cap;}
    void resetSum(void) {sumError = 0;}
};
