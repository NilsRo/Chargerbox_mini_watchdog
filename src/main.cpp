#include <Arduino.h>
// const int PIN_STEPS = 1024.0;
const analog_pin_t PIN_CHARGER2 = A1;
const analog_pin_t PIN_BAT1 = A2;
const analog_pin_t PIN_BAT2 = A3;
const unsigned int PIN_LED = DD0;
const unsigned int PIN_SWITCH = DD1;

bool charger_on = false;

bool check_akku() {
  // Check multiple times to avoid glitch
  int16_t val_akku1 = 0;
  int16_t val_akku2 = 0;
  for(int cnt = 0; cnt < 4; cnt++) {
    val_akku1 = max(val_akku1, analogRead(PIN_BAT1));
    val_akku2 = max(val_akku2, analogRead(PIN_BAT2));
  }
  return (val_akku1 > 0 or val_akku2 > 0); 
} 

bool check_charger() {
  // Check multiple times to avoid glitch
  int16_t val_charger2 = 0;
  for(int cnt = 0; cnt < 4; cnt++) {
    val_charger2 = min(val_charger2, analogRead(PIN_CHARGER2));
  }
  return (val_charger2 > 1000); 
}

void setup() {
  wdt_enable(WDTO_2S);

  pinMode(PIN_BAT1, INPUT);
  pinMode(PIN_BAT2, INPUT);
  pinMode(PIN_CHARGER2, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SWITCH, OUTPUT);

  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_SWITCH, LOW);
}

void loop() {
  if (!charger_on && !check_akku()) {
    digitalWrite(PIN_SWITCH, HIGH);
    digitalWrite(PIN_LED, LOW);
    charger_on = true;
    // wait for DC in regulation
    delay(500);
  }

  if (charger_on && !check_charger()) {
    digitalWrite(PIN_SWITCH, LOW);
    digitalWrite(PIN_LED, HIGH);
    charger_on = false;
    //Wait 2 seconds to let the psu stabilize
    delay(1000);
    wdt_reset();
    delay(1000);
  }
  wdt_reset();
}