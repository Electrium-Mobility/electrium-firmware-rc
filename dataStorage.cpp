#include "settings.h"
#include "dataStorage.h"
#include <EEPROM.h>

//if joystick/throttle calibration is off by more than this amount, write to EEPROM
#define MAX_CALIBRATION_CHANGE 2

//EEPROM Layout
//     0                 1..5               6           7            8              9..10              11              12
//  uint8_t           uint8_t[5]         uint8_t      uint8_t     uint8_t         uint16_t           uint8_t         uint8_t
//  0bxxxxxxxx     0x00 00 00 00 00        0x00        0x00         0x00           0x00 00            0x00            0x00
// bitmask field     NRF address       max joystick   middle    min joystick  wheel diameter (mm)  max throttle     max brake
// MSB: NRF addr      (7)
//      joystick      (6)
//      diameter      (5)
//      min throttle  (4)
//      max throttle  (3)
//      0
//      1
// LSB: 0
#define ADDRESS_BIT 7
#define JOYSTICK_BIT 6
#define WHEEL_DIAMETER_BIT 5
#define MAX_THROTTLE_BIT 4
#define MAX_BRAKE_BIT 3

#define BITMASK_OFFSET 0
#define ADDRESS_OFFSET 1
#define JOYSTICK_MAX_OFFSET 6
#define JOYSTICK_MID_OFFSET 7
#define JOYSTICK_MIN_OFFSET 8
#define WHEEL_DIAMETER_OFFSET 9
#define MAX_THROTTLE_OFFSET 11
#define MAX_BRAKE_OFFSET 12

bool isInitialized(uint8_t offset){
  uint8_t data = EEPROM.read(BITMASK_OFFSET);
  uint8_t checkBits = data & 0b111;
  return (checkBits == 0b010) && (data & (1 << offset));
}

//sets the n-th bit of the isSet bitmask to 1
void writeMask(uint8_t offset){
  uint8_t data = EEPROM.read(BITMASK_OFFSET);
  uint8_t checkBits = data & 0b111;
  if(checkBits != 0b010)
    EEPROM.write(BITMASK_OFFSET, 0b010 | 1 << offset); //if not initialized, set it to initialize
  else if(data & (1 << offset) ){ //otherwise if bit isnt set, set it
    EEPROM.write(BITMASK_OFFSET,  data | 1 << offset);
  }
  //if bit is set already, dont do unecessary writes to EEPROM
}

uint8_t* getAddress(){
  uint8_t *addr = (uint8_t*) malloc(5);
  //If there is a saved address, read it
  if (isInitialized(ADDRESS_BIT)){
    //Read the 5 address bytes
    for(byte i=0;i<5;i++)
      addr[i] = EEPROM.read(ADDRESS_OFFSET + i);
  }
  //if EEPROM not set, just set r_address to the lowest read address (either 1 or 0) as a default
  else{
    memset(addr, 0, 5);
    #ifdef CONTROLLER
    addr[4] ^= 1;
    #endif
  }
  return addr;
}

//Save the read address
void saveAddress(uint8_t *addr){
  writeMask(ADDRESS_BIT); //set the address mask bit to 1
  EEPROM.write(ADDRESS_OFFSET  , addr[0]);
  EEPROM.write(ADDRESS_OFFSET+1, addr[1]);
  EEPROM.write(ADDRESS_OFFSET+2, addr[2]);
  EEPROM.write(ADDRESS_OFFSET+3, addr[3]);

  //save read address, force the bit convention
  #ifdef REMOTE  //read address LSB = 0, write address LSB = 1
    EEPROM.write(ADDRESS_OFFSET+4, addr[4] & 0b11111110);
  #elif defined(CONTROLLER) //read address LSB = 1, write address LSB = 0
    EEPROM.write(ADDRESS_OFFSET+4, addr[4] | 1);
  #endif
}

//no defaults, so we just give whatever is there
uint8_t getJoystickMax(){
  return (uint8_t) ( ( isInitialized(JOYSTICK_BIT) ) ?  EEPROM.read(JOYSTICK_MAX_OFFSET) : 255);
}

uint8_t getJoystickMid(){
  return (uint8_t) ( ( isInitialized(JOYSTICK_BIT) ) ?  EEPROM.read(JOYSTICK_MID_OFFSET) : 128);
}

uint8_t getJoystickMin(){
  return (uint8_t) ( ( isInitialized(JOYSTICK_BIT) ) ?  EEPROM.read(JOYSTICK_MIN_OFFSET) : 0);
}

void saveJoystickCalibration(uint8_t max, uint8_t mid, uint8_t min){
  writeMask(JOYSTICK_BIT); //set the joystick mask bit to 1
  uint8_t current_max, current_mid, current_min;
  current_max = getJoystickMax();
  current_mid = getJoystickMid();
  current_min = getJoystickMin();
  if(abs(current_max - max) > MAX_CALIBRATION_CHANGE) EEPROM.write(JOYSTICK_MAX_OFFSET, max);
  if(abs(current_mid - mid) > MAX_CALIBRATION_CHANGE) EEPROM.write(JOYSTICK_MID_OFFSET, mid);
  if(abs(current_min - min) > MAX_CALIBRATION_CHANGE) EEPROM.write(JOYSTICK_MIN_OFFSET, min);
}

uint16_t getWheelDiameter(){
  return (uint16_t) ( ( isInitialized(WHEEL_DIAMETER_BIT) ) ? (EEPROM.read(WHEEL_DIAMETER_OFFSET) << 8) + EEPROM.read(WHEEL_DIAMETER_OFFSET+1) : 90);
}

uint8_t getMaxThrottle(){
  return (uint8_t) ( ( isInitialized(MAX_THROTTLE_BIT) ) ? EEPROM.read(MAX_THROTTLE_OFFSET) : 100);
}

uint8_t getMaxBrake(){
  return (uint8_t) ( ( isInitialized(MAX_BRAKE_BIT) ) ? EEPROM.read(MAX_BRAKE_OFFSET) : 100);
}

void setWheelDiameter(uint16_t diameter_mm){
  writeMask(WHEEL_DIAMETER_BIT); //set the wheel diameter mask bit to 1
  //if diameter is the same, dont save
  if(diameter_mm == getWheelDiameter()) return; 
  //otherwise, save data
  EEPROM.write(WHEEL_DIAMETER_OFFSET  , (uint8_t)((diameter_mm >> 8) % 256));
  EEPROM.write(WHEEL_DIAMETER_OFFSET+1, (uint8_t)((diameter_mm     ) % 256));
}

void setMaxThrottle(uint8_t throttle_power){
  writeMask(MAX_THROTTLE_BIT); //set the max throttle mask bit to 1
  //if throttle is the same, dont save
  if(throttle_power == getMaxThrottle()) return; 
  //otherwise, save data
  EEPROM.write(MAX_THROTTLE_OFFSET, throttle_power);
}

void setMaxBrake(uint8_t brake_power){
  writeMask(MAX_BRAKE_BIT); //set the max brake mask bit to 1
  //if brake is the same, dont save
  if(brake_power == getMaxBrake()) return; 
  //otherwise, save data
  EEPROM.write(MAX_BRAKE_OFFSET, brake_power);
}
