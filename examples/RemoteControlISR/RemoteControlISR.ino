
#include <Romi32U4.h>
#include "RemoteConstants.h"
#include "IRdecoder.h"

IRDecoder decoder;

void setup()
{
  decoder.init();
}

void loop()
{ 
  delay(1);
  uint32_t datum = decoder.getCode();

  if(datum)
  {
    Serial.print(datum, HEX); 
    Serial.print('\t');
    Serial.print((uint8_t)(datum >> 16));
    Serial.print('\n');
  }
}