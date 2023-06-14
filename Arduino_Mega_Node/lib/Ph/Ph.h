#ifndef PH_H
#define PH_H

#include <Arduino.h>

class Ph
{
public:
  Ph(int sensorPin, float offset);
  void begin();
  double readVoltage();
  float readpH();

private:
  int _sensorPin;
  float _offset;
  static const int _arrayLength = 40;
  int _pHArray[_arrayLength];
  int _pHArrayIndex = 0;
  unsigned long _samplingTime = 0;
  unsigned long _printTime = 0;
  float _pHValue, _voltage;
  double _averageArray(int *arr, int number);
};

#endif /* PH_H */
