//Made by: Winston Lu 
//Last updated: 2023/2/15
#ifndef SETTINGS_H
#define SETTINGS_H

#include "RF24.h"

//------------- Which device is this on?  ------------------------//
//-- On auto set, the Arduino Nano/Pi Pico will be the REMOTE.  --//
//--         Otherwise it will default to controller            --//
//----------------------------------------------------------------//
#define AUTO_SET
// #define CONTROLLER
// #define REMOTE
//--------------------------------------------------------//

//Enable debug through serial
#define DEBUG

// NRF pins
#define CE_PIN 17
#define CSN_PIN 14

//Transmit level. Higher = more power usage
// Options: RF24_PA_MIN | RF24_PA_LOW | RF24_PA_MED | RF24_PA_HIGH
//            -18dBm    |   -12dBm    |    -6dBm    |     0dBm
#define PA_LEVEL RF24_PA_HIGH

// 2.4GHz channel (0-125). Define to enable channels.
//    Not really needed unless working in places with a lot of 2.4GHz signals
//#define CHANNEL 1


//How long to check if there is an ongoing pair request on bootup? Only affects controller
//Pair requests only start on the MCU side on startup. After this time passes, no more pair requests
//  can be made until the MCU is power-cycled
#define PAIR_WAIT 5000         //in milliseconds
//How long to keep trying to pair with the controller before failing. Only affects the remote
#define PAIR_DURATION 30000    //in milliseconds
//Pair retries. Sometimes >4 tries are needed before it successfully pairs. Only affects the controller
#define PAIR_RETRIES 10

/****************************************************************************/
/**** THE BELOW SETTINGS SHOULD BE THE SAME ON THE CONTROLLER AND REMOTE ****/
/****************************************************************************/

// Error correction and message settings. 
#define MAX_MESSAGE_LENGTH 6      //min 6

// Data transfer speeds. Lower = more reliable transmission
// Options: RF24_250KBPS | RF24_1MBPS | RF24_2MBPS
#define DATA_RATE RF24_250KBPS


//********************** Dont touch the code below ********************************//

//You can change this from 0-5, but theres no reason to. This is just to not hard-code in a pipe channel
#define PIPE_CHANNEL 1

#if defined(AUTO_SET) && !defined(REMOTE) && !defined(CONTROLLER) && (defined(ARDUINO_AVR_NANO) || defined(ARDUINO_GENERIC_RP2040))
  #define REMOTE
#elif defined(AUTO_SET) && !defined(REMOTE) && !defined(CONTROLLER)
  #define CONTROLLER
#else
  #error "Auto set is enabled in settings.h but CONTROLLER or REMOTE is also defined"
#endif

#if !( defined(REMOTE) ^ defined(CONTROLLER))
  #error "You must specify if the radio module is attached to the controller or the remote"
#endif

#if MAX_MESSAGE_LENGTH < 6
  #error "MAX_MESSAGE_LENGTH must be at least 6"
#endif

#endif