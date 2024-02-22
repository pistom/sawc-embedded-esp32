#include "./tokenManager.h"
#include "./generateRandomString.h"
#include <EEPROM.h>


// Function to write a string to EEPROM
void writeStringToEEPROM(int addr, const std::string& data) {
  for (unsigned int i = 0; i < data.length(); i++) {
    EEPROM.write(addr + i, data[i]);
  }
  EEPROM.commit(); // Persist changes
}

// Function to read a string from EEPROM
std::string readStringFromEEPROM(int addr, unsigned int length) {
  std::string data = "";
  for (unsigned int i = 0; i < length; i++) {
    char character = EEPROM.read(addr + i);
    // Break loop if null character is encountered
    if (character == '\0') break;
    data += character;
  }
  return data;
}

// Function to remove a string from EEPROM
void removeStringFromEEPROM(int addr, unsigned int length) {
  for (unsigned int i = 0; i < length; i++) {
    EEPROM.write(addr + i, '\0'); // Overwrite with null characters
  }
  EEPROM.commit(); // Persist changes
}

bool isStringStored(int addr, unsigned int length) {
  for (unsigned int i = 0; i < length; i++) {
    char character = EEPROM.read(addr + i);
    if (character != '\0') {
      return true; // Non-null character found, string exists
    }
  }
  return false; // No non-null character found, no string exists
}