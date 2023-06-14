#include "NTPClientWrapper.h"

NTPClientWrapper::NTPClientWrapper(int timeZone) : timeClient(ntpUDP, "pool.ntp.org")
{
    _timeZone = timeZone;
}

void NTPClientWrapper::begin()
{
    timeClient.begin();
    timeClient.setTimeOffset(_timeZone);
}

void NTPClientWrapper::update()
{
    timeClient.update();
}

time_t NTPClientWrapper::_getEpochTime()
{
    return timeClient.getEpochTime();
}

String NTPClientWrapper::getFormattedTime()
{
    return timeClient.getFormattedTime();
}

int NTPClientWrapper::getHours()
{
    return timeClient.getHours();
}

int NTPClientWrapper::getMinutes()
{
    return timeClient.getMinutes();
}

int NTPClientWrapper::getSeconds()
{
    return timeClient.getSeconds();
}

int NTPClientWrapper::getDay()
{
    return timeClient.getDay();
}

int NTPClientWrapper::getMonthDay()
{
    time_t getEpochTime = _getEpochTime();
    struct tm *ptm = gmtime((time_t *)&getEpochTime);
    return ptm->tm_mday;
}

int NTPClientWrapper::getMonth()
{
    time_t getEpochTime = _getEpochTime();
    struct tm *ptm = gmtime((time_t *)&getEpochTime);
    return ptm->tm_mon + 1;
}

String NTPClientWrapper::getMonthName()
{
    time_t getEpochTime = _getEpochTime();
    struct tm *ptm = gmtime((time_t *)&getEpochTime);
    return months[ptm->tm_mon];
}

int NTPClientWrapper::getYear()
{
    time_t getEpochTime = _getEpochTime();
    struct tm *ptm = gmtime((time_t *)&getEpochTime);
    return ptm->tm_year + 1900;
}

String NTPClientWrapper::getCurrentDate()
{
    String currentDate = String(getYear()) + "-" + String(getMonth()) + "-" + String(getMonthDay());
    return currentDate;
}
