#include <FS.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h>
#include <iostream>
#include <random>
#include <string>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "./cert/cert.h"
#include "./cert/private_key.h"

#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include "HTTPURLEncodedBodyParser.hpp"

using namespace httpsserver;
using namespace std;

string token = "";
string action = "";
int outputNumber = -1;
int nextOutputNumber = -1;
int duration = -1;
int pumpDelayOff = 0;
bool outputTurnedOn = false;

// Create an SSL certificate object from the files included above
SSLCert cert = SSLCert(
  example_crt_DER, example_crt_DER_len,
  example_key_DER, example_key_DER_len
);

HTTPSServer secureServer = HTTPSServer(&cert, 443, 1);

void handleRoot(HTTPRequest * req, HTTPResponse * res);
void handle404(HTTPRequest * req, HTTPResponse * res);
void handleOutput(HTTPRequest * req, HTTPResponse * res);
void serverTask(void *params);
void getParamsFromRequest(HTTPRequest * req);

#define I2C_ADDR 0x3F
#define LCD_COLUMNS 16
#define LCD_ROWS 2

std::string generateRandomString(int length);
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

// Pins map (the first element is the pump output pin)
const int valves[] = {23, 32, 33, 25, 26 };

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

  Serial.println("Creating server task... ");
  xTaskCreatePinnedToCore(serverTask, "https443", 6144, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}

void loop() {
  delay(1000);
  // Automatic turn off the pump after watering time
  if (outputTurnedOn && duration < 0 - pumpDelayOff) {
      for(int i : valves) {
        digitalWrite(i, LOW);
      }
  } else {
    pumpDelayOff -= 1;
  }
}

std::string generateRandomString(int length) {
    std::random_device rd;
    std::mt19937 generator(rd());
    const std::string characters = "abcdefghijkmnpqrstuvwxyz23456789";
    std::uniform_int_distribution<int> distribution(0, characters.size() - 1);
    std::string randomString;
    for (int i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }
    return randomString;
}

void serverTask(void *params) {
  ResourceNode * nodeRoot    = new ResourceNode("/", "GET", &handleRoot);
  ResourceNode * node404     = new ResourceNode("", "GET", &handle404);
  ResourceNode * nodeOutput      = new ResourceNode("/output", "POST", &handleOutput);

  secureServer.registerNode(nodeRoot);
  secureServer.registerNode(nodeOutput);
  secureServer.setDefaultNode(node404);

  Serial.println("Starting server...");
  secureServer.start();
  if (secureServer.isRunning()) {
    Serial.println("Server ready.");
    
    while(true) {
      secureServer.loop();
      delay(10);
    }
  }
}

void handleRoot(HTTPRequest * req, HTTPResponse * res) {
  res->setHeader("Content-Type", "text/html");

  res->print("Your server is running for ");
  res->print((int)(millis()/1000), DEC);
  res->println(" seconds.");
}

void handle404(HTTPRequest * req, HTTPResponse * res) {
  req->discardRequestBody();
  res->setStatusCode(404);
  res->setStatusText("Not Found");
  res->setHeader("Content-Type", "text/html");
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Not Found</title></head>");
  res->println("<body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body>");
  res->println("</html>");
}

void handleOutput(HTTPRequest * req, HTTPResponse * res) {
  res->setHeader("Content-Type", "application/json");
  getParamsFromRequest(req);
  if (outputNumber == -1) {
    res->setStatusCode(404);
    res->setStatusText("Output not Found");
    res->print("{");
    res->print("\"output\": \"");
    res->print(outputNumber);
    res->print("\",");
    res->print("\"error\": \"No output found\"");
    res->print("}");
  } else { 
    pinMode(valves[outputNumber], OUTPUT);
    if (action == "on") {
      if (outputNumber > 0) {
        outputTurnedOn = true;
        digitalWrite(valves[outputNumber], HIGH);
      } else if(outputTurnedOn == true) {
        digitalWrite(valves[0], HIGH);
      }
    } else if (action == "off") {
      digitalWrite(valves[outputNumber], LOW);
      if (nextOutputNumber > 0) {
        digitalWrite(valves[nextOutputNumber], HIGH);
        outputNumber = nextOutputNumber;
        nextOutputNumber = -1;
      }
      if (outputNumber > 0) {
        outputTurnedOn = false;
      }
    }
    res->print("{");
    res->print("\"output\": \"");
    res->print(outputNumber);
    res->print("\",");
    res->print("\"action\": \"");
    if (action == "on") {
      res->print("on");
    } else if (action == "off") {
      res->print("off");
    }
    res->print("\"");
    res->print("}");
  }
}

void getParamsFromRequest(HTTPRequest * req) {
  HTTPURLEncodedBodyParser parser(req);
  string output = "";
  string nextOutput = "";

  while(parser.nextField()) {
    string name = parser.getFieldName();
    char buf[512];
    size_t readLength = parser.read((byte *)buf, 512);
    if (name == "token") {
      token = string(buf, readLength);
    } else if (name == "output") {
      output = string(buf, readLength);
      if (output == "pump") {
        outputNumber = 0;
      } else if (stoi(output) != 0) {
        outputNumber = stoi(output);
        if (outputNumber > (sizeof(valves)/sizeof(valves[0])) || outputNumber < 0) {
          outputNumber = -1;
        }
      }
    } else if (name == "nextOutput") {
      nextOutput = string(buf, readLength);
      if (stoi(nextOutput) > 0) {
        nextOutputNumber = stoi(nextOutput);
        if (nextOutputNumber > (sizeof(valves)/sizeof(valves[0])) || nextOutputNumber < 0) {
          nextOutputNumber = -1;
        }
      }
    } else if (name == "action") {
      action = string(buf, readLength);
      if (action != "on" && action != "off") {
        action = "";
      }
    } else if (name == "duration") {
      duration = stoi(string(buf, readLength));
    } else if (name == "pumpDelayOff") {
      pumpDelayOff = stoi(string(buf, readLength)) + 1;
    }
  }
}