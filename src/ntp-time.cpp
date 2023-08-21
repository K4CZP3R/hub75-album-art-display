#include "ntp-time.h"

NtpTime::NtpTime()
{
}

NtpTime::~NtpTime()
{
}

void NtpTime::setup()
{

    configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");
}

void NtpTime::loop()
{
}

time_t NtpTime::getTime()
{
    time_t now;
    time(&now);
    return now;
}

void NtpTime::getHourAndMinutes(int &hour, int &minutes)
{
    time_t now = getTime();
    struct tm *timeinfo;
    timeinfo = localtime(&now);
    hour = timeinfo->tm_hour;
    minutes = timeinfo->tm_min;
}
