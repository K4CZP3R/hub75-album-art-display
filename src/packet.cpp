#include "packet.h"

const byte Packet::startMagic[Packet::START_MAGIC_SIZE] = {0x13, 0x37};
const byte Packet::endMagic[Packet::END_MAGIC_SIZE] = {0xde, 0xad};
