#include "settings.h"
#ifdef CONTROLLER
#include "pair.h"
#include "dataStorage.h"
#include "radioFunctions.h"

//Controller pairing to the remote
//Works only if the radio setup has not been done yet
void pairController(){
    #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Pairing...");
    #endif
    
    radioSetupPair();
    unsigned long start = millis();
    while (millis() <= start + PAIR_WAIT){
        //see if theres a pair request
        if(recieve(PAIR_WAIT)){
            #ifdef DEBUG
            Serial.println("Controller got message: ");
            printMsg(5);
            #endif
            uint8_t* msg = (uint8_t*) getMsg();
            //pair request will be 0xff xx xx xx xx. Use the rest
            //  of the bytes as a seed, and xor with the start time
            //  just to add a bit more entropy
            if(msg[0] == 0xff){
                uint8_t newAddr[5];
                //generate random seed
                randomSeed( ((uint32_t) msg[1] << 24) + 
                            ((uint32_t) msg[2] << 16) + 
                            ((uint32_t) msg[3] << 8)  + 
                                        msg[4]         );
                for(uint8_t i=0 ; i < 5 ; i++)
                    newAddr[i] = random(256) ^ (start % 255);

                delay(100); //wait for the remote to start listening
                //if successfully sends new address, save it
                bool saved = false;
                for(int i=0 ; i < PAIR_RETRIES ; i++){
                  if (transmit((char*) newAddr)){
                      #ifdef DEBUG
                      Serial.print("Controller set address to: ");
                      printMsg(5);
                      #endif

                      saveAddress(newAddr);
                      saved = true;
                      break;
                  }
                  else{
                    Serial.println("Failed pairing, retrying...");
                    delay(100);
                    continue; //otherwise retry
                  }
                }
                if(saved) break;
            }
            else continue; //if regular message or bad message, just keep listening
        }
    } 
    #ifdef DEBUG
    Serial.println("Resuming normal operations");
    #endif
    //once time is up, wheter paired or not, switch to regular operation
    radioSetupSwitch();
}
#endif