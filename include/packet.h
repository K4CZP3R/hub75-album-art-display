#ifndef packet_h
#define packet_h

#include <Arduino.h>

class Packet
{
public:
    static const int START_MAGIC_SIZE = 2;
    static const int END_MAGIC_SIZE = 2;
    static const int MESSAGE_OVERHEAD = START_MAGIC_SIZE + 1 + END_MAGIC_SIZE;
    static const byte startMagic[START_MAGIC_SIZE];
    static const byte endMagic[END_MAGIC_SIZE];

    static bool isValidPacket(byte *buf, int length)
    {
        if (length < MESSAGE_OVERHEAD)
        {
            return false;
        }

        if (buf[0] != startMagic[0] || buf[1] != startMagic[1] || buf[length - 2] != endMagic[0] || buf[length - 1] != endMagic[1])
        {
            return false;
        }

        return true;
    }
};

#endif