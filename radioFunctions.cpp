//Reference document: https://maniacbug.github.io/RF24/classRF24.html
//Made by: Winston Lu 
//Last updated: 2023/2/15
#include "HardwareSerial.h"
#include "settings.h"
#include "radioFunctions.h"
#include "RF24.h"
#include "dataStorage.h"


RF24 radio(CE_PIN, CSN_PIN);

// Addresses are a 5 byte field.
uint8_t pipeChannel = PIPE_CHANNEL; //cant be const or #define cause they want an address pointer
uint8_t r_address[5]; //address for reading
uint8_t w_address[5]; //address for writing
char msg[MAX_MESSAGE_LENGTH];

void radioSetup(){
  //get the radio transmit address (not channel, channel will be hard-coded in settings.h)
  uint8_t *addr = getAddress();
  memcpy(r_address, addr, 5);
  memcpy(w_address, addr, 5);
  w_address[4] ^= 1; //flip the last bit
  free(addr);

  _initNRF();
}

void radioSetupPair(){
  uint8_t addr[5] = {'P','A','I','R', 0};
  #ifdef CONTROLLER
  addr[4] = 1;
  #endif
  memcpy(r_address, addr, 5);
  memcpy(w_address, addr, 5);
  w_address[4] ^= 1; //flip the last bit

  _initNRF();
}

void radioSetupSwitch(){
  uint8_t *addr = getAddress();
  memcpy(r_address, addr, 5);
  memcpy(w_address, addr, 5);
  w_address[4] ^= 1; //flip the last bit
  free(addr);
  #ifdef DEBUG
    Serial.print("Switching read address to: ");
    for(size_t i=0;i<5;i++){
      _printHex(r_address[i]);
      Serial.print(" ");
    }
    Serial.print("Switching write address to: ");
    for(size_t i=0;i<5;i++){
      _printHex(w_address[i]);
      Serial.print(" ");
    }
    Serial.println(" Switch done.");
  #endif

  //close the original reading pipe and open the new one stored in EEPROM, since
  //otherwise it would open another reading pipe, which other pair requests may interfere
  //  with regular operations of another controller if not closed
  radio.closeReadingPipe(pipeChannel);
  radio.openReadingPipe(pipeChannel, r_address); 

  //stop the write address and change the pipe to the new address. 
  //No need to close the pipe since there can only be 1 write pipe
  radio.stopListening();
  radio.openWritingPipe(w_address); 
}

void stopRadio(){
  radio.closeReadingPipe(PIPE_CHANNEL);
  radio.stopListening();
}

bool transmit(char* message){
  //Stop listening and get ready to transmit
  radio.stopListening();

  //Store into global variable if needed to print out later
  #ifdef DEBUG
  memcpy(msg,message,MAX_MESSAGE_LENGTH);
  #endif

  //print out the hex message once sent
  #ifdef DEBUG
    Serial.print("Sending to address: ");
    for(size_t i=0;i<5;i++){
      _printHex(w_address[i]);
      Serial.print(" ");
    }
    Serial.print(" payload: ");
    for(size_t i=0;i<MAX_MESSAGE_LENGTH;i++){
      _printHex(message[i]);
      Serial.print(" ");
    }
    Serial.println();
  #endif
  
  return radio.write(message, MAX_MESSAGE_LENGTH);
}

int recieve(unsigned long timeout){
  unsigned long start = millis();
  radio.stopListening();
  radio.startListening();
  do{
    if(radio.available(&pipeChannel)){
      radio.read( msg, MAX_MESSAGE_LENGTH );

      #ifdef DEBUG
      Serial.print("Recieved on address: ");
      for(size_t i=0;i<5;i++){
        _printHex(r_address[i]);
        Serial.print(" ");
      }
      Serial.print(" contents: ");
      for(size_t i=0;i<MAX_MESSAGE_LENGTH;i++){
        _printHex(msg[i]);
        Serial.print(" ");
      }
      Serial.println();
      #endif
      return 1;
    }
  } while (millis() < start + timeout);
  return 0;
}

char* getMsg(){
  return msg;
}

void printMsg(){
  #ifdef DEBUG
  printMsg(MAX_MESSAGE_LENGTH);
  #endif
}

void printMsg(int numBytes){
  #ifdef DEBUG
    Serial.print("Msg: ");
    for(int i=0;i<numBytes;i++){
      if(msg[i] == 0) break; //dont print out the square null bytes
      Serial.print(msg[i]);
    }
    Serial.println();
    Serial.print("Hex: ");
    for(int i=0;i<numBytes;i++){
      _printHex(msg[i]);
      Serial.print(" ");
    }
    Serial.println();
  #endif
}

void _initNRF(){
  //Debug Addresses
  #ifdef DEBUG
    Serial.begin(115200); while (!Serial);
    Serial.print("Setting read address to: ");
    for(size_t i=0;i<5;i++){
      _printHex(r_address[i]);
      Serial.print(" ");
    }
    Serial.print("Setting write address to: ");
    for(size_t i=0;i<5;i++){
      _printHex(w_address[i]);
      Serial.print(" ");
    }
    Serial.println(" Init done.");
  #endif

  //NRF initialization
  radio.begin(); // Start up the radio
  radio.setDataRate(DATA_RATE); //we dont need much speed, prefer range
  radio.setPALevel(PA_LEVEL);
  radio.setAutoAck(1); // Enable auto-acknowledge
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openWritingPipe(w_address); 
  radio.openReadingPipe(pipeChannel, r_address); 
  
  //If we defined a 2.4GHz channel, set it to that. By default I believe its 0 if we don't set it
  #ifdef CHANNEL
  radio.setChannel(CHANNEL);
  #endif

}

void _printHex(char val){
  //Since Serial.print(hex_val, HEX) doesnt seem to work for single bytes
  _printHexChar(((uint8_t) val) >> 4);
  _printHexChar(((uint8_t) val) % 16);
}

void _printHexChar(uint8_t val){
  switch(val){
    case 0:
      Serial.print("0"); break;
    case 1:
      Serial.print("1"); break;
    case 2:
      Serial.print("2"); break;
    case 3:
      Serial.print("3"); break;
    case 4:
      Serial.print("4"); break;
    case 5:
      Serial.print("5"); break;
    case 6:
      Serial.print("6"); break;
    case 7:
      Serial.print("7"); break;
    case 8:
      Serial.print("8"); break;
    case 9:
      Serial.print("9"); break;
    case 10:
      Serial.print("A"); break;
    case 11:
      Serial.print("B"); break;
    case 12:
      Serial.print("C"); break;
    case 13:
      Serial.print("D"); break;
    case 14:
      Serial.print("E"); break;
    case 15:
      Serial.print("F"); break;
    default:
      Serial.print("0d");
      Serial.print((int)val); 
      Serial.print(" ");
      break; //shouldn't happen
  }
}


