#include "Light.h"

Light::Light(int pin)
{
    _pin = pin;
}

void Light::begin()
{
    pinMode(_pin, INPUT);
}

double Light::getLux()
{
    double Vout = analogRead(_pin) * 0.0048828125;
    int lux = (2500 / Vout - 500) / 10;
    return lux;
}
