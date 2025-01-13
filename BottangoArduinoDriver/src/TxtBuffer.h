#include "../BottangoArduinoModules.h"
#if defined(RELAY_COMS_ESPNOW)

#ifndef TxtBuffer_h
#define TxtBuffer_h

#include "../BottangoArduinoConfig.h"

class TxtBuffer
{
public:
    TxtBuffer();
    void addTxt(const char *txt);
    bool available();
    void getNextTxt(char *outTxt);
    bool isFull();

private:
    int head = 0;
    int tail = 0;
    char buffer[ESPNOW_TXT_BUFFER_SIZE];
};

#endif
#endif