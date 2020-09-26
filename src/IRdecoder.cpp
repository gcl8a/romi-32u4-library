#include <IRdecoder.h>
#include <FastGPIO.h>
#include <pcint.h>


void handleIRsensor(void)
{
  decoder.handleIRsensor();
}

void IRDecoder::init(void)
{
  //SET UP FOR PIN 1
  pinMode(1, INPUT);
  attachInterrupt(digitalPinToInterrupt(1), ::handleIRsensor, CHANGE);

  //UNCOMMENT THESE AND CHANGE LINE 27 TO MATCH TO USE PIN 14 (one of the PCINTs)
  //pinMode(14, INPUT);
  //attachPCInt(PCINT3, ::handleIRsensor);
}

void IRDecoder::handleIRsensor(void)
{
  uint32_t currUS = micros();

  //THIS MUST AGREE WITH THE PIN IN THE init() FUNCTION.
  if(!FastGPIO::Pin<1>::isInputHigh()) // FALLING edge
  {
    fallingEdge = currUS; 
  }

  else // RISING edge
  {
    risingEdge = currUS;

    //and process
    uint32_t delta = risingEdge - fallingEdge; //length of pulse, in us
    uint32_t codeLength = risingEdge - lastRisingEdge;
    lastRisingEdge = risingEdge;

    // bits[index] = delta;
    
    if(delta > 8500 && delta < 9500) // received a start pulse
    {
      index = 0;
      state = IR_PREAMBLE;
      return;
    }
    
    else if(delta < 520 || delta > 700) // pulse wasn't right length => error
    {
      state = IR_ERROR;
      currCode = -1;

      return;
    }

    else if(state == IR_PREAMBLE)
    {
      if(codeLength < 5300 && codeLength > 4800) //preamble
      {
        currCode = 0;
        state = IR_ACTIVE;
      }

      else if(codeLength < 3300 && codeLength > 2800) //repeat code
      {
        state = IR_REPEAT;
        lastReceiveTime = millis();
      }
    }

    else if(state == IR_ACTIVE)
    {
      //bits[index] = codeLength;
      
      if(codeLength < 1300 && codeLength > 900) //short = 0
      {
  //      bits[index] = 0;
        index++;
      }
      
      else if(codeLength < 2500 && codeLength > 2000) //long = 1
      {
  //      bits[index] = 1;
        currCode += ((uint32_t)1 << index);
        index++;
      }
      
      else //error
      {
        state = IR_ERROR;
      }

      if(index == 32) 
      {
        if(((currCode ^ (currCode >> 8)) & 0x00ff00ff) != 0x00ff00ff) state = IR_ERROR;
        else
        {        
          state = IR_COMPLETE;
          lastReceiveTime = millis();
        }
      }
    }
  }
}