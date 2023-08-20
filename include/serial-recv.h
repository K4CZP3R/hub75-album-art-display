#ifndef serialrecv_h
#define serialrecv_h

#include <Arduino.h>
#include "packet.h"

class SerialRecv
{
public:
    SerialRecv();
    ~SerialRecv();

    void loop(byte (*externalFunction)(byte *buf, int length));
    void sendMessageByte(byte type, byte val);
    void sendMessageWord(byte type, uint16_t val);

private:
    byte *msgBuffer = NULL;
    int messageIdx = 0;
    int expectedMessageLen = 0;

    enum ReadingState
    {
        WAITING,
        READING_MESSAGE,
        CHECK_END_DELIMITER
    };

    ReadingState currentState = WAITING;
};

#endif