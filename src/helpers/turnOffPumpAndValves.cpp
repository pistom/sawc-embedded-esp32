#include <Arduino.h>
#include "../AppConfig.h"

void turnOffPumpAndValves()
{
  if (AppConfig::outputTurnedOn)
  {
    if (AppConfig::wateringDuration < 0 - AppConfig::pumpDelayOff - 5)
    {
      for (int i : AppConfig::valvesGpioPins)
      {
        digitalWrite(i, LOW);
      }
      AppConfig::outputTurnedOn = false;
      AppConfig::wateringDuration = -1;
      AppConfig::pumpDelayOff = 0;
    }

    else
    {
      AppConfig::pumpDelayOff -= 1;
    }
  }
}