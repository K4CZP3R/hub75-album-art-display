#include "serial-recv.h"

SerialRecv::SerialRecv()
{
}

SerialRecv::~SerialRecv()
{
    if (msgBuffer != NULL)
    {
        delete[] msgBuffer;
        msgBuffer = NULL;
    }
}

void SerialRecv::sendMessageByte(byte type, byte val)
{
    byte buffer[] = {Packet::startMagic[0], Packet::startMagic[1], 0x2, type, val, Packet::endMagic[0], Packet::endMagic[1]};
    Serial.write(buffer, sizeof(buffer));
}
void SerialRecv::sendMessageWord(byte type, uint16_t val)
{
    byte buffer[] = {Packet::startMagic[0], Packet::startMagic[1], 0x5, type, (byte)(val >> 8), (byte)(val), Packet::endMagic[0], Packet::endMagic[1]};
    Serial.write(buffer, sizeof(buffer));
}

void SerialRecv::loop(byte (*externalFunction)(byte *buf, int length))
{
    while (Serial.available())
    {
        byte incomingByte = Serial.read();

        switch (currentState)
        {
        case WAITING:
            if (incomingByte == Packet::startMagic[messageIdx])
            {
                messageIdx++;

                if (messageIdx == Packet::START_MAGIC_SIZE)
                {
                    currentState = READING_MESSAGE;
                    messageIdx = 0;
                }
            }
            else
            {
                messageIdx = 0;
            }
            break;
        case READING_MESSAGE:
            if (messageIdx == 0)
            {
                expectedMessageLen = incomingByte;

                if (msgBuffer != NULL)
                {
                    delete[] msgBuffer;
                    msgBuffer = NULL;
                }

                msgBuffer = new byte[expectedMessageLen];
            }
            else
            {
                msgBuffer[messageIdx - 1] = incomingByte;
                if (messageIdx - 1 == expectedMessageLen - 1)
                {
                    currentState = CHECK_END_DELIMITER;
                }
            }

            messageIdx++;
            break;
        case CHECK_END_DELIMITER:
            if (incomingByte == Packet::endMagic[messageIdx - expectedMessageLen - 1])
            {
                messageIdx++;

                if (messageIdx - expectedMessageLen == Packet::END_MAGIC_SIZE)
                {
                    byte ret = externalFunction(msgBuffer, expectedMessageLen);

                    sendMessageByte(0xff, ret);

                    delete[] msgBuffer;
                    msgBuffer = NULL;

                    currentState = WAITING;
                    messageIdx = 0;
                }
            }
            else
            {
                currentState = WAITING;
                messageIdx = 0;
            }
            break;
        }
    }
}
