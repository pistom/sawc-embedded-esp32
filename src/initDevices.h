#ifndef INIT_DEVICES_H
#define INIT_DEVICES_H
#include <LiquidCrystal_I2C.h>

void initDevices(LiquidCrystal_I2C &lcd);
void resetData(int eepromAddress, unsigned int tokenLength);
void disableLcdBacklightAfterTimeout(LiquidCrystal_I2C &lcd);

#endif