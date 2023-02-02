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
int counter;
char* resp;

void setup() {
  radio_setup();
  pinMode(A0,INPUT);
}

void loop(void){
  itoa(analogRead(A0),transmit_msg,10);
  transmit(transmit_msg);
  if(recieve(1000)){
    printMsg();
  }
  counter++;
}

