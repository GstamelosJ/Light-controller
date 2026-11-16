#include "scheduler.h"

#include <time.h>

static bool dayMatches(
    uint8_t mask,
    int day)
{
    switch(day)
    {
        case 1:
            return mask & SUNDAY_MASK;

        case 2:
            return mask & MONDAY_MASK;

        case 3:
            return mask & TUESDAY_MASK;

        case 4:
            return mask & WEDNESDAY_MASK;

        case 5:
            return mask & THURSDAY_MASK;

        case 6:
            return mask & FRIDAY_MASK;

        case 7:
            return mask & SATURDAY_MASK;
    }

    return false;
}

void Scheduler::begin(
    ChannelConfig channels[],
    RelayManager* relays,
    AstroManager* astro)
{
    _channels = channels;

    _relays = relays;

    _astro = astro;
}

void Scheduler::run()
{
    time_t nowEpoch;

    time(&nowEpoch);

    struct tm now;

    localtime_r(
        &nowEpoch,
        &now);

    int minuteNow =
        (now.tm_hour * 60)
        + now.tm_min;

    /*
     * Εκτέλεση μόνο μία φορά
     * ανά λεπτό
     */
    if(minuteNow == _lastMinute)
    {
        return;
    }

    _lastMinute =
        minuteNow;

    for(int channel = 0;
        channel < MAX_CHANNELS;
        channel++)
    {
        ChannelConfig& ch =
            _channels[channel];

        /*
         * Disabled channel
         */
        if(!ch.enabled)
        {
            continue;
        }

        /*
         * Manual modes
         */
        if(ch.mode == MANUAL_ON)
        {
            _relays->set(
                channel,
                true);

            continue;
        }

        if(ch.mode == MANUAL_OFF)
        {
            _relays->set(
                channel,
                false);

            continue;
        }

        if(ch.mode == DISABLED_MODE)
        {
            _relays->set(
                channel,
                false);

            continue;
        }

        /*
         * AUTO MODE
         */
        for(int i = 0;
            i < ch.eventCount;
            i++)
        {
            ScheduleEvent& ev =
                ch.events[i];

            if(!ev.enabled)
            {
                continue;
            }

            if(!dayMatches(
                    ev.daysMask,
                    now.tm_wday + 1))
            {
                continue;
            }

            /*
             * Ήδη εκτελέστηκε σήμερα
             */
            if(ev.lastExecutionDay ==
               now.tm_yday)
            {
                continue;
            }

            int triggerMinute = 0;

            switch(ev.trigger)
            {
                case FIXED_TIME:

                    triggerMinute =
                        (ev.hour * 60)
                        + ev.minute;

                    break;

                case SUNRISE:

                    triggerMinute =
                        _astro->getSunriseMinutes()
                        + ev.offsetMinutes;

                    break;

                case SUNSET:

                    triggerMinute =
                        _astro->getSunsetMinutes()
                        + ev.offsetMinutes;

                    break;
            }

            /*
             * Προστασία ορίων
             */
            if(triggerMinute < 0)
            {
                triggerMinute = 0;
            }

            if(triggerMinute > 1439)
            {
                triggerMinute = 1439;
            }

            if(triggerMinute ==
               minuteNow)
            {
                _relays->set(
                    channel,
                    ev.action);

                ev.lastExecutionDay =
                    now.tm_yday;
            }
        }
    }
}