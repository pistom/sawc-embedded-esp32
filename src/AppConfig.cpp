#include "AppConfig.h"
#include <iostream>
#include <string>

int AppConfig::valvesGpioPins[AppConfig::valvesNumber] = { 23, 32, 33, 25, 26 };
bool AppConfig::outputTurnedOn = false;
int AppConfig::wateringDuration = -1;
int AppConfig::pumpDelayOff = 0;
std::string AppConfig::token = "";