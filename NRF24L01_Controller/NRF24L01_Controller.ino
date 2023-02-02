//Made by: Winston Lu 
//Last updated: 2023/1/31

// Controller: RF module on the main device (ESC)
// Remote: Handheld RF module 

//pair feature:
// set to address: 0 p a i r    which is    0x 00 50 41 49 52
// controller randomly generates new address
// send address to remote
// controller -> remote transmission uses base address with the LSB=0
// remote -> controller transmission uses base address with the LSB=1
// both switch over if recieved, otherwise retry 10 times
// if fail after 10 times, default to 0x 00 00 00 00 00

#include "radioFunctions.h"
#include "settings.h"

char transmit_msg[MAX_MESSAGE_LENGTH];
int counter;
char* resp;

void setup() {
  radio_setup();
}

void loop(void){
  // itoa(counter,transmit_msg,10);
  // transmit(transmit_msg);
  if(recieve(1000)){
    printMsg();
  }
  counter++;
}

