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
class Romi32U4Motors
{
public:
  /** \brief Sets the effort for the left motor.
     *
     * \param effort A number from -300 to 300 representing the effort and
     * direction of the left motor.  Values of -300 or less result in full effort
     * reverse, and values of 300 or more result in full effort forward. */
  void setLeftEffort(int16_t effort);

  /** \brief Sets the effort for the right motor.
     *
     * \param effort A number from -300 to 300 representing the effort and
     * direction of the right motor. Values of -300 or less result in full effort
     * reverse, and values of 300 or more result in full effort forward. */
  void setRightEffort(int16_t effort);

  /** \brief Sets the effort for both motors.
     *
     * \param leftEffort A number from -300 to 300 representing the speed and
     * direction of the right motor. Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward.
     * \param rightEffort A number from -300 to 300 representing the speed and
     * direction of the right motor. Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward. */
  void setEfforts(int16_t leftEffort, int16_t rightEffort);

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

  /** \brief returns the Max Effort for the motors
     * 
     * \return The maximum effort for the motors based on the turbo setting */
  int16_t getMaxEffort();

  inline void init()
  {
    bool initialized = false;

    if (!initialized)
    {
      initialized = true;
      initMotors();
      initEncoders();
    }
  }
  
  inline void motorISR(void);

private:

  void initMotors();
  void initEncoders();
  int16_t maxEffort = 300;

public:
   uint8_t readyToPID = 0;

   int16_t prevCountLeft;
   int16_t prevCountRight;

   int16_t speedLeft;
   int16_t speedRight;

   int16_t targetSpeedLeft = 0;
   int16_t targetSpeedRight = 0;

   volatile bool lastLeftA;
   volatile bool lastLeftB;
   volatile bool lastRightA;
   volatile bool lastRightB;

   volatile bool errorLeft;
   volatile bool errorRight;

   PIDController pidCtrlLeft;
   PIDController pidCtrlRight;

   // volatile int16_t countLeft;
   // volatile int16_t countRight;


public:
      /*! Returns the number of counts that have been detected from the left-side
     * encoder.  These counts start at 0.  Positive counts correspond to forward
     * movement of the left side of the Romi, while negative counts correspond
     * to backwards movement.
     *
     * The count is returned as a signed 16-bit integer.  When the count goes
     * over 32767, it will overflow down to -32768.  When the count goes below
     * -32768, it will overflow up to 32767. */
    int16_t getCountsLeft();

    /*! This function is just like getCountsLeft() except it applies to the
     *  right-side encoder. */
    int16_t getCountsRight();

    /*! This function is just like getCountsLeft() except it also clears the
     *  counts before returning.  If you call this frequently enough, you will
     *  not have to worry about the count overflowing. */
    int16_t getCountsAndResetLeft();

    /*! This function is just like getCountsAndResetLeft() except it applies to
     *  the right-side encoder. */
    int16_t getCountsAndResetRight();

    /*! This function returns true if an error was detected on the left-side
     * encoder.  It resets the error flag automatically, so it will only return
     * true if an error was detected since the last time checkErrorLeft() was
     * called.
     *
     * If an error happens, it means that both of the encoder outputs changed at
     * the same time from the perspective of the ISR, so the ISR was unable to
     * tell what direction the motor was moving, and the encoder count could be
     * inaccurate.  The most likely cause for an error is that the interrupt
     * service routine for the encoders could not be started soon enough.  If
     * you get encoder errors, make sure you are not disabling interrupts for
     * extended periods of time in your code. */
    bool checkErrorLeft();

    /*! This function is just like checkErrorLeft() except it applies to
     *  the right-side encoder. */
    bool checkErrorRight();

    void leftISR(void);
    void rightISR(void);
};

extern Romi32U4Motors motors;