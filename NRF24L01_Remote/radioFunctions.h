//Made by: Winston Lu 
//Last updated: 2023/1/31

#ifndef RADIOECC_H
#define RADIOECC_H

//setup radio module
void radio_setup();

//transmits RS encoded data
void transmit(char* message); 

//recieve ECC transmission. Returns 0 on success, 1 on timeout
int recieve(unsigned long timeout); 

//local msg variable getter
char* getMsg();

//only works if debug is defined
void printMsg();

//helper functions
void _print_hex(char val);
void _print_hex_char(uint8_t val);

#endif