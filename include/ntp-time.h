#ifndef ntptime_h
#define ntptime_h

#include <Arduino.h>
#include "time.h"

class NtpTime
{
public:
    NtpTime();
    ~NtpTime();

    void setup();
    void loop();

    time_t getTime();
    void getHourAndMinutes(int &hour, int &minutes);
};

#endif