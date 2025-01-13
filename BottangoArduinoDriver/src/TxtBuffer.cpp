#include "../BottangoArduinoModules.h"
#if defined(RELAY_COMS_ESPNOW)

#include "TxtBuffer.h"

TxtBuffer::TxtBuffer()
{
    head = 0;
    tail = 0;
    buffer[0] = '\0';
}

void TxtBuffer::addTxt(const char *txt)
{
    int len = strlen(txt);
    if (len > MAX_COMMAND_LENGTH)
    {
        // todo error
        len = MAX_COMMAND_LENGTH;
    }

    for (int i = 0; i < len; i++)
    {
        // Check if the buffer is full before adding more characters
        if (isFull())
        {
            // todo error
            break; // Stop adding text if the buffer is full
        }

        buffer[head] = txt[i];
        head = (head + 1) % ESPNOW_TXT_BUFFER_SIZE;

        // Handle buffer overflow
        if (head == tail)
        {
            tail = (tail + 1) % ESPNOW_TXT_BUFFER_SIZE;
        }
    }
}

bool TxtBuffer::available()
{
    return head != tail;
}

void TxtBuffer::getNextTxt(char *outTxt)
{
    if (!available())
    {
        outTxt[0] = '\0'; // Return empty string if no data
        return;
    }

    int i = 0;
    while (tail != head && i < MAX_COMMAND_LENGTH - 1)
    {
        outTxt[i] = buffer[tail];
        char currentChar = buffer[tail];
        tail = (tail + 1) % ESPNOW_TXT_BUFFER_SIZE;
        i++;

        if (currentChar == '\n')
        {
            break; // Stop at newline but include it
        }
    }

    outTxt[i] = '\0'; // null terminate the output
}

bool TxtBuffer::isFull()
{
    return ((head + 1) % ESPNOW_TXT_BUFFER_SIZE) == tail;
}
#endif