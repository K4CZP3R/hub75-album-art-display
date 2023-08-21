#include "main.h"

Matrix *matrix = nullptr;
CoverArt *coverArt = nullptr;
SerialRecv *serialRecv = nullptr;
Wireless *wireless = nullptr;
Socket *socket = nullptr;
Ota *ota = nullptr;
NtpTime *ntpTime = nullptr;
static unsigned long lastMsgTime = millis();

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

    ntpTime = new NtpTime();
    ntpTime->setup();

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

    socket = new Socket();
    socket->setup([](byte *buf, int length) -> byte
                  { 
                    lastMsgTime = millis();
                    return coverArt->handleMessage(buf, length); });
#endif
}

void loop()
{
    ota->loop();
    socket->loop();
    serialRecv->loop([](byte *buf, int length) -> byte
                     { 
                        lastMsgTime = millis();
                        return coverArt->handleMessage(buf, length); });

    if (millis() - lastMsgTime > 30000)
    {

        int hour, minute;
        ntpTime->getHourAndMinutes(hour, minute);

        if (matrix != nullptr)
        {
            matrix->getVirtDisplay()->clearScreen();
            matrix->getVirtDisplay()->setCursor(0, 0);
            matrix->getVirtDisplay()->printf("%02d:%02d", hour, minute);
            matrix->getDmaDisplay()->setBrightness(64);
        }

        lastMsgTime = millis();
    }
}