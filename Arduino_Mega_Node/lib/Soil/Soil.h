#ifndef SOIL_H
#define SOIL_H

#include <Arduino.h>

class Soil
{
private:
    int _airValue;
    int _waterValue;
    int _soilMoistureValue;
    int _soilMoisturePercent;
    int _analogPin;

public:
    Soil(int airValue, int waterValue, int analogPin);
    void begin();
    int readSensorValue();
    int calculateSoilMoisturePercent();
    void printSoilMoisturePercent();
};

#endif
