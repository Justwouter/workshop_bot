#include "CommandStreamProvider.h"
#include "../BottangoArduinoConfig.h"
#include "BottangoCore.h"

CommandStreamProvider::CommandStreamProvider()
{
}

void CommandStreamProvider::runSetup()
{
    if (streamIsInProgress())
    {
        stop();
    }

#ifdef USE_CODE_COMMAND_STREAM
    commandStream = GeneratedCodeAnimations::GenerateSetupCommandStream();
#elif defined(USE_SD_CARD_COMMAND_STREAM)
    SDCardCommandStreamDataSource *dataSource = new SDCardCommandStreamDataSource();
    if (dataSource->isValid)
    {
        commandStream = new CommandStream(dataSource);
    }
    else
    {
        delete dataSource;
        commandStream = nullptr;
    }

#endif

    // run setup
    runInProgressCommand();

    // parse configs
    // will also run start animation if any
#if defined(USE_SD_CARD_COMMAND_STREAM) || defined(USE_CODE_COMMAND_STREAM)
    ExportedAnimationPlaybackControl::initialize();
#endif
}

void CommandStreamProvider::startCommandStream(byte streamID, bool loop)
{
    if (streamIsInProgress())
    {
        stop();
    }

#ifdef USE_CODE_COMMAND_STREAM
    commandStream = GeneratedCodeAnimations::GenerateCommandStreamByIndex(streamID);
#elif defined(USE_SD_CARD_COMMAND_STREAM)
    commandStream = new CommandStream(new SDCardCommandStreamDataSource(streamID));
#endif

    if (commandStream != nullptr)
    {
        if (loop)
        {
            commandStream->setShouldLoop();
        }
        Time::syncTime(0);
    }
}

void CommandStreamProvider::runInProgressCommand()
{
    if (streamIsInProgress())
    {
        while (commandStream->readyForNextCommand())
        {
            char commandBuffer[MAX_COMMAND_LENGTH];
            commandBuffer[0] = '\0';

            commandStream->getNextCommand(commandBuffer);
            if (commandBuffer[0] != '\0')
            {
                BottangoCore::executeCommand(commandBuffer);
            }
        }

        if (commandStream->complete())
        {
            stop();
        }
    }
}

void CommandStreamProvider::updateOnLoop()
{
    runInProgressCommand();
#if defined(USE_CODE_COMMAND_STREAM) || defined(USE_SD_CARD_COMMAND_STREAM)
    ExportedAnimationPlaybackControl::updatePlaybackStatus();
#endif
}

void CommandStreamProvider::stop()
{
    delete commandStream;
    commandStream = nullptr;
    BottangoCore::effectorPool.clearAllCurves();
}

bool CommandStreamProvider::streamIsInProgress()
{
    return commandStream != nullptr;
}