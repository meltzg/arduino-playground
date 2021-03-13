#include <Components.h>

#define SEGMENT_LATCH A0
#define SEGMENT_CLOCK A1
#define SEGMENT_DATA A2
#define SEGMENT_LEFT A3
#define SEGMENT_RIGHT A4

#define LED_ARRAY 7
#define NUM_LEDS 11

#define NUM_BUTTONS 10
#define BTN_LOAD 10
#define BTN_CLK_ENABLE 11
#define BTN_DATA 12
#define BTN_CLOCK 13

SegmentDisplay disp(
    SEGMENT_LATCH,
    SEGMENT_CLOCK,
    SEGMENT_DATA,
    SEGMENT_LEFT,
    SEGMENT_RIGHT,
    500);

LEDStatusDisplay leds(LED_ARRAY, NUM_LEDS);

ButtonArray16 btns(
    BTN_LOAD,
    BTN_CLK_ENABLE,
    BTN_DATA,
    BTN_CLOCK);

int colorOffset = 0;

void setup() {
  Serial.begin(9600);
  disp.setChars("Hello World! ");

  __int24 ledColors[NUM_LEDS] = {BLACK};
  leds.setState(ledColors, 50);

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect)
{
  unsigned long currentMillis = millis();
  disp.render(currentMillis);
  leds.render(currentMillis);
  btns.render(currentMillis);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t btnState = btns.getState();
  __int24 ledColors[NUM_LEDS] = {BLACK};
  for (int i = 0; i < NUM_LEDS; i++) {
    if (((1 << i) & btnState) > 0) {
      continue;
    }
    ledColors[i] = RAINBOW[(i + colorOffset) % 6];
  }
  colorOffset++;
  leds.setState(ledColors, 50);
  delay(100);
}
