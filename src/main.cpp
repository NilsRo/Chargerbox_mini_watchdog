#include <Arduino.h>
// const int PIN_STEPS = 1024.0;
const analog_pin_t PIN_CHARGER2 = A1;
const analog_pin_t PIN_BAT1 = A2;
const analog_pin_t PIN_BAT2 = A3;
const unsigned int PIN_LED = DD0;
const unsigned int PIN_SWITCH = DD1;

bool charger_on = false;

bool bat_connected(uint16_t ms) {
  // Check multiple times to avoid glitch
  int16_t val_akku1 = 0;
  int16_t val_akku2 = 0;
  uint32_t startTime;
  startTime = millis();
  while((millis() - startTime) < ms) {
    // 120Kohm / 10KkOhm -> 63V to 4,85V -> 993
    val_akku1 = max(val_akku1, analogRead(PIN_BAT1));
    val_akku2 = max(val_akku2, analogRead(PIN_BAT2));
    delayMicroseconds(20);
    wdt_reset();
  }
  return (val_akku1 > 0 or val_akku2 > 0); 
} 

bool vout_ok() {
  // Check multiple times to avoid glitch
  int16_t val_charger2 = 1023;
  for(int cnt = 0; cnt < 4; cnt++) {
    val_charger2 = min(val_charger2, analogRead(PIN_CHARGER2));
    delayMicroseconds(20);
  }
  // 18Kohm / 10KkOhm -> 12,8V to 4,57V -> 935
  return (val_charger2 > 925); 
}

void setup() {
  wdt_enable(WDTO_2S);
  analogReference(EXTERNAL);   

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SWITCH, OUTPUT);

  digitalWrite(PIN_LED, LOW);  
  digitalWrite(PIN_SWITCH, LOW);
}

void loop() {
  if (charger_on) {
    if (!vout_ok()) {
      digitalWrite(PIN_SWITCH, LOW);
      charger_on = false;
      //Block PSU until a complete restart
      while (true)
      {
        delay(200);
        digitalWrite(PIN_LED, LOW);
        wdt_reset();
        delay(200);
        digitalWrite(PIN_LED, HIGH);
        wdt_reset();
      }
    }  
  } else {
    if (!bat_connected(1000)) {
      digitalWrite(PIN_SWITCH, HIGH);
      charger_on = true;
      // wait for DC in regulation
      delay(500);
      digitalWrite(PIN_LED, HIGH);
    }
  } 
  wdt_reset();
}