//Made by: Winston Lu 
//Last updated: 2023/2/15

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

//main loop
// remote sends 7 bits representing a number from 0-128
// last bit will be direction, 0 being forward 1 being backward
//    likely have CRC or some EC/ED on it
// remote will send about ever 200ms, so about 5 updates per second
// while waiting, remote will listen for battery/controller updates
//    recieved information will be a 0-255 number containing battery levels

//Nano has 30720 bytes of flash
//          2048 bytes of ram
#include "radioFunctions.h"
#include "settings.h"
#include "pair.h"

char transmit_msg[MAX_MESSAGE_LENGTH];
char* resp;

void setup() {
  #ifdef CONTROLLER
    pairController();
  #elif defined(REMOTE)
    pinMode(A0,INPUT);
    if(analogRead(A0) <= 10){
      pairRemote();
    }
    else{
      radioSetup();
      #ifdef DEBUG
      Serial.begin(115200);
      #endif
    }
  #endif
}

void loop(void){
  #ifdef CONTROLLER
    if(recieve(100)){
      printMsg();
    }
  #elif defined(REMOTE)
    itoa(analogRead(A0),transmit_msg,10);
    transmit(transmit_msg);
    Serial.print("Analog value: ");
    Serial.println(analogRead(A0));
    if(recieve(3000)){
      printMsg();
    }
  #endif
}

