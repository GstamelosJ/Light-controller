#pragma once
#include "../common/types.h"
#include <stdint.h>

enum UIScreen
{
    UI_STATUS,

    UI_CHANNEL_LIST,

    UI_CHANNEL,

    UI_EVENT_LIST,

    UI_EVENT_EDIT
};

enum EventEditField
{
    EDIT_TRIGGER,

    EDIT_HOUR,

    EDIT_MINUTE,

    EDIT_OFFSET,

    EDIT_ACTION,

    EDIT_DAYS,

    EDIT_ENABLED
};

struct UIContext
{
    UIScreen currentScreen =
        UI_STATUS;
        
    UIScreen lastScreen =
        UI_STATUS;

    uint8_t selectedChannel = 0;

    uint8_t selectedEvent = 0;

    EventEditField editField =
        EDIT_TRIGGER;

    ChannelMode pendingMode =
        AUTO_MODE;
};





