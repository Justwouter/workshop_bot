#include "../BottangoArduinoModules.h"

#ifdef AUDIO_SD_I2S
#include "I2SHelper.h"

namespace I2SHelper
{
    uint8_t buffer[I2S_BUFFER_SIZE];
    uint8_t cacheBuffer[I2S_BUFFER_SIZE];
    uint8_t volumeBuffer[I2S_BUFFER_SIZE];
    size_t cachedBytes = 0;
    bool playing = false;
    File audioFile;
    bool fileOnDeck = false;
    File audioFileOnDeck;

    i2s_config_t i2sConfig;
    i2s_pin_config_t pinConfig = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE};

    TaskHandle_t i2sTaskHandle = nullptr; // Task handle for i2S task
    volatile bool stopTask = false;

    void startPlaying(i2s_config_t i2sConfig, File audioFile)
    {

#ifdef DYNAMIC_VOLUME
        pinMode(VOLUME_PIN, INPUT);
#endif

#ifdef PIN_ON_AUDIO_PLAY
        pinMode(AUDIO_ENABLE_PIN, OUTPUT);
        digitalWrite(AUDIO_ENABLE_PIN, LOW);
#endif
        if (isPlaying())
        {
            I2SHelper::audioFileOnDeck = audioFile;
            I2SHelper::fileOnDeck = true;
            stopPlaying();
        }
        else
        {
            I2SHelper::i2sConfig = i2sConfig;
            I2SHelper::audioFile = audioFile;
            executeStart();
        }
    }

    void executeStart()
    {
#ifdef PIN_ON_AUDIO_PLAY
        digitalWrite(AUDIO_ENABLE_PIN, HIGH);
#endif
        // install i2s
        i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, NULL);
        i2s_set_pin(I2S_NUM_0, &pinConfig);

        // reset buffers
        memset(buffer, 0, sizeof(I2S_BUFFER_SIZE));
        memset(cacheBuffer, 0, sizeof(I2S_BUFFER_SIZE));
        memset(volumeBuffer, 0, sizeof(I2S_BUFFER_SIZE));

        cachedBytes = 0;

        // assume that the audio file is passed here in the correct position

        Serial.println("start task");
        xTaskCreate(
            i2sTask,       // Task function
            "i2SPlay",     // Name of the task
            4096,          // Stack size in words
            NULL,          // Task input parameter
            1,             // Priority of the task
            &i2sTaskHandle // Task handle
        );

        playing = true;
    }

    bool isPlaying()
    {
        return playing;
    }

    void stopPlaying()
    {
        if (i2sTaskHandle != nullptr)
        {
            stopTask = true;
        }
    }

    void i2sTask(void *param)
    {
        while (!stopTask && (audioFile.available() || cachedBytes > 0))
        {
            size_t bytesRead = 0;
            // restore cached to main buffer
            if (cachedBytes > 0)
            {
                bytesRead = cachedBytes;
                memcpy(buffer, cacheBuffer, cachedBytes); // Copy cached data to the primary buffer
                cachedBytes = 0;                          // Clear cache
            }
            // fill remaining buffer with read from SD
            if (audioFile.available() && bytesRead < I2S_BUFFER_SIZE)
            {
                size_t additionalBytes = audioFile.read(volumeBuffer, I2S_BUFFER_SIZE - bytesRead); // read to fill the volume buffer, only up to what we need to fill remaining space

// // adjust volume
#ifdef DYNAMIC_VOLUME
                float min = 0.01;
                float max = 0.1;
                float volume = min + (max - min) * ((float)analogRead(VOLUME_PIN) / 4095.0);
                // float volume = 0.1; // Volume scale factor (0.0 to 1.0)
#else
                float volume = 0.1; // Volume scale factor (0.0 to 1.0)
#endif

                switch (i2sConfig.bits_per_sample)
                {
                case I2S_BITS_PER_SAMPLE_8BIT:
                {
                    uint8_t *sampleBuffer8 = volumeBuffer;
                    for (size_t i = 0; i < additionalBytes; i++)
                    {
                        sampleBuffer8[i] = static_cast<uint8_t>(sampleBuffer8[i] * volume);
                    }
                    break;
                }
                case I2S_BITS_PER_SAMPLE_16BIT:
                {
                    int16_t *sampleBuffer16 = reinterpret_cast<int16_t *>(volumeBuffer);
                    for (size_t i = 0; i < additionalBytes / 2; i++)
                    {
                        sampleBuffer16[i] = static_cast<int16_t>(sampleBuffer16[i] * volume);
                    }
                    break;
                }
                case I2S_BITS_PER_SAMPLE_24BIT:
                {
                    uint8_t *byteBuffer24 = volumeBuffer;
                    for (size_t i = 0; i < additionalBytes / 3; i++)
                    {
                        int32_t sample = (byteBuffer24[i * 3 + 0] << 8) | (byteBuffer24[i * 3 + 1] << 16) | (byteBuffer24[i * 3 + 2] << 24);
                        sample = static_cast<int32_t>(sample * volume);
                        byteBuffer24[i * 3 + 0] = (sample >> 8) & 0xFF;
                        byteBuffer24[i * 3 + 1] = (sample >> 16) & 0xFF;
                        byteBuffer24[i * 3 + 2] = (sample >> 24) & 0xFF;
                    }
                    break;
                }
                case I2S_BITS_PER_SAMPLE_32BIT:
                {
                    int32_t *sampleBuffer32 = reinterpret_cast<int32_t *>(volumeBuffer);
                    for (size_t i = 0; i < additionalBytes / 4; i++)
                    {
                        sampleBuffer32[i] = static_cast<int32_t>(sampleBuffer32[i] * volume);
                    }
                    break;
                }
                }

                memcpy(buffer + bytesRead, volumeBuffer, additionalBytes);
                bytesRead += additionalBytes; // Update bytesRead with the newly read data
            }

            if (bytesRead > 0)
            {

                unsigned long now = millis();
                size_t bytesWritten = 0;
                // Write samples to I2S
                esp_err_t result = i2s_write(I2S_NUM_0, buffer, bytesRead, &bytesWritten, I2S_WRITE_TIMEOUT);

                if (result != ESP_OK || bytesWritten < bytesRead)
                {
                    // Cache the unwritten portion if the buffer was full
                    size_t unwrittenBytes = bytesRead - bytesWritten;
                    memcpy(cacheBuffer, buffer + bytesWritten, unwrittenBytes);
                    cachedBytes = unwrittenBytes;
                }
            }
        }

        playing = false;
#ifdef PIN_ON_AUDIO_PLAY
        digitalWrite(AUDIO_ENABLE_PIN, LOW);
#endif
        SDCardUtil::closeFile(audioFile);
        i2s_driver_uninstall(I2S_NUM_0);
        stopTask = false;

        if (fileOnDeck)
        {
            startPlaying(i2sConfig, audioFileOnDeck);
            fileOnDeck = false;
        }

        vTaskDelete(NULL);
    }
}
#endif