#include "lcd_manager.h"

#include <Arduino.h>
#include <cstring>

#include "../common/event_factory.h"
#include "../common/globals.h"
#include "../common/helpers.h"

static const char* dayPresetToString(uint8_t mask)
{
    if(mask == EVERYDAY_MASK)
        return "EVERYDAY";

    if(mask == WEEKDAYS)
        return "WEEKDAYS";

    if(mask == WEEKEND)
        return "WEEKEND";

    return "CUSTOM";
}

void LCDManager::begin()
{
    lcd.init(18, 19);

    lcd.backlight();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Lighting Controller");

    lcd.setCursor(0, 1);
    lcd.print("Starting...");
}

void LCDManager::clear()
{
    lcd.clear();
}

void LCDManager::printLine(
    uint8_t row,
    const char* text)
{
    char line[21];

    snprintf(
        line,
        sizeof(line),
        "%-20s",
        text);

    lcd.setCursor(0, row);
    lcd.print(line);
}

void LCDManager::showStatus()
{
    tm now =
        TimeMgr.getLocalTimeInfo();

    int sunrise =
        Astro.getSunriseMinutes();

    int sunset =
        Astro.getSunsetMinutes();

    char line[21];

    /*
     * Exactly 20 characters:
     *
     * 22:01 R07:52 S17:11W
     *
     * W = WiFi connected
     * - = WiFi disconnected
     */
    char wifiSymbol =
        WifiMgr.connected()
            ? 'W'
            : '-';

	if(WifiMgr.connected())
	{
		wifiSymbol = 'W';
	}
	else if(WifiMgr.provisioningActive())
	{
		wifiSymbol = 'P';
	}
	else
	{
		wifiSymbol = '-';
	}
    if(TimeMgr.isValid())
    {
        snprintf(
            line,
            sizeof(line),
            "%02d:%02d R%02d:%02d S%02d:%02d%c",
            now.tm_hour,
            now.tm_min,
            sunrise / 60,
            sunrise % 60,
            sunset / 60,
            sunset % 60,
            wifiSymbol);
    }
    else
    {
        snprintf(
            line,
            sizeof(line),
            "--:-- R--:-- S--:--%c",
            wifiSymbol);
    }

    printLine(0, line);

    /*
     * Currently active outputs.
     *
     * Example:
     * ON:1 3 5
     */
    strcpy(
        line,
        "ON:");

    bool first = true;

    for(uint8_t channel = 0;
        channel < MAX_CHANNELS;
        channel++)
    {
        if(Config.config
               .channels[channel]
               .currentState)
        {
            char channelText[4];

            snprintf(
                channelText,
                sizeof(channelText),
                "%s%d",
                first ? "" : " ",
                channel + 1);

            strcat(
                line,
                channelText);

            first = false;
        }
    }

    if(first)
    {
        strcat(
            line,
            "-");
    }

    printLine(1, line);

    /*
     * Channel modes:
     *
     * A = Auto
     * M = Manual ON or OFF
     * D = Disabled
     *
     * Exactly 19 characters:
     * MD:1A2M3M4D5D6A7A8D
     */
    strcpy(
        line,
        "MD:");

    for(uint8_t channel = 0;
        channel < MAX_CHANNELS;
        channel++)
    {
        char modeSymbol = '?';

        switch(Config.config
                   .channels[channel]
                   .mode)
        {
            case AUTO_MODE:

                modeSymbol = 'A';
                break;

            case MANUAL_ON:
            case MANUAL_OFF:

                modeSymbol = 'M';
                break;

            case DISABLED_MODE:

                modeSymbol = 'D';
                break;

            default:

                modeSymbol = '?';
                break;
        }

        char channelMode[4];

        snprintf(
            channelMode,
            sizeof(channelMode),
            "%d%c",
            channel + 1,
            modeSymbol);

        strcat(
            line,
            channelMode);
    }

    printLine(2, line);

    printLine(
        3,
        "OK->MENU");
}

void LCDManager::showChannelList()
{
    uint8_t first =
        (UI.selectedChannel / 4) * 4;

    for(uint8_t i = 0; i < 4; i++)
    {
        uint8_t ch =
            first + i;

        if(ch >= MAX_CHANNELS)
        {
            printLine(i, "");
            continue;
        }

        char line[21];

        snprintf(
            line,
            sizeof(line),
            "%c CH%d %s",
            (ch == UI.selectedChannel) ? '>' : ' ',
            ch + 1,
            modeToString(
                Config.config.channels[ch].mode));

        printLine(i, line);
    }
}

void LCDManager::showChannel()
{
    uint8_t ch =
        UI.selectedChannel;

    ChannelConfig& cfg =
        Config.config.channels[ch];

    char line[21];

    snprintf(
        line,
        sizeof(line),
        "CHANNEL %d",
        ch + 1);

    printLine(0, line);

    snprintf(
        line,
        sizeof(line),
        "[MODE] %s",
        modeToString(
            UI.pendingMode));

    printLine(1, line);

    snprintf(
        line,
        sizeof(line),
        "STATE:%s",
        cfg.currentState ? "ON" : "OFF");

    printLine(2, line);

    if(cfg.mode != UI.pendingMode)
    {
        printLine(3, "OK=APPLY ESC=BACK");
    }
    else
    {
        printLine(3, "OK->EVENTS");
    }
}

void LCDManager::showEventList()
{
    uint8_t ch =
        UI.selectedChannel;

    ChannelConfig& cfg =
        Config.config.channels[ch];

    uint8_t first =
        (UI.selectedEvent / 4) * 4;

    bool addVisible =
        (cfg.eventCount < MAX_EVENTS);

    uint8_t maxItems =
        cfg.eventCount + (addVisible ? 1 : 0);

    for(uint8_t i = 0; i < 4; i++)
    {
        uint8_t item =
            first + i;

        if(item >= maxItems)
        {
            printLine(i, "");
            continue;
        }

        char line[21];

        if(addVisible &&
           item == cfg.eventCount)
        {
            snprintf(
                line,
                sizeof(line),
                "%c + ADD EVENT",
                (item == UI.selectedEvent) ? '>' : ' ');

            printLine(i, line);
            continue;
        }

        ScheduleEvent& ev =
            cfg.events[item];

        snprintf(
            line,
            sizeof(line),
            "%c E%d %s",
            (item == UI.selectedEvent) ? '>' : ' ',
            item + 1,
            triggerToString(
                ev.trigger));

        printLine(i, line);
    }
}

void LCDManager::showEventEditor()
{
    ScheduleEvent& ev =
        Config.config
            .channels[
                UI.selectedChannel]
            .events[
                UI.selectedEvent];

    char line[21];

    snprintf(
        line,
        sizeof(line),
        "EVENT %d",
        UI.selectedEvent + 1);

    printLine(0, line);

    switch(UI.editField)
    {
        case EDIT_TRIGGER:

            snprintf(
                line,
                sizeof(line),
                "[TYPE] %s",
                triggerToString(
                    ev.trigger));

            printLine(1, line);
            break;

        case EDIT_HOUR:

            snprintf(
                line,
                sizeof(line),
                "[HOUR] %02d",
                ev.hour);

            printLine(1, line);
            break;

        case EDIT_MINUTE:

            snprintf(
                line,
                sizeof(line),
                "[MINUTE] %02d",
                ev.minute);

            printLine(1, line);
            break;

        case EDIT_OFFSET:

            snprintf(
                line,
                sizeof(line),
                "[OFFSET] %+d",
                ev.offsetMinutes);

            printLine(1, line);
            break;

        case EDIT_ACTION:

            snprintf(
                line,
                sizeof(line),
                "[ACTION] %s",
                ev.action ? "ON" : "OFF");

            printLine(1, line);
            break;

        case EDIT_DAYS:

            snprintf(
                line,
                sizeof(line),
                "[DAYS] %s",
                dayPresetToString(
                    ev.daysMask));

            printLine(1, line);
            break;

        case EDIT_ENABLED:

            snprintf(
                line,
                sizeof(line),
                "[ENABLE] %s",
                ev.enabled ? "YES" : "NO");

            printLine(1, line);
            break;

        default:

            printLine(1, "");
            break;
    }

    printLine(2, "");

    if(UI.editField == EDIT_DAYS &&
            ev.daysMask != EVERYDAY_MASK &&
            ev.daysMask != WEEKDAYS &&
            ev.daysMask != WEEKEND)
    {
        printLine(3, "OK=EDIT DAYS");
    }
    else
    {
        printLine(3, "UP/DN CHG OK>");
    }
}

void LCDManager::showDaysEditor()
{
    ScheduleEvent& ev =
        Config.config
            .channels[
                UI.selectedChannel]
            .events[
                UI.selectedEvent];

    static const char* names[8] =
    {
        "ALL DAYS",
        "SUN",
        "MON",
        "TUE",
        "WED",
        "THU",
        "FRI",
        "SAT"
    };

    static const uint8_t masks[7] =
    {
        SUNDAY_MASK,
        MONDAY_MASK,
        TUESDAY_MASK,
        WEDNESDAY_MASK,
        THURSDAY_MASK,
        FRIDAY_MASK,
        SATURDAY_MASK
    };

    uint8_t first =
        (UI.selectedDay / 4) * 4;

    for(uint8_t i = 0; i < 4; i++)
    {
        uint8_t item =
            first + i;

        if(item > 7)
        {
            printLine(i, "");
            continue;
        }

        bool selected = false;

        if(item == 0)
        {
            selected =
                (ev.daysMask == EVERYDAY_MASK);
        }
        else
        {
            selected =
                (ev.daysMask &
                 masks[item - 1]);
        }

        char line[21];

        snprintf(
            line,
            sizeof(line),
            "%c %-8s [%c]",
            (item == UI.selectedDay) ? '>' : ' ',
            names[item],
            selected ? 'X' : ' ');

        printLine(i, line);
    }
}
void LCDManager::showEventAction()
{
    ChannelConfig& cfg =
        Config.config.channels[
            UI.selectedChannel];

    if(UI.selectedEvent >=
       cfg.eventCount)
    {
        printLine(0, "INVALID EVENT");
        printLine(1, "");
        printLine(2, "");
        printLine(3, "ESC=BACK");
        return;
    }

    ScheduleEvent& ev =
        cfg.events[
            UI.selectedEvent];

    char line[21];

    snprintf(
        line,
        sizeof(line),
        "EVENT %d %s",
        UI.selectedEvent + 1,
        triggerToString(
            ev.trigger));

    printLine(0, line);

    snprintf(
        line,
        sizeof(line),
        "%c EDIT",
        UI.selectedEventAction ==
            EVENT_ACTION_EDIT
            ? '>'
            : ' ');

    printLine(1, line);

    snprintf(
        line,
        sizeof(line),
        "%c DELETE",
        UI.selectedEventAction ==
            EVENT_ACTION_DELETE
            ? '>'
            : ' ');

    printLine(2, line);

    snprintf(
        line,
        sizeof(line),
        "%c BACK",
        UI.selectedEventAction ==
            EVENT_ACTION_BACK
            ? '>'
            : ' ');

    printLine(3, line);
}