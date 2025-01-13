#include "../BottangoArduinoModules.h"

#ifdef AUDIO_SD_I2S
#ifndef I2SHelper_h
#define I2SHelper_h

#include "SDCardUtil.h"
#include "driver/i2s.h"
#include "BottangoArduinoConfig.h"

namespace I2SHelper
{
#define I2S_WRITE_TIMEOUT (20 / portTICK_PERIOD_MS) // Short timeout for non-blocking operation
#define I2S_BUFFER_SIZE 512

    extern uint8_t buffer[];
    extern uint8_t cacheBuffer[];
    extern uint8_t volumeBuffer[];
    extern size_t cachedBytes;
    extern bool playing;
    extern File audioFile;
    extern File audioFileOnDeck;
    extern bool fileOnDeck;

    extern i2s_config_t i2sConfig;
    extern i2s_pin_config_t pinConfig;

    extern TaskHandle_t i2sTaskHandle;
    extern volatile bool stopTask;

    void startPlaying(i2s_config_t i2sConfig, File audioFile);
    void executeStart();
    bool isPlaying();
    void cleanup();
    void stopPlaying();

    void i2sTask(void *param);
}

#endif
#endif