//Made by: Winston Lu 
//Last updated: 2023/1/31

#ifndef SETTINGS_H
#define SETTINGS_H

#include "RF24.h"

//Enable debug through serial. Controllers like the ATTiny have no hardware serial
#define DEBUG

// NRF pins
#define CE_PIN 9
#define CSN_PIN 8 

//Transmit level. 
// Options: RF24_PA_MIN | RF24_PA_LOW | RF24_PA_MED | RF24_PA_HIGH
//            -18dBm    |   -12dBm    |    -6dBm    |     0dBm
#define PA_LEVEL RF24_PA_HIGH

// 2.4GHz channel (0-125). Define to enable channels.
//    Not really needed unless working in places with a lot of 2.4GHz signals
//#define CHANNEL 1

//Which device is this on?
#define CONTROLLER
// #define REMOTE

/****************************************************************************/
/**** THE BELOW SETTINGS SHOULD BE THE SAME ON THE CONTROLLER AND REMOTE ****/
/****************************************************************************/

// Have error correction or just do error detection and retransmit
#define ENABLE_ERROR_CORRECTION

// Error correction and message settings. 
#define MAX_MESSAGE_LENGTH 6      //min 5 if CHANNEL is disabled, 6 if CHANNEL is enabled
#define ECC_LENGTH 4              //max correction: ECC_LENGTH/2



// Data transfer speeds
// Options: RF24_250KBPS | RF24_1MBPS | RF24_2MBPS
#define DATA_RATE RF24_250KBPS


//Dont touch the code below, they're just calculations and sanity checks to check if we 
//    have a invalid configuration
#ifdef ENABLE_ERROR_CORRECTION
  #define TOTAL_LENGTH MAX_MESSAGE_LENGTH + ECC_LENGTH
#else 
  #define TOTAL_LENGTH MAX_MESSAGE_LENGTH
#endif

#if !defined(REMOTE) && !defined(CONTROLLER)
  #error "You must specify if the radio module is attached to the controller or the remote"
#endif
#if defined(REMOTE) && defined(CONTROLLER)
  #error "You must specify if the radio module is attached to the controller or the remote, not both"
#endif

#ifdef CHANNEL
  #if MAX_MESSAGE_LENGTH < 6
  #error "MAX_MESSAGE_LENGTH must be at least 6"
  #endif
#else 
  #if MAX_MESSAGE_LENGTH < 5
  #error "MAX_MESSAGE_LENGTH must be at least 5"
  #endif
#endif


#endif