#include "HardwareSerial.h"
#include "settings.h"
#ifdef REMOTE
#include "pair.h"
#include "dataStorage.h"
#include "radioFunctions.h"

//Controller pairing to the remote
void pairRemote(){
    #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Starting the pairing process...");
    #endif
    radioSetupPair();
    unsigned long start = millis();
    randomSeed(start);
    uint8_t *msg = (uint8_t*) malloc(5);
    memset(msg, 0, 5);
    msg[0] = 0xff;
    while (millis() <= start + PAIR_DURATION){
        //generate a new address each try
        for(uint8_t i=1;i<5;i++)
            msg[i] = random(256);
        //if we successfully transmit and recieve a response within 500ms.
        //  we save the address
        transmit((char*) msg);

        if(recieve(5000)){
            #ifdef DEBUG
            Serial.print("Remote set address to: ");
            printMsg(5);
            #endif
            Serial.println("Saving address...");
            saveAddress((uint8_t*) getMsg());
            break;
        }
    } 
    free(msg);
    //once time is up, wheter paired or not, switch to regular operation
    radioSetupSwitch();
}
#endif