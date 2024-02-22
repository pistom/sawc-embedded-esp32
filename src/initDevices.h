#ifndef INIT_DEVICES_H
#define INIT_DEVICES_H

void initDevices();
void resetData(int eepromAddress, unsigned int tokenLength);
void disableLcdBacklight(int interval);

#endif