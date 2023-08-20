#ifndef wireless_H
#define wireless_h

#include <Arduino.h>
#include "config.h"
#include <WiFi.h>

class Wireless
{
public:
    Wireless();
    ~Wireless();

    void setup();
    void loop();
};

#endif