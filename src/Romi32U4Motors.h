// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Romi32U4Motors.h */

#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <PIDController.h>

//#include <FastGPIO.h>

/*! \brief Controls motor effort and direction on the Romi 32U4.
 *
 * This library uses Timer 1, so it will conflict with any other libraries using
 * that timer. 
 * 
 * Also reads counts from the encoders on the Romi 32U4.
 *
 * This class allows you to read counts from the encoders on the Romi 32U4,
 * which lets you tell how much each motor has turned and in what direction.
 *
 * The encoders are monitored in the background using interrupts, so your code
 * can perform other tasks without missing encoder counts.
 *
 * To read the left encoder, this class uses an interrupt service routine (ISR)
 * for PCINT0_vect, so there will be a compile-time conflict with any other code
 * that defines a pin-change ISR.
 *
 * To read the right encoder, this class calls
 * [attachInterrupt()](http://arduino.cc/en/Reference/attachInterrupt), so there
 * will be a compile-time conflict with any other code that defines an ISR for
 * an external interrupt directly instead of using attachInterrupt().
 *
 * The standard Romi motors have a gear ratio of 3952:33 (approximately 120:1).
 * The standard Romi encoders give 12 counts per revolution.  Therefore, one
 * revolution of a Romi wheel corresponds to 12*3952/33 (approximately 1437.09)
 * encoder counts as returned by this library.
 */
class Romi32U4Motor
{
protected:
   enum CTRL_MODE {CTRL_DIRECT, CTRL_SPEED, CTRL_POS};
   CTRL_MODE ctrlMode = CTRL_DIRECT;
   
public:
  /** \brief Sets the effort for the left motor.
     *
     * \param effort A number from -300 to 300 representing the effort and
     * direction of the left motor.  Values of -300 or less result in full effort
     * reverse, and values of 300 or more result in full effort forward. */
  virtual void setEffort(int16_t effort) = 0;

  /** \brief Turns turbo mode on or off.
     *
     * By default turbo mode is off.  When turbo mode is on, the range of speeds
     * accepted by the other functions in this library becomes -400 to 400
     * (instead of -300 to 300).  Turning turbo mode on allows the Romi to move
     * faster but could decrease the lifetime of the motors.
     *
     * This function does not have any immediate effect on the speed of the
     * motors; it just changes the behavior of the other functions in this
     * library.
     *
     * \param turbo If true, turns turbo mode on.
     *   If false, turns turbo mode off. */
   void allowTurbo(bool turbo);

   Romi32U4Motor(void) : pidCtrl(5, 0.2)
   {}

  static void init()
  {
    {
      initMotors();
      initEncoders();
    }
  }
  
  inline void motorISR(void);

protected:

  static void initMotors();
  static void initEncoders();

  int16_t maxEffort = 300;

public:
   volatile int16_t prevCount;

   int16_t speed = 0;

   int16_t targetSpeed = 0;

   int16_t targetPos = 0;

   volatile int16_t count = 0;

   volatile int16_t lastA = 0;
   volatile int16_t lastB = 0;

   volatile int16_t delta = 0;

   PIDController pidCtrl;

public:
      /*! Returns the number of counts that have been detected from the left-side
     * encoder.  These counts start at 0.  Positive counts correspond to forward
     * movement of the left side of the Romi, while negative counts correspond
     * to backwards movement.
     *
     * The count is returned as a signed 16-bit integer.  When the count goes
     * over 32767, it will overflow down to -32768.  When the count goes below
     * -32768, it will overflow up to 32767. */
    int16_t getCount(void);
    int16_t returnAndResetCount(void);

    inline void handleISR(bool newA, bool newB);

    void calcEncoderDelta(void);
    void update(void);

    void setTargetSpeed(int16_t targetSpeed);
    void moveFor(int16_t amount, int16_t speed);
    bool checkComplete(void) {return ctrlMode == CTRL_DIRECT;}
};

class LeftMotor : public Romi32U4Motor
{
public:
   void setEffort(int16_t effort);
};

class RightMotor : public Romi32U4Motor
{
public:
   void setEffort(int16_t effort);
};

// trying without externs -- see Chassis
// extern LeftMotor leftMotor;
// extern RightMotor rightMotor;