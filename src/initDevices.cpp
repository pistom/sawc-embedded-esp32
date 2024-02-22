#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h>
#include "./helpers/generateRandomString.h"
#include "./helpers/tokenManager.h"
#include "./AppConfig.h"

#define I2C_ADDR 0x3F
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);
WiFiManager wifiManager;
unsigned long previousMillis = 0;

void initDevices() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Watering Can");
  lcd.setCursor(0, 1);
  lcd.print("Hello :)");
  delay(2000);

  wifiManager.setBreakAfterConfig(true);

  std::string ap_name = "SAWC " + generateRandomString(3);
  std::string ap_pwd = generateRandomString(8);

  lcd.clear();
  lcd.setContrast(255);
  lcd.setCursor(0, 0);
  lcd.print(("AP: " + ap_name).c_str());
  lcd.setCursor(0, 1);
  lcd.print(("PWD: " + ap_pwd).c_str());

  if (!wifiManager.autoConnect(ap_name.c_str(), ap_pwd.c_str())) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cant connect :(");
    lcd.setCursor(0, 1);
    lcd.print("Restarting...");
    delay(3000);
    ESP.restart();
    delay(5000);
  }

  lcd.clear();
  lcd.print(WiFi.localIP().toString().c_str());
  lcd.setCursor(0, 0);
  lcd.setCursor(0, 1);
  lcd.print("Token: ");
  lcd.print(AppConfig::token.c_str());
}

void resetData(int eepromAddress, unsigned int tokenLength) {
  removeStringFromEEPROM(eepromAddress, tokenLength);
  wifiManager.resetSettings();
  ESP.restart();
}

void disableLcdBacklight(int interval) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    delay(1000);
    lcd.noBacklight();
  }
}