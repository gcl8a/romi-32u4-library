#pragma once

class PIDController
{
private:
    float Kp, Ki, Kd;
    float currError = 0;
    float prevError = 0;
    
    float sumError = 0;
    float errorBound = 0;

    float deltaT = 0; //not used for now; could be useful

    float currEffort = 0;

public:
    PIDController(float p, float i, float d, float bound = 0) : Kp(p), Ki(i), Kd(d), errorBound(bound) {}
    float ComputeEffort(float error);
};

