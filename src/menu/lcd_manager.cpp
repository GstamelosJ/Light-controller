#include "lcd_manager.h"

#include "../common/event_factory.h"
#include "../common/globals.h"
#include "../common/helpers.h"

void LCDManager::begin()
{
    lcd.init(18,19);

    lcd.backlight();

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Lighting Controller");

    lcd.setCursor(0,1);
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

    lcd.setCursor(0,row);
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

    uint8_t activeChannels = 0;

    for(int i=0;i<MAX_CHANNELS;i++)
    {
        if(Config.config.channels[i]
            .currentState)
        {
            activeChannels++;
        }
    }

    char line[21];

    snprintf(
        line,
        sizeof(line),
        "TIME %02d:%02d",
        now.tm_hour,
        now.tm_min);

    printLine(0,line);

    snprintf(
        line,
        sizeof(line),
        "SR %02d:%02d SS %02d:%02d",
        sunrise/60,
        sunrise%60,
        sunset/60,
        sunset%60);

    printLine(1,line);

    snprintf(
        line,
        sizeof(line),
        "CH ON : %d",
        activeChannels);

    printLine(2,line);

    printLine(
        3,
        "OK->Channels");
}

void LCDManager::showChannelList()
{
    uint8_t first =
        (UI.selectedChannel / 4) * 4;

    for(uint8_t i=0;i<4;i++)
    {
        uint8_t ch =
            first + i;

        char line[21];

        if(ch >= MAX_CHANNELS)
        {
            printLine(i,"");
            continue;
        }

        snprintf(
            line,
            sizeof(line),
            "%cCH%d %s",
            (ch == UI.selectedChannel) ?
                '>' : ' ',
            ch + 1,
            modeToString(
                Config.config
                      .channels[ch]
                      .mode));

        printLine(i,line);
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

    printLine(0,line);

    snprintf(
    line,
    sizeof(line),
    "MODE:%s",
    modeToString(UI.pendingMode));

    printLine(1,line);

    snprintf(
        line,
        sizeof(line),
        "STATE:%s",
        cfg.currentState ?
        "ON" :
        "OFF");

    printLine(2,line);

    printLine(
        3,
        "OK->EVENTS");
}

void LCDManager::showEventList()
{
    uint8_t ch =
        UI.selectedChannel;

    ChannelConfig& cfg =
        Config.config.channels[ch];

    uint8_t first =
        (UI.selectedEvent / 4) * 4;

    for(uint8_t i=0;i<4;i++)
    {
        uint8_t ev =
            first + i;

        char line[21];

        if(ev >= cfg.eventCount)
        {
            printLine(i,"");
            continue;
        }

        ScheduleEvent& e =
            cfg.events[ev];

        snprintf(
            line,
            sizeof(line),
            "%cE%d %s",
            (ev == UI.selectedEvent) ?
                '>' : ' ',
            ev + 1,
            triggerToString(
                e.trigger));

        printLine(i,line);
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

    printLine(0,line);

    switch(UI.editField)
    {
        case EDIT_TRIGGER:

            snprintf(
                line,
                sizeof(line),
                "TYPE:%s",
                triggerToString(
                    ev.trigger));

            printLine(1,line);

            break;

        case EDIT_HOUR:

            snprintf(
                line,
                sizeof(line),
                "HOUR:%02d",
                ev.hour);

            printLine(1,line);

            break;

        case EDIT_MINUTE:

            snprintf(
                line,
                sizeof(line),
                "MIN:%02d",
                ev.minute);

            printLine(1,line);

            break;

        case EDIT_OFFSET:

            snprintf(
                line,
                sizeof(line),
                "OFFSET:%d",
                ev.offsetMinutes);

            printLine(1,line);

            break;

        case EDIT_ACTION:

            snprintf(
                line,
                sizeof(line),
                "ACTION:%s",
                ev.action ?
                    "ON" :
                    "OFF");

            printLine(1,line);

            break;

        case EDIT_DAYS:

            if(ev.daysMask ==
               EVERYDAY_MASK)
            {
                printLine(
                    1,
                    "DAYS:EVERYDAY");
            }
            else if(ev.daysMask ==
                    WEEKDAYS)
            {
                printLine(
                    1,
                    "DAYS:WEEKDAYS");
            }
            else if(ev.daysMask ==
                    WEEKEND)
            {
                printLine(
                    1,
                    "DAYS:WEEKEND");
            }
            else
            {
                printLine(
                    1,
                    "DAYS:CUSTOM");
            }

            break;

        case EDIT_ENABLED:

            snprintf(
                line,
                sizeof(line),
                "ENABLE:%s",
                ev.enabled ?
                    "YES" :
                    "NO");

            printLine(1,line);

            break;
    }

    printLine(
        2,
        "");

    printLine(
        3,
        "UP/DN CHG  OK>");
}
