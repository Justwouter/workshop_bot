#include "../BottangoArduinoModules.h"

#ifdef USE_SD_CARD_COMMAND_STREAM
#include "SDCardCommandStreamDataSource.h"
#include "../BottangoArduinoConfig.h"
#include "BottangoCore.h"

SDCardCommandStreamDataSource::SDCardCommandStreamDataSource() : AbstractCommandStreamDataSource()
{
    SDCardUtil::initialize();
    if (SDCardUtil::sdCardInitialized && SDCardUtil::sdCardAvailable)
    {
        this->setup = true;
        isValid = true;
    }
    dataComplete = false;
}

SDCardCommandStreamDataSource::SDCardCommandStreamDataSource(byte index) : AbstractCommandStreamDataSource()
{
    SDCardUtil::initialize();
    if (SDCardUtil::sdCardInitialized && SDCardUtil::sdCardAvailable)
    {
        this->index = index;
        this->setup = false;
        isValid = true;
    }
    dataComplete = false;
}

void SDCardCommandStreamDataSource::getNextCommand(char *output, bool shouldLoop, unsigned long &msEndOfThisCommand, unsigned long &msStartOfNextCommand)
{
    if (!SDCardUtil::sdCardInitialized || !SDCardUtil::sdCardAvailable)
    {
        return;
    }

    // open the right file
    char finalPath[MAX_FILE_PATH_SIZE];
    if (setup)
    {
        SDCardUtil::getSetupFilePath(finalPath);
    }
    else
    {
        SDCardUtil::getAnimationFilePath(index, finalPath, onLoop, false);
    }

    File file = SDCardUtil::openFile(finalPath);

    // couldn't open file
    if (!file)
    {
        return;
    }

    // catch up cursor
    if (travel > 0)
    {
        file.seek(travel);
    }

    // get output
    internalGetCharCommand(output, true, file);
    if (output[0] != '\0')
    {
        char copy[strlen(output) + 1]; // +1 for the null terminator
        strcpy(copy, output);
        msEndOfThisCommand = BottangoCore::getMSEndTimeOfCommand(copy);
    }

    // at end of file
    if (!file.available())
    {
        if (shouldLoop)
        {
            // move to loop
            if (!onLoop)
            {
                travel = 0;
                onLoop = true;
                finalPath[0] = '\0';
                SDCardUtil::getAnimationFilePath(index, finalPath, true, false);
                SDCardUtil::closeFile(file);

                // end if can't find loop file
                file = SDCardUtil::openFile(finalPath);
                if (!file)
                {
                    SDCardUtil::closeFile(file);
                    msStartOfNextCommand = 0;
                    dataComplete = true;
                    return;
                }
            }
            // loop complete
            else
            {
                msStartOfNextCommand = 0;
                dataComplete = true;
            }
        }
        // data complete, no loop
        else
        {
            SDCardUtil::closeFile(file);
            dataComplete = true;
            msStartOfNextCommand = 0;
            return;
        }
    }

    // find next command
    char nextCommand[MAX_COMMAND_LENGTH];
    nextCommand[0] = '\0';
    internalGetCharCommand(nextCommand, false, file);
    if (nextCommand[0] != '\0')
    {
        msStartOfNextCommand = BottangoCore::getMSStartTimeOfCommand(nextCommand);
    }
    SDCardUtil::closeFile(file);
}

void SDCardCommandStreamDataSource::internalGetCharCommand(char *output, bool persistTravel, File file)
{
    unsigned long travelCache = travel;

    int iterationCount = 0;
    while (file.available())
    {
        char c = file.read();
        travel++;

        // if got a full command
        // or at end of buffer
        if (c == '\n' || iterationCount >= MAX_COMMAND_LENGTH)
        {
            // add string termination instead of newline
            output[iterationCount] = '\0';

            // command found, we're done

            if (!persistTravel)
            {
                file.seek(travel * -1);
                travel = travelCache;
            }

            return;
        }

        // add char to ouput
        output[iterationCount] = c;
        iterationCount++;
    }

    // at end of file mid command?
    if (!persistTravel)
    {
        file.seek(travel * -1);
        travel = travelCache;
    }
}

void SDCardCommandStreamDataSource::reset()
{
    onLoop = false;
    dataComplete = false;
    travel = 0;
}

SDCardCommandStreamDataSource::~SDCardCommandStreamDataSource()
{
}
#endif