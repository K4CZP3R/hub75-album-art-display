#include "main.h"

Matrix *matrix = nullptr;
CoverArt *coverArt = nullptr;
SerialRecv *serialRecv = nullptr;
Wireless *wireless = nullptr;
Socket *socket = nullptr;

void setup()
{
    Serial.begin(BAUD_RATE);
    matrix = new Matrix();
    if (matrix->setup() != 0)
    {
        Serial.println("Matrix setup failed");
        while (true)
            ;
    }

    coverArt = new CoverArt(matrix);
    serialRecv = new SerialRecv();

#ifdef USE_WS

    wireless = new Wireless();
    wireless->setup();

    socket = new Socket();
    socket->setup([](byte *buf, int length) -> byte
                  { return coverArt->handleMessage(buf, length); });
#endif
}

void loop()
{
    socket->loop();
    serialRecv->loop([](byte *buf, int length) -> byte
                     { return coverArt->handleMessage(buf, length); });
}