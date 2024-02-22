#ifndef TOKEN_MANAGER_H
#define TOKEN_MANAGER_H

#include <string>

void writeStringToEEPROM(int addr, const std::string& data);
std::string readStringFromEEPROM(int addr, unsigned int length);
void removeStringFromEEPROM(int addr, unsigned int length);
bool isStringStored(int addr, unsigned int length);

#endif