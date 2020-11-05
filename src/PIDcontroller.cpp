#include <PIDcontroller.h>
#include <math.h>

float PIDController::ComputeEffort(float error)
{
    currError = error; //store in case we want it later
    sumError += currError;

    if(errorBound > 0) //cap error; errorBound == 0 means don't cap
    {
        if(fabs(sumError) > errorBound) //not sure if should multiply by Ki or not to make it scale
        {
            sumError -= currError;
        }
    }

    float derivError = currError - prevError;
    prevError = currError;

    currEffort = Kp * currError + Ki * sumError + Kd * derivError;

    return currEffort;
}