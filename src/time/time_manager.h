#pragma once

#include <Arduino.h>
#include <time.h>

class TimeManager
{
public:

    void begin();

    void update();

    bool isValid();

    tm getLocalTimeInfo();

    uint32_t getEpoch();

private:

    bool _valid = false;

    uint32_t _lastCheck = 0;
};
