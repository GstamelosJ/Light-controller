#pragma once

#include <Arduino.h>

#define MAX_CHANNELS 8
#define MAX_EVENTS   16

enum TriggerType
{
    FIXED_TIME,
    SUNRISE,
    SUNSET
};

enum ChannelMode
{
    AUTO_MODE,
    MANUAL_ON,
    MANUAL_OFF,
    DISABLED_MODE
};

enum DaysMask
{
    MONDAY_MASK    = 0x01,
    TUESDAY_MASK   = 0x02,
    WEDNESDAY_MASK = 0x04,
    THURSDAY_MASK  = 0x08,
    FRIDAY_MASK    = 0x10,
    SATURDAY_MASK  = 0x20,
    SUNDAY_MASK    = 0x40,

    EVERYDAY_MASK  = 0x7F
};

struct ScheduleEvent
{
    bool enabled;

    TriggerType trigger;

    uint8_t daysMask;

    uint8_t hour;

    uint8_t minute;

    int16_t offsetMinutes;

    bool action;

    int16_t lastExecutionDay;
};

struct ChannelConfig
{
    bool enabled;

    ChannelMode mode;

    bool currentState;

    uint8_t relayPin;

    uint8_t eventCount;

    ScheduleEvent events[MAX_EVENTS];
};

struct LocationConfig
{
    double latitude;
    double longitude;
};

struct NetworkConfig
{
    char wifiSSID[32];

    char wifiPASS[64];

    char apn[32];

    char apnUser[32];

    char apnPass[32];
};

struct SystemConfig
{
    LocationConfig location;

    NetworkConfig network;

    ChannelConfig channels[MAX_CHANNELS];
};