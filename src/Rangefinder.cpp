#include <Rangefinder.h>
#include <pcint.h>

#define ECHO_RECD   0x02

// constructor
Rangefinder::Rangefinder(uint8_t echo, uint8_t trig) 
{
    echoPin = echo;
    trigPin = trig;
}

// sets up the interface
void Rangefinder::init(void)
{
  // assert ECHO pin is an input
  pinMode(echoPin, INPUT);

  // register the interrupt for the echo
  if(digitalPinToInterrupt(echoPin) != NOT_AN_INTERRUPT)
  {
    Serial.println("Attaching rangefinder ISR");
    attachInterrupt(digitalPinToInterrupt(echoPin), ::ISR_Rangefinder, CHANGE);
  }
  else if(digitalPinToPCInterrupt(echoPin) != NOT_AN_INTERRUPT)
  {
    Serial.println("Attaching rangefinder PC_ISR");
    attachPCInt(digitalPinToPCInterrupt(echoPin), ::ISR_Rangefinder);
  }
  else
  {
    Serial.println("Not a rangefinder interrupt pin!");
  }

  //control pin for commanding pings must be an output
  pinMode(trigPin, OUTPUT);
}

/**
 * checkPingTimer check to see if it's time to send a new ping.
 */
uint8_t Rangefinder::checkPingTimer(void)
{
    // if the echo pin is still high, just update the last ping time
    if(digitalRead(echoPin)) lastPing = millis();

    // check if we're ready to ping
    if(millis() - lastPing >= pingInterval)
    {
        //disable interrupts while we adjust the ISR variables
        cli();
        pulseEnd = pulseStart = 0;

        //clear out any leftover states
        state = 0;
        sei();

        // keep track of when we sent the last ping
        lastPing = millis();  

        // toggle the trigger pin to send a chirp
        digitalWrite(trigPin, HIGH); //commands a ping; leave high for the duration
        delayMicroseconds(30); //datasheet says hold HIGH for >20us; we'll use 30 to be 'safe'
        digitalWrite(trigPin, LOW); //unclear if pin has to stay HIGH
    }

    return state;
}

uint16_t Rangefinder::checkEcho(void)
{
    uint16_t echoLength = 0;
    if(state & ECHO_RECD)
    {
        cli();
        echoLength = pulseEnd - pulseStart;
        state &= ~ECHO_RECD;
        sei();
    }

    return echoLength;
}

float Rangefinder::getDistance(void)
{
    checkPingTimer();

    uint16_t pulseDur = checkEcho();
    if(pulseDur) distance = pulseDur / 58.0;

    return distance;
}

// ISR for echo pin
void Rangefinder::ISR_echo(void)
{
    if(digitalRead(echoPin))  //transitioned to HIGH
    {
        pulseStart = micros();
    }

    else                      //transitioned to LOW
    {
        pulseEnd = micros();
        state |= ECHO_RECD;
    } 
}

void ISR_Rangefinder(void)
{
    rangefinder.ISR_echo();
}