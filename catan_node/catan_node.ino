#import "components.h"

SegmentDisplay tileValue(2,3,4,5,6);

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  Serial.begin(9600);

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect)
{
  unsigned long currentMillis = millis();
  tileValue.render(currentMillis);
}

void loop()
{
  for (int i = 0; i < 0x100; i++) {
    char buf[3] = {0};
    sprintf(buf, "%02x", i);
    Serial.println(buf);

    tileValue.setChars(buf);
    
    delay(500);
  }
}

// This method sends bits to the shift register:
