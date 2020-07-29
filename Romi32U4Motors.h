// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Romi32U4Motors.h */

#pragma once

#include <stdint.h>
#include <FastGPIO.h>

/*! \brief Controls motor speed and direction on the Romi 32U4.
 *
 * This library uses Timer 1, so it will conflict with any other libraries using
 * that timer. */
class Romi32U4Motors
{
  public:

    /** \brief Flips the direction of the left motor.
     *
     * You can call this function with an argument of \c true if the left motor
     * of your Romi was not wired in the standard way and you want a
     * positive speed argument to correspond to forward movement.
     *
     * \param flip If true, then positive motor speeds will correspond to the
     * direction pin being high.  If false, then positive motor speeds will
     * correspond to the direction pin being low.
     */
    static void flipLeftMotor(bool flip);

    /** \brief Flips the direction of the right motor.
     *
     * You can call this function with an argument of \c true if the right motor
     * of your Romi was not wired in the standard way and you want a
     * positive speed argument to correspond to forward movement.
     *
     * \param flip If true, then positive motor speeds will correspond to the
     * direction pin being high.  If false, then positive motor speeds will
     * correspond to the direction pin being low. */
    static void flipRightMotor(bool flip);

    /** \brief Sets the speed for the left motor.
     *
     * \param speed A number from -300 to 300 representing the speed and
     * direction of the left motor.  Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward. */
    static void setLeftEffort(int16_t speed);

    /** \brief Sets the speed for the right motor.
     *
     * \param speed A number from -300 to 300 representing the speed and
     * direction of the right motor. Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward. */
    static void setRightEffort(int16_t speed);

    /** \brief Sets the speed for both motors.
     *
     * \param leftSpeed A number from -300 to 300 representing the speed and
     * direction of the right motor. Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward.
     * \param rightSpeed A number from -300 to 300 representing the speed and
     * direction of the right motor. Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward. */
    static void setEfforts(int16_t leftEffort, int16_t rightEffort);

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
    static void allowTurbo(bool turbo);

  private:

    static inline void init()
    {
        static bool initialized = false;

        if (!initialized)
        {
            initialized = true;
            init2();
        }
    }

    static void init2();

    static int16_t maxEffort;
    static bool flipLeft;
    static bool flipRight;
};

// /*! \brief Controls motor speed and direction for a single motor on the Romi 32U4.
//  *
//  * This library uses Timer 1, so it will conflict with any other libraries using
//  * that timer. */
// template <uint8_t DIR, uint8_t PWM, uint8_t addr> class Romi32U4Motor
// {
//   private:
//     FastGPIO::Pin<PWM> pwm;
//     FastGPIO::Pin<DIR> dir;

//   public:
//     void SetEffort(int16_t effort)
//     {
//       if(effort < 0) {dir.setOutputHigh(); effort *= -1;}
//       else {dir.setOutputLow();}
//       _SFR_MEM16(addr) = abs(effort);
//     }
// };