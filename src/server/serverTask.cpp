#include <SSLCert.hpp>
#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include "HTTPURLEncodedBodyParser.hpp"
#include "../cert/cert.h"
#include "../cert/private_key.h"
#include "./getParamsFromRequest.h"
#include "../AppConfig.h"
#include "../devices/lcd.h"

using namespace httpsserver;
using namespace std;

// Create an SSL certificate object from the files included above
SSLCert cert = SSLCert(
  example_crt_DER, example_crt_DER_len,
  example_key_DER, example_key_DER_len
);

HTTPSServer secureServer = HTTPSServer(&cert, 443, 1);

void handleRoot(HTTPRequest * req, HTTPResponse * res);
void handle404(HTTPRequest * req, HTTPResponse * res);
void handleOutput(HTTPRequest * req, HTTPResponse * res);

void serverTask(void *params) {
  ResourceNode * nodeRoot    = new ResourceNode("/", "GET", &handleRoot);
  ResourceNode * node404     = new ResourceNode("", "GET", &handle404);
  ResourceNode * nodeOutput  = new ResourceNode("/output", "POST", &handleOutput);

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
  res->println("SAWC - WiFi Module");
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
  auto params = getParamsFromRequest(req);

  string token = get<0>(params);
  if (AppConfig::token != token) {
    res->setStatusCode(401);
    res->setStatusText("Unauthorized");
    res->print("{\"error\": \"invalid token\"}");
    return;
  }
  string action = get<1>(params);
  int outputNumber = get<2>(params);

  
  if (outputNumber == -1) {
    res->setStatusCode(404);
    res->setStatusText("Output not Found");
    res->print("{");
    res->print("\"output\": \"");
    res->print(outputNumber);
    res->print("\",");
    res->print("\"error\": \"No output found\"");
    res->print("}");
  } 
  // Manage valves
  else if (outputNumber > 0) {
    int nextOutputNumber = get<3>(params);
    AppConfig::wateringDuration = get<4>(params);
    AppConfig::pumpDelayOff = get<5>(params);

    if (action == "on") {
      AppConfig::outputTurnedOn = true;
      digitalWrite(AppConfig::valvesGpioPins[outputNumber], HIGH);
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Watering...");
      lcd.setCursor(0, 1);
      lcd.print("Valve: ");
      lcd.print(outputNumber);
    }
    else if (action == "off") {
      digitalWrite(AppConfig::valvesGpioPins[outputNumber], LOW);
      if (nextOutputNumber > 0) {
        digitalWrite(AppConfig::valvesGpioPins[nextOutputNumber], HIGH);
        lcd.clear();
        lcd.backlight();
        outputNumber = nextOutputNumber;
        AppConfig::wateringDuration = 3;
        nextOutputNumber = -1;
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Done");
        AppConfig::wateringDuration = -1;
        AppConfig::backlightTimeout = 10;
        AppConfig::outputTurnedOn = false;
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
  // Manage pump
  else if (outputNumber == 0) { 
    if (action == "on" && AppConfig::outputTurnedOn == true) {
      digitalWrite(AppConfig::valvesGpioPins[0], HIGH);
    } else if (action == "off") {
      digitalWrite(AppConfig::valvesGpioPins[0], LOW);
      AppConfig::pumpDelayOff = 0;
    }
    res->print("{");
    res->print("\"output\": \"pump\"");
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