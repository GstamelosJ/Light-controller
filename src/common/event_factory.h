#pragma once

#include "types.h"

constexpr uint8_t WEEKDAYS =
    MONDAY_MASK |
    TUESDAY_MASK |
    WEDNESDAY_MASK |
    THURSDAY_MASK |
    FRIDAY_MASK;

constexpr uint8_t WEEKEND =
    SATURDAY_MASK |
    SUNDAY_MASK;
inline ScheduleEvent createSunsetEvent(
        int offsetMinutes,
        bool action)
{
    ScheduleEvent ev;

    ev.enabled = true;

    ev.trigger = SUNSET;

    ev.daysMask = EVERYDAY_MASK;

    ev.hour = 0;
    ev.minute = 0;

    ev.offsetMinutes = offsetMinutes;

    ev.action = action;

    ev.lastExecutionDay = -1;

    return ev;
}
inline ScheduleEvent createFixedEvent(
        int hour,
        int minute,
        bool action)
{
    ScheduleEvent ev;

    ev.enabled = true;

    ev.trigger = FIXED_TIME;

    ev.daysMask = EVERYDAY_MASK;

    ev.hour = hour;

    ev.minute = minute;

    ev.offsetMinutes = 0;

    ev.action = action;

    ev.lastExecutionDay = -1;

    return ev;
}
inline ScheduleEvent createWeekEvent(
    TriggerType trigger,
    uint8_t daysMask,
    int hour,
    int minute,
    int offsetMinutes,
    bool action)
{
    ScheduleEvent ev;

    ev.enabled = true;

    ev.trigger = trigger;

    ev.daysMask = daysMask;

    ev.hour = hour;

    ev.minute = minute;

    ev.offsetMinutes = offsetMinutes;

    ev.action = action;

    ev.lastExecutionDay = -1;

    return ev;
}
	