//Made by: Winston Lu 
//Last updated: 2023/1/31

// Controller: RF module on the main device (ESC)
// Remote: Handheld RF module 

//IO settings:
//Power switch and function button
//  Power on with function down
//    Enter config screen, current pot setting set as middle with +-10% deadzone
//    Throttle moves up/down in setting, doesnt move again till throttle is reset
//    Press config button again to select option to change, press again to exit
//    Options:
//      Pair
//      Configure throttle range (sets high, middle, low)
//      Configure wheel diameter
//      Max throttle percentage
//      Max brake percentage
//Power switch and no function button
//   Regular operation
//   If no response after 10 seconds, vibrate to indicate no connection
//   to reverse, hold function button, will vibrate to indicate switch
//   If no battery level response from controller after 5 retries, start vibrate to tell signal lost

//pair feature:
// set to address: 0 p a i r    which is    0x 00 50 41 49 52
// controller randomly generates new address
// send address to remote
// controller -> remote transmission uses base address with the LSB=0
// remote -> controller transmission uses base address with the LSB=1
// both switch over if recieved, otherwise retry 10 times
// if fail after 10 times, default to 0x 00 00 00 00 00

//main loop
// remote sends 7 bits representing a number from 0-128
// last bit will be direction, 0 being forward 1 being backward
//    likely have CRC or some EC/ED on it
// remote will send about ever 200ms, so about 5 updates per second
// while waiting, remote will listen for battery/controller updates
//    recieved information will be a 0-255 number containing battery levels
#include "radioFunctions.h"
#include "settings.h"

char transmit_msg[MAX_MESSAGE_LENGTH];
char* resp;

void setup() {
  radio_setup();
  pinMode(A0,INPUT);
}

void loop(void){
  itoa(analogRead(A0),transmit_msg,10);
  transmit(transmit_msg);
  if(recieve(100)){
    printMsg();
  }
}

