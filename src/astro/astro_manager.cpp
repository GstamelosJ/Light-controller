#include "astro_manager.h"

#include <Dusk2Dawn.h>
#include <time.h>
void AstroManager::begin(
    double latitude,
    double longitude)
{
    _latitude = latitude;
    _longitude = longitude;
}
void AstroManager::calculate()
{
    time_t now;

    time(&now);

    struct tm info;

    localtime_r(
        &now,
        &info);

    if(info.tm_yday ==
       _lastCalculatedDay)
    {
        return;
    }

    _lastCalculatedDay =
        info.tm_yday;
		
Dusk2Dawn astro(
        _latitude,
        _longitude,
        2);
		
_sunriseMinutes =
        astro.sunrise(
            info.tm_year + 1900,
            info.tm_mon + 1,
            info.tm_mday,
            false);

    _sunsetMinutes =
        astro.sunset(
            info.tm_year + 1900,
            info.tm_mon + 1,
            info.tm_mday,
            false);
}
int AstroManager::getSunriseMinutes()
{
    return _sunriseMinutes;
}

int AstroManager::getSunsetMinutes()
{
    return _sunsetMinutes;
}

