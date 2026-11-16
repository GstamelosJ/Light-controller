#include "astro_manager.h"

#include <Dusk2Dawn.h>
#include <time.h>

void AstroManager::begin(
    double latitude,
    double longitude)
{
    _latitude = latitude;
    _longitude = longitude;

    // Force calculation after initialization.
    _lastCalculatedDay = -1;
}

void AstroManager::calculate()
{
    time_t now;
    time(&now);

    /*
     * Do not perform astronomical calculations
     * before valid NTP time is available.
     */
    if(now < 1700000000)
    {
        return;
    }

    struct tm info;

    localtime_r(
        &now,
        &info);

    /*
     * Calculate only once per calendar day.
     */
    if(info.tm_yday ==
       _lastCalculatedDay)
    {
        return;
    }

    const int year =
        info.tm_year + 1900;

    const int month =
        info.tm_mon + 1;

    const int day =
        info.tm_mday;

    /*
     * tm_isdst:
     * > 0 : daylight-saving time active
     *   0 : standard time
     * < 0 : DST state unknown
     */
    const bool isDst =
        info.tm_isdst > 0;

    /*
     * Greece standard timezone is UTC+2.
     * DST is supplied separately through isDst.
     */
    Dusk2Dawn astro(
        _latitude,
        _longitude,
        2.0f);

    const int sunrise =
        astro.sunrise(
            year,
            month,
            day,
            isDst);

    const int sunset =
        astro.sunset(
            year,
            month,
            day,
            isDst);

    /*
     * Keep the previous valid values if the
     * astronomical calculation returns an error.
     */
    if(sunrise >= 0 &&
       sunrise <= 1439)
    {
        _sunriseMinutes =
            sunrise;
    }

    if(sunset >= 0 &&
       sunset <= 1439)
    {
        _sunsetMinutes =
            sunset;
    }

    /*
     * Mark the day as processed only after
     * performing the calculation.
     */
    _lastCalculatedDay =
        info.tm_yday;
}

int AstroManager::getSunriseMinutes()
{
    return _sunriseMinutes;
}

int AstroManager::getSunsetMinutes()
{
    return _sunsetMinutes;
}