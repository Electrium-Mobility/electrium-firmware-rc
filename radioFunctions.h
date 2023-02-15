//Made by: Winston Lu 
//Last updated: 2023/2/15
#ifndef RADIOECC_H
#define RADIOECC_H

//setup radio module
void radioSetup();

//setup radio module to pair
void radioSetupPair();

//switch from the pair address to the saved EEPROM address
void radioSetupSwitch();

//stop all read/write operations
void stopRadio();

//transmits RS encoded data
bool transmit(char* constmessage); 

//recieve ECC transmission. Returns 0 on success, 1 on timeout
int recieve(unsigned long timeout); 

//local msg variable getter
char* getMsg();

//only works if debug is defined
void printMsg();
void printMsg(int numBytes);


//helper functions
void _initNRF();
void _printHex(char val);
void _printHexChar(uint8_t val);

#endif