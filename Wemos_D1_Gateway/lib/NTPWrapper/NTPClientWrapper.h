#ifndef NTPCLIENT_WRAPPER_H
#define NTPCLIENT_WRAPPER_H

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class NTPClientWrapper
{
public:
  NTPClientWrapper(int timeZone);
  void begin();
  void update();
  time_t _getEpochTime();
  String getFormattedTime();
  int getHours();
  int getMinutes();
  int getSeconds();
  int getDay();
  int getMonthDay();
  int getMonth();
  String getMonthName();
  int getYear();
  String getCurrentDate();

private:
  WiFiUDP ntpUDP;
  NTPClient timeClient;
  int _timeZone;
  String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
};

#endif
