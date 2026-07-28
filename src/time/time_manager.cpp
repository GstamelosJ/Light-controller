#include "time_manager.h"

void TimeManager::begin()
{
    setenv(
        "TZ",
        "EET-2EEST,M3.5.0/3,M10.5.0/4",
        1);

    tzset();
}
void TimeManager::update()
{
    if(millis() - _lastCheck < 1000)
        return;

    _lastCheck = millis();

    time_t now;

    time(&now);

    if(now > 1700000000)
    {
        _valid = true;
    }
}
bool TimeManager::isValid()
{
    return _valid;
}
tm TimeManager::getLocalTimeInfo()
{
    struct tm timeinfo;

    time_t now;

    time(&now);

    localtime_r(
        &now,
        &timeinfo);

    return timeinfo;
}
uint32_t TimeManager::getEpoch()
{
    time_t now;

    time(&now);

    return (uint32_t)now;
}

