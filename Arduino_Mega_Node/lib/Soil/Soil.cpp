#include "Soil.h"

Soil::Soil(int airValue, int waterValue, int analogPin)
{
    _airValue = airValue;
    _waterValue = waterValue;
    _soilMoistureValue = 0;
    _soilMoisturePercent = 0;
    _analogPin = analogPin;
}

void Soil::begin()
{
    pinMode(_analogPin, INPUT);
}

int Soil::readSensorValue()
{
    _soilMoistureValue = analogRead(_analogPin);
    return _soilMoistureValue;
}

int Soil::calculateSoilMoisturePercent()
{
    //_soilMoisturePercent = map(_soilMoistureValue, _airValue, _waterValue, 0, 100);
    int raw = map(_soilMoistureValue, _airValue, _waterValue, 0, 100);
    _soilMoisturePercent = constrain(raw, 0, 100);
    // if (_soilMoisturePercent >= 100)
    // {
    //     _soilMoisturePercent = 100;
    // }
    // else if (_soilMoisturePercent <= 0)
    // {
    //     _soilMoisturePercent = 0;
    // }
    // else if (_soilMoisturePercent > 0 && _soilMoisturePercent < 100)
    // {
    //     _soilMoisturePercent = _soilMoisturePercent;
    // }
    return _soilMoisturePercent;
}

void Soil::printSoilMoisturePercent()
{
    if (_soilMoisturePercent >= 100)
    {
        Serial.println("100 %");
    }
    else if (_soilMoisturePercent <= 0)
    {
        Serial.println("0 %");
    }
    else if (_soilMoisturePercent > 0 && _soilMoisturePercent < 100)
    {
        Serial.print(_soilMoisturePercent);
        Serial.println("%");
    }
}
