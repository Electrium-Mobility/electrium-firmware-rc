//Reference document: https://maniacbug.github.io/RF24/classRF24.html
//Made by: Winston Lu 
//Last updated: 2023/1/31

#include "HardwareSerial.h"
#include "settings.h"
#include "radioFunctions.h"
#include "RS-FEC.h"
#include "RF24.h"
#include <EEPROM.h>


RF24 radio(CE_PIN, CSN_PIN);
#ifdef ENABLE_ERROR_CORRECTION
RS::ReedSolomon<MAX_MESSAGE_LENGTH, ECC_LENGTH> rs;
#endif

// Addresses are a 5 byte field.
uint8_t r_address[5]; //address for reading
uint8_t w_address[5]; //address for writing
uint8_t pipe_channel = 1;
char msg[MAX_MESSAGE_LENGTH];

void radio_setup(){
  #ifdef DEBUG
  Serial.begin(115200); while (!Serial);
  #endif

  //get the radio transmit address (not channel, channel will be hard-coded in settings.h)
  getAddress();

  //NRF initialization
  radio.begin(); // Start up the radio
  radio.setDataRate(DATA_RATE); //we dont need much speed, prefer range
  radio.setPALevel(PA_LEVEL);
  radio.setAutoAck(1); // Enable auto-acknowledge
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openWritingPipe(w_address); 
  radio.openReadingPipe(pipe_channel, r_address); 

  //CRC will fail if theres a correctable error (and also incorrectable errors)
  //   so disable CRC and roll with any corrupt data hoping its correctable
  #ifdef ENABLE_ERROR_CORRECTION
  radio.disableCRC(); 
  #endif
  
  //If we defined a 2.4GHz channel, set it to that. By default I believe its 0 if we don't set it
  #ifdef CHANNEL
  radio.setChannel(CHANNEL);
  #endif
}

void transmit(char* message){
  //Stop listening and get ready to transmit
  radio.stopListening();

  //Store into global variable if needed to print out later
  #ifdef DEBUG
  memcpy(msg,message,MAX_MESSAGE_LENGTH);
  #endif

  //Encode message if error correction is enabled, otherwise just send
  #ifdef ENABLE_ERROR_CORRECTION
  char encoded[TOTAL_LENGTH];
  rs.Encode(message, encoded); 
  radio.write(encoded, TOTAL_LENGTH);
  #else
  radio.write(message, TOTAL_LENGTH);
  #endif

  //print out the hex message once sent
  #ifdef DEBUG
    Serial.print("Sent payload hex: ");
    for(size_t i=0;i<TOTAL_LENGTH;i++){
      #ifdef ENABLE_ERROR_CORRECTION 
      _print_hex(encoded[i]);
      #else 
      _print_hex(message[i]);
      #endif
      Serial.print(" ");
    }
    Serial.println();
  #endif
}

int recieve(unsigned long timeout){
  char buffer[TOTAL_LENGTH];
  unsigned long start = millis();
  radio.stopListening();
  radio.startListening();
  do{
    if(radio.available(&pipe_channel)){
      #ifdef ENABLE_ERROR_CORRECTION
      radio.read( buffer, TOTAL_LENGTH );
      rs.Decode(buffer, msg);
      #else 
      radio.read( msg, TOTAL_LENGTH );
      #endif

      #ifdef DEBUG
      Serial.print("Got Payload ");
      Serial.println(msg);
      #endif
      return 0;
    }
  } while (millis() < start + timeout);
  return 1;
}

void getAddress(){
  //EEPROM layout:               [if enabled]
  // isSet(0)    address(1..5)    channel(6) 
  //   0x0*    0x** ** ** ** **     0x**

  //Check if the EEPROM has a saved address
  byte isSet = EEPROM.read(0);
  //If there is a saved address, read it
  if (isSet){
    //Read the 5 address bytes
    for(byte i=0;i<5;i++)
      r_address[i] = EEPROM.read(i+1);
    
    //Check if the read address is stored correctly. Shouldn't really ever happen unless EEPROM dies
    #ifdef REMOTE  //read address LSB = 0, write address LSB = 1
    if(r_address[4] & 1 == 1){
      #ifdef DEBUG 
      Serial.println("Remote read address ends in 1, is EEPROM corrupted?");
      Serial.print("Address stored: ");
      for(size_t i=0;i<5;i++) Serial.print(r_address[i], HEX);
      Serial.println();
      #endif
      r_address[4] -= 1;
    }

    #elif defined(CONTROLLER) //read address LSB = 1, write address LSB = 0
    if(r_address[4] & 1 == 0){
      #ifdef DEBUG 
      Serial.println("Remote read address ends in 0, is EEPROM corrupted?");
      Serial.print("Address stored: ");
      for(size_t i=0;i<5;i++) Serial.print(r_address[i], HEX);
      Serial.println();
      #endif
      r_address[4] += 1;
    }
    #endif

  }

  //if EEPROM not set, just set r_address to the lowest unsigned value as a default
  else{
    memset(r_address, 0, 5);
    EEPROM.write(1, 0);
    EEPROM.write(2, 0);
    EEPROM.write(3, 0);
    EEPROM.write(4, 0);
    #ifdef REMOTE
    EEPROM.write(5, 0);
    #else
    EEPROM.write(5, 1);
    #endif
  }

  // set the write address
  memcpy(w_address,r_address,5);
  #ifdef REMOTE
  w_address[4] += 1;
  #else
  w_address[4] -= 1;
  #endif
}

char* getMsg(){
  return msg;
}

void printMsg(){
  #ifdef DEBUG
    Serial.print("Msg: ");
    for(size_t i=0;i<MAX_MESSAGE_LENGTH;i++){
      if(msg[i] == 0) break; //dont print out the square null bytes
      Serial.print(msg[i]);
    }
    Serial.println();
    Serial.print("Hex: ");
    for(size_t i=0;i<MAX_MESSAGE_LENGTH;i++) Serial.print(msg[i],HEX);
    Serial.println();
  #endif
}

void _print_hex(char val){
  //Since Serial.print(hex_val, HEX) doesnt seem to work for single bytes
  _print_hex_char(((uint8_t) val) >> 4);
  _print_hex_char(((uint8_t) val) % 16);
}

void _print_hex_char(uint8_t val){
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

