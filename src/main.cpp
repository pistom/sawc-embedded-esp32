#include <FS.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h>
#include <iostream>
#include <random>
#include <string>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
using namespace std;
// using System;

#define I2C_ADDR 0x3F
#define LCD_COLUMNS 16
#define LCD_ROWS 2

std::string generateRandomString(int length);
int getOutputFromRequestParam(AsyncWebServerRequest *request);

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);
AsyncWebServer server(80);

// Pins map (first element is a pump output pin)
const int pump = 23;
const int valves[] = { 32, 33, 25, 26 };

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Watering Can");
  lcd.setCursor(0, 1);
  lcd.print("Hello :)");
  delay(2000);

  WiFiManager wifiManager;
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
  lcd.print("Token: test1234");

  // Handle turning on outputs
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    int outputNumber = getOutputFromRequestParam(request);
    if (outputNumber > (sizeof(valves)/sizeof(valves[0] - 1)) || outputNumber < 0) {
      request->send(200, "text/plain", "No output found");
    } else if (outputNumber == 0) {
      pinMode(pump, OUTPUT);
      digitalWrite(pump, HIGH);
      request->send(200, "text/plain", "Pump turned on");
    } else {
      pinMode(valves[outputNumber-1], OUTPUT);
      digitalWrite(valves[outputNumber-1], HIGH);
      request->send(200, "text/plain", "Output turned on");
    }
  });

  // Handle turning off outputs
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    int outputNumber = getOutputFromRequestParam(request);
    if (outputNumber > (sizeof(valves)/sizeof(valves[0] - 1)) || outputNumber < 0) {
      request->send(200, "text/plain", "No output found");
    } else if (outputNumber == 0) {
      pinMode(pump, OUTPUT);
      digitalWrite(pump, LOW);
      request->send(200, "text/plain", "Pump turned off");
    } else {
      pinMode(valves[outputNumber-1], OUTPUT);
      digitalWrite(valves[outputNumber-1], LOW);
      request->send(200, "text/plain", "Output turned off");
    }
  });

  // Start server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}

std::string generateRandomString(int length) {
    // Set up a random number generator
    std::random_device rd;
    std::mt19937 generator(rd());

    // Define characters allowed in the random string
    const std::string characters = "abcdefghijkmnpqrstuvwxyz23456789";

    // Create a distribution for selecting characters
    std::uniform_int_distribution<int> distribution(0, characters.size() - 1);

    // Generate the random string
    std::string randomString;
    for (int i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }

    return randomString;
}

int getOutputFromRequestParam(AsyncWebServerRequest *request) {
    int outputNumber = -1;
    if (request->hasParam("output") && request->getParam("output")->value().toInt() != 0) {
      outputNumber = request->getParam("output")->value().toInt();
    }
    if (request->hasParam("pump")) {
      outputNumber = 0;
    }
    return outputNumber;
}
