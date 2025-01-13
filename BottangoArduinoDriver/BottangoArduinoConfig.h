#ifndef DevConsts_h
#define DevConsts_h

#include "BottangoArduinoModules.h"
#include <Arduino.h>

// !! Max Registered Motors !!
// Max effectors (motors, etc.) that can be registered at once.
// more than 8 crashes an Arduino Uno / Nano
// otherwise capped by default at 16, but can be increased if needed (depending on your hardware)

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
#ifdef USE_ADAFRUIT_PWM_LIBRARY
#define MAX_REGISTERED_EFFECTORS 6 // Arduino Uno or Nano + PCA driver can only support 6
#else
#define MAX_REGISTERED_EFFECTORS 8 // Arduino Uno or Nano bare pin pwm supports 8
#endif
#else
#define MAX_REGISTERED_EFFECTORS 16 // most modern boards can handle 16 or more (EX, ESP32, etc.)
#endif

// ---------------------------------- //

// !! Module Configuration !!

// !! Wifi Configuration !!
#ifdef USE_ESP32_WIFI
#define WIFI_SSID "MY_NETWORK_ID"                   // wifi ssid
#define WIFI_PASSWORD "MY_NETWORK_PW"               // wifi password
#define WIFI_SERVER_IP "XXX.XXX.XXX.X"              // wifi server ip on the local network, likely something like 192.168.1.X
#define WIFI_SERVER_PORT 59225                      // wifi server port on the local network
#endif

// !! Module Pins Configuration !!

// !! SD Card Pin Configuration !!
// Note that Arduino Uno / Nano are not supported with SD Card
#if defined(USE_SD_CARD_COMMAND_STREAM) || defined(AUDIO_SD_I2S)
#define SDPIN_CS 5                                  // chip select pin
#ifdef ESP32                                        // spi pins
#define SDPIN_MOSI 23                               // MOSI pin
#define SDPIN_CLK 18                                // clock pin
#define SDPIN_MISO 19                               // MISO pin
#endif
#endif

// !! Dynamic switch between live and exported animations !!
#ifdef ENABLE_DYNAMIC_ANIMATION_SOURCE_SWITCH
#define ANIMATION_STATE_SELECTION_PIN 34            // Pin to monitor on boot to determine if should be in live control or exported
#define SELECT_EXPORTED_IS_HIGH                     // High on pin selects exported. Comment out this line to choose exported on low.
#endif

// !! I2S and Audio Pins !!
#ifdef AUDIO_SD_I2S
#define I2S_BCLK 25                                 // bit clock line
#define I2S_LRC 17                                  // left right clock
#define I2S_DOUT 16                                 // data
#ifdef DYNAMIC_VOLUME
#define VOLUME_PIN 35
#define VOLUME_MIN 0.01;
#define VOLUME_MAX 0.5;
#endif
#ifdef PIN_ON_AUDIO_PLAY
#define AUDIO_ENABLE_PIN 2
#endif
#endif


// !! SD Card Pin Configuration !!
// Note that Arduino Uno / Nano are not supported with SD Card

// ---------------------------------- //

// !! The below are ADVANCED configurations !!

// Curve Evaluation:
// enable one of the bezier curve default evaluation methods:
#define DEFAULT_FLOAT_CURVE // float math option (default, battle hardened and highly tested)
// #define DEFAULT_FIXED_CURVE                      // fixed math option. Slightly lower curve precision, but faster.  Likely will see improved HW communication and higher max effector count. New and being tested.


#define AVR_STEPPER_FIXED                           // in order to keep up with the demands of stepper motors, AVR boards (Arduino Uno, Nano, Mega, etc) use
                                                    // fixed evaluation overriding the above default. Comment this line out to use the default curve evaluation on avr steppers

#define VELOCITY_SEGMENT_MS 67                      // Velocity effectors (IE steppers) will segment curves into chunks of velocity. This field defines how long each chunk is in MS.
                                                    // lower number means more segments. More segments will be more accurate to the curve, but slower to process and less smooth stepper movement
                                                    // If you wanted to target 15 segments per second (the default) divide 1000 by 15: 1000 / 15 = 66.666, so rounded to 67
                                                    // 30 segments per second would be 33, etc.

#define STEPPER_SYNC_SPEED 2                        // steppers will sync at 1/X percent of max speed. IE, 2 == 50% max speed, 4 == 25% max Speed, etc.


// recieved Command parsing:
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
#define MAX_COMMAND_LENGTH 100                               // max character count of a command
#else
#define MAX_COMMAND_LENGTH 248                               // max character count of a command
#endif
#define READ_TIMEOUT 50                                      // time in MS before timeout while reading incoming serial
#define COMMANDS_PARAMS_SIZE 15                              // max number of paramaters in an incoming command
#define BAUD_RATE 115200                                     // serial speed at which Bottango and Arduino communicate with each other
#define COMPRESSED_SIGNAL_MAX 8192.0f                        // Movement is sent as a value between 0 and this value
#define COMPRESSED_SIGNAL_MAX_INT (int)COMPRESSED_SIGNAL_MAX // Cast to int shortcut
#define TRIGGER_EVENT_PIN_TIME 25                            // time in MS to keep a pin high before setting it back low in trigger events
#define MAX_NUM_CURVES 8                                     // number of curves to precache

// sd card configuration
#if defined(USE_SD_CARD_COMMAND_STREAM) || defined(AUDIO_SD_I2S)
#define MAX_FILE_PATH_SIZE 30                               // max character count of a file path
const char SD_ANIMATION_PATH[] PROGMEM = "/anim/";          // file path to look for animation directories on sd card.
const char SD_AUDIO_PATH[] PROGMEM = "/audio/";             // file path to look for audio files
const char SD_SETUP_PATH[] PROGMEM = "setup/";              // file path to look for initial set up data
const char SD_DATA_ANIMDATA[] PROGMEM = "data.txt";         // file name of animation data
const char SD_DATA_LOOPDATA[] PROGMEM = "loop.txt";         // file name of loop data
const char SD_DATA_CONFIGDATA[] PROGMEM = "config.txt";     // file name of config data
const char SD_AUDIO_FORMAT[] PROGMEM = ".wav        ";      // file format name of audio file
#endif

// animation export config
#define MAX_EXPORTED_ANIMATIONS 32                          // max number of animations that can be exported and played back without PC connection

// relay parent and child config
#ifdef RELAY_PARENT
#define MAX_RELAY_CHILD 16                                  // max number of controllers to relay commands to
#endif

#ifdef RELAY_COMS_ESPNOW
#define ESPNOW_CHANNEL 1                                    // Wifi channel to use. All relays (send and recv) must be on the same channel
#define ESPNOW_TXT_BUFFER_SIZE 256                         // max num chars to store in the espnow recv and send buffers
#define ESPNOW_RETRY_MS 50                                  // time in MS before retry sending
#define ESPNOW_TIMEOUT_MS 250                                  // time in MS before timeout

#ifdef RELAY_CHILD
#define ESPNOW_PARENT_ADDRESS {0x90, 0x15, 0x06, 0xdb, 0x41, 0x7c} // mac address of parent 

#endif
#endif

// extra logging
#if defined(RELAY_CHILD) || defined(USE_ESP32_WIFI)
#define ENABLE_SERIAL_LOGGING                               // when communicating without serial, enable it for extra logging
#endif

#endif //config