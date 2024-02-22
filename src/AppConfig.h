#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <string>

class AppConfig
{
public:
    static std::string token;
    static const int valvesNumber = 5;
    static int valvesGpioPins[valvesNumber]; // First value is pump pin number
    static bool outputTurnedOn;
    static int wateringDuration;
    static int pumpDelayOff;
};

#endif
