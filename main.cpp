#include <TM1637Display.h>

#define CLK 17
#define DIO 16
#define SWITCH_PIN 4

TM1637Display display(CLK, DIO);

enum State {
  IDLE,
  START_BLINK,
  RUNNING,
  HOLD
};

State state = IDLE;

unsigned long startTime = 0;
unsigned long holdStart = 0;
unsigned long blinkStart = 0;

unsigned long frozenTime = 0;

void setup() {

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  display.setBrightness(1);

  showZero();
}

void loop() {

  bool leverDown = digitalRead(SWITCH_PIN) == LOW;
  unsigned long now = millis();

  switch (state) {

    case IDLE:

      if (leverDown) {
        blinkStart = now;
        state = START_BLINK;
      }

      break;


    case START_BLINK:

      blinkAnimation(now);

      if (now - blinkStart > 1000) {
        startTime = millis();
        state = RUNNING;
      }

      break;


    case RUNNING:

      if (!leverDown) {

        frozenTime = now - startTime;

        holdStart = now;
        state = HOLD;
        break;
      }

      showTime(now - startTime);

      break;


    case HOLD:

      holdDisplay(now);

      if (now - holdStart > 5000) {
        showZero();
        state = IDLE;
      }

      break;
  }
}

void showZero() {

  display.showNumberDecEx(0, 0b01000000, true);
}

void blinkAnimation(unsigned long now) {

  if ((now / 200) % 2) {
    display.showNumberDecEx(0, 0b01000000, true);
  } else {
    display.clear();
  }
}

void showTime(unsigned long ms) {

  unsigned long seconds = ms / 1000;

  bool colon = (millis() / 500) % 2;

  if (seconds < 3600) {

    int minutes = seconds / 60;
    int secs = seconds % 60;

    int value = minutes * 100 + secs;

    display.showNumberDecEx(value, colon ? 0b01000000 : 0, true);

  } else {

    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;

    int value = hours * 100 + minutes;

    display.showNumberDecEx(value, colon ? 0b01000000 : 0, true);
  }
}

void holdDisplay(unsigned long now) {

  unsigned long elapsed = now - holdStart;

  if (elapsed < 3000) {

    // steady display first
    showTime(frozenTime);

  } else {

    // blink during last 2 seconds
    if ((now / 300) % 2) {
      showTime(frozenTime);
    } else {
      display.clear();
    }

  }
}
