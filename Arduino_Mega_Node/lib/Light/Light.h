#ifndef LIGHT_H
#define LIGHT_H
#include <Arduino.h>

class Light
{
private:
    int _pin;

public:
    Light(int pin);
    void begin();
    double getLux();
};

#endif
