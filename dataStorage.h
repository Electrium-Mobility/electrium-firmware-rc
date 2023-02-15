//Made by: Winston Lu 
//Last updated: 2023/2/15
#ifndef DATASTORAGE_H
#define DATASTORAGE_H

//check if EEPROM has been written to
bool isInitialized(uint8_t offset);

//sets the n-th bit of the isSet bitmask to 1
void writeMask(uint8_t offset);

//gets address from EEPROM, otherwise sets them to default
uint8_t* getAddress();

//sets the address in EEPROM
void saveAddress(uint8_t *addr);

//Throttle data
uint8_t getJoystickMax();
uint8_t getJoystickMid();
uint8_t getJoystickMin();

void saveJoystickCalibration(uint8_t max, uint8_t mid, uint8_t min);

//Settings data
uint16_t getWheelDiameter();
uint8_t getMaxThrottle();
uint8_t getMaxBrake();

void setWheelDiameter(uint16_t diameter_mm);
void setMaxThrottle(uint8_t throttle_power);
void setMaxBrake(uint8_t brake_power);

#endif