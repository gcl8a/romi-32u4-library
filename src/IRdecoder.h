#include <Arduino.h>

class IRDecoder
{
private:
    enum IR_STATE {IR_READY, IR_PREAMBLE, IR_REPEAT, IR_ACTIVE, IR_COMPLETE, IR_ERROR};
    IR_STATE state = IR_READY;

    volatile uint32_t lastReceiveTime = 0; //not really used -- could be used to sunset codes

    volatile uint32_t currCode = 0;
    volatile uint8_t index = 0;

    volatile uint32_t fallingEdge = 0;
    volatile uint32_t risingEdge = 0;

    volatile uint32_t lastRisingEdge = 0;
public:
    //volatile uint16_t bits[32];

public:
    void init(void);
    void handleIRsensor(void);
    uint32_t getCode(void)
    {
      if(state == IR_COMPLETE || state == IR_REPEAT) {state = IR_READY; return currCode;}
      else return 0;
    }
};

extern IRDecoder decoder;