#pragma once

#include <Arduino.h>

class AstroManager
{
public:

    void begin(
        double latitude,
        double longitude);

    void calculate();

    int getSunriseMinutes();

    int getSunsetMinutes();

private:

    double _latitude;
    double _longitude;

    int _sunriseMinutes = 360;

    int _sunsetMinutes = 1200;

    int _lastCalculatedDay = -1;
};
