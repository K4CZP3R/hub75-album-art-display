#ifndef ota_h
#define ota_h

#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

class Ota
{
public:
    Ota();
    ~Ota();

    void setup(void (*onStart)(), void (*onEnd)(), void (*onProgress)(unsigned int progress, unsigned int total), void (*onError)(ota_error_t error));
    void loop();
};

#endif