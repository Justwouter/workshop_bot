#include "../BottangoArduinoModules.h"

#ifdef USE_SD_CARD_COMMAND_STREAM
#ifndef SDCardCommandStreamDataSource_h
#define SDCardCommandStreamDataSource_h

#include "AbstractCommandStreamDataSource.h"
#include "../BottangoArduinoConfig.h"
#include "SDCardUtil.h"

class SDCardCommandStreamDataSource : public AbstractCommandStreamDataSource
{
public:
    SDCardCommandStreamDataSource();
    SDCardCommandStreamDataSource(byte index);
    virtual void getNextCommand(char *output, bool shouldLoop, unsigned long &msEndOfThisCommand, unsigned long &msStartOfNextCommand) override;
    virtual void reset() override;
    ~SDCardCommandStreamDataSource();
    bool isValid = false;

private:
    byte index;
    bool setup;
    unsigned long travel = 0;
    bool onLoop = false;
    void internalGetCharCommand(char *output, bool persistTravel, File file);
};

#endif
#endif