#include "NKeyboard.h"

// Connect a button between pin 0 and GND (same wiring as the original example).
const uint8_t BUTTON_PIN = 0;
const char HELD_KEYS[] = "abcdefgh";
const unsigned long DEBOUNCE_MS = 20;

static_assert(sizeof(HELD_KEYS) - 1 <= NKEYBOARD_MAX_KEYS,
              "Increase NKEYBOARD_MAX_KEYS or reduce HELD_KEYS");

bool lastReading = false;
bool stablePressed = false;
unsigned long lastChange = 0;

void setup()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Keyboard.begin();
  Keyboard.releaseAll();
}

void loop()
{
  const unsigned long now = millis();
  const bool reading = digitalRead(BUTTON_PIN) == LOW;

  if (reading != lastReading) {
    lastChange = now;
    lastReading = reading;
  }

  if (now - lastChange < DEBOUNCE_MS || reading == stablePressed) {
    return;
  }

  stablePressed = reading;
  if (stablePressed) {
    // Send each key once, keeping earlier keys held.
    for (size_t i = 0; i < sizeof(HELD_KEYS) - 1; ++i) {
      Keyboard.press(HELD_KEYS[i]);
    }
  } else {
    Keyboard.releaseAll();
  }
}
