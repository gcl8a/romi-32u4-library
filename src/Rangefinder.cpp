#include <Rangefinder.h>
#include <pcint.h>

// Constructor
Rangefinder::Rangefinder(uint8_t echo, uint8_t trig) 
{
    echoPin = echo;
    trigPin = trig;
}

void ISR_Rangefinder(void)
{
    rangefinder.ISR_echo();
}

// Allows the user to select the interface
void Rangefinder::init(void)
{
  // assert ECHO pin is an input
  pinMode(echoPin, INPUT);

  if(digitalPinToInterrupt(echoPin) != NOT_AN_INTERRUPT)
  {
    Serial.println("Attaching ISR");
    attachInterrupt(digitalPinToInterrupt(echoPin), ::ISR_Rangefinder, CHANGE);
  }
  else if(digitalPinToPCInterrupt(echoPin) != NOT_AN_INTERRUPT)
  {
    Serial.println("Attaching PC_ISR");
    attachPCInt(digitalPinToPCInterrupt(echoPin), ::ISR_Rangefinder);
  }
  else
  {
    Serial.println("Not an interrupt pin!");
  }

  //control pin for commanding pings
  pinMode(trigPin, OUTPUT);
}

/**
 * checkPingTimer check to see if it's time to send a new ping.
 */
uint8_t Rangefinder::checkPingTimer(void)
{
    if(digitalRead(echoPin)) lastPing = millis();
    //check if we're ready to ping
    if(millis() - lastPing >= pingInterval)
    {
        pulseEnd = pulseStart = 0;

        //clear out any leftover states
        state = 0;

        lastPing = millis();    //not perfectly on schedule, but safer and close enough

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
        echoLength = pulseEnd - pulseStart;
        state &= ~ECHO_RECD;
    }

    return echoLength;
}

//ISR for echo pin
void Rangefinder::ISR_echo(void)
{
    if(digitalRead(echoPin))  //transitioned to HIGH
    {
        pulseStart = micros();
    }

    else                        //transitioned to LOW
    {
        pulseEnd = micros();
        state |= ECHO_RECD;
    } 
}

float Rangefinder::getDistance(void)
{
    checkPingTimer();

    uint16_t pulseDur = checkEcho();
    if(pulseDur) distance = pulseDur / 58.0;

    return distance;
}