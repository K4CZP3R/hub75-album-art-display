#include "main.h"

Matrix *matrix = nullptr;
CoverArt *coverArt = nullptr;
SerialRecv *serialRecv = nullptr;
Wireless *wireless = nullptr;
Socket *socket = nullptr;
Ota *ota = nullptr;

void setup()
{
    Serial.begin(BAUD_RATE);

    wireless = new Wireless();
    wireless->setup();

    ota = new Ota();
    ota->setup(
        []()
        {
            if (matrix == nullptr)
            {
                return;
            }
            matrix->getVirtDisplay()->clearScreen();
            matrix->getVirtDisplay()->println("Update");
        },
        []()
        {
            if (matrix == nullptr)
            {
                return;
            }
            matrix->getVirtDisplay()->fillScreen(matrix->getVirtDisplay()->color444(0, 255, 0));
        },
        [](unsigned int progress, unsigned int total)
        { 
            if(matrix == nullptr) 
            {
                return;
            }

            matrix->getVirtDisplay()->clearScreen();
            matrix->getVirtDisplay()->setCursor(0,0);

            int mapped = map(progress, 0, total, 0, 100);
            matrix->getVirtDisplay()->printf("%d%%", mapped);
            
            int pixels = map(progress, 0, total, 0, 32*32);
            matrix->getVirtDisplay()->drawPixel(
                pixels % 32, 
                pixels / 32, 
                matrix->getVirtDisplay()->color444(0, 0, 255)
            );
            
            
            Serial.printf("OTA progress: %u%%\r", (progress / (total / 100))); },
        [](ota_error_t error)
        {
            if (matrix == nullptr)
            {
                return;
            }
            matrix->getVirtDisplay()->fillScreen(matrix->getVirtDisplay()->color444(255, 0, 0));
        });
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
    ota->loop();
    socket->loop();
    serialRecv->loop([](byte *buf, int length) -> byte
                     { return coverArt->handleMessage(buf, length); });
}