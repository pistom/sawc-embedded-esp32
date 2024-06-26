#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include <Arduino.h>
#include <EEPROM.h>
#include <string>
#include "./initDevices.h"
#include "./server/serverTask.h"
#include "./helpers/turnOffPumpAndValves.h"
#include "./helpers/tokenManager.h"
#include "./helpers/generateRandomString.h"
#include "./AppConfig.h"
#include "./devices/lcd.h"

#define EEPROM_SIZE 4096
#define EEPROM_ADDR_START 0
unsigned int TOKEN_LENGTH = 8;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  EEPROM.begin(EEPROM_SIZE);

  // Read token from memory or generate one if not exists
  if (!isStringStored(EEPROM_ADDR_START, TOKEN_LENGTH))
  {
    writeStringToEEPROM(EEPROM_ADDR_START, generateRandomString(TOKEN_LENGTH));
  }
  AppConfig::token = readStringFromEEPROM(EEPROM_ADDR_START, TOKEN_LENGTH);

  initDevices(lcd);

  // Initialize the GPIO pins
  for (int outputNumber = 0; outputNumber < AppConfig::valvesNumber; outputNumber++) {
    pinMode(AppConfig::valvesGpioPins[outputNumber], OUTPUT);
  }

  Serial.println("Creating server task... ");
  xTaskCreatePinnedToCore(serverTask, "https443", 6144, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
  AppConfig::backlightTimeout = 60;
}

void loop()
{
  delay(1000);

  disableLcdBacklightAfterTimeout(lcd);
  AutomaticTurnOffPumpAndValvesIfNoStopWaterRequest();

  // Reset the device to factory settings
  if (digitalRead(0) == LOW)
  {
    Serial.println("Factory reset initiated...");
    resetData(EEPROM_ADDR_START, TOKEN_LENGTH);
  }
}