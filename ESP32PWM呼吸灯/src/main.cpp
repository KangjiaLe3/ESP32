#include <Arduino.h>

#define FREQ 2000
#define CHANNEL 0
#define RES 8
#define LED 12

void setup()
 {
  ledcSetup(CHANNEL,FREQ,RES);
  ledcAttachPin(LED,CHANNEL);
}

void loop() 
{
  uint16_t i;
  for(i=0;i<pow(2,RES);i++)
  {
    ledcWrite(CHANNEL,i);
    delay(10);

  }
    for(i=255;i>0;i--)
  {
    ledcWrite(CHANNEL,i);
    delay(10);
    
  }

}

