#include "wireless.h"

Wireless::Wireless()
{
}

Wireless::~Wireless()
{
}

void Wireless::setup()
{
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void Wireless::loop()
{
}