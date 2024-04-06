#include <Arduino.h>
#include "../AppConfig.h"
#include "../devices/lcd.h"

void AutomaticTurnOffPumpAndValvesIfNoStopWaterRequest() {
  if (AppConfig::outputTurnedOn) {
    Serial.print("Duration: ");
    Serial.println(AppConfig::wateringDuration);
    Serial.print("Delay: ");
    Serial.println(AppConfig::pumpDelayOff);
    Serial.println("################");
    if (AppConfig::wateringDuration < (-2 - AppConfig::pumpDelayOff)) {
      for (int i : AppConfig::valvesGpioPins) {
        digitalWrite(i, LOW);
      }
      AppConfig::outputTurnedOn = false;
      AppConfig::wateringDuration = -1;
      AppConfig::pumpDelayOff = 0;
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("E: Automatic");
      lcd.setCursor(0, 1);
      lcd.print("shutdown");
      for (int i = 0; i <=8; i++) {
        lcd.noBacklight();
        delay(500);
        lcd.backlight();
        delay(500);
      }
    }
    else {
      AppConfig::wateringDuration--;
    }
  }
}