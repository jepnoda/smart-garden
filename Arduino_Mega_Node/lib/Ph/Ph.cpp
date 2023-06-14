#include "Ph.h"

Ph::Ph(int sensorPin, float offset)
{
  _sensorPin = sensorPin;
  _offset = offset;
}

void Ph::begin()
{
  pinMode(_sensorPin, INPUT);
}

float Ph::readpH()
{
  _voltage = readVoltage();
  _pHValue = 3.5 * _voltage + _offset;

  return _pHValue;
}

double Ph::readVoltage()
{
  _pHArray[_pHArrayIndex++] = analogRead(_sensorPin);
  if (_pHArrayIndex == _arrayLength)
    _pHArrayIndex = 0;
  _voltage = _averageArray(_pHArray, _arrayLength) * 5.0 / 1024;

  return _voltage;
}

double Ph::_averageArray(int *arr, int number)
{
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0)
  {
    Serial.println("Error number for the array to averaging!/n");
    return 0;
  }
  if (number < 5)
  {
    for (i = 0; i < number; i++)
    {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  }
  else
  {
    if (arr[0] < arr[1])
    {
      min = arr[0];
      max = arr[1];
    }
    else
    {
      min = arr[1];
      max = arr[0];
    }
    for (i = 2; i < number; i++)
    {
      if (arr[i] < min)
      {
        amount += min;
        min = arr[i];
      }
      else
      {
        if (arr[i] > max)
        {
          amount += max;
          max = arr[i];
        }
        else
        {
          amount += arr[i];
        }
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}
